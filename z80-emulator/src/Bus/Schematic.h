#pragma once

#include "src/Bus/Datasheet.h"
#include "src/Bus/Library.h"
#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Menu.h"
#include "src/Window/Window.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstdio>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>


namespace Bus {

// What the schematic draws: chips (one Datasheet each, from Library) placed on a sheet and wired together.
// The Lua file lists parts, nets and notes and merges SCHEMATIC_LAYOUT; Load() calls SCHEMATIC.build() and reads what it left:
//   parts  { id = "D5", type = "AND", title = "D5 AND", x =, y =, bx =, by = }   type = name of a datasheet (Library::Find);
//                                                          x,y = position now, bx,by = position written in the Lua file ("Reset moves");
//                                                          a part with no bx,by was not placed there: Load() parks it in a column to the right
//   nets   a map: net name -> { pins = { "D1.RD", "D2.OE", ... }, routes = { { from = "D1.RD", to = "D2.OE", dx = 150 }, ... } }
//            * the name is the wire id used by Set / Bind / Hit; every pin of a net carries the same signal
//            * a route is drawn from its `from` pin to its `to` pin (dx = distance of the vertical leg from `from`, optional)
//            * a pin that is in no route is drawn as a tag: a short stub with the net name next to it
//   notes  { x =, y =, text = "..." }
// Routing and parking are done here, not in Lua: C++ knows the symbol sizes, and wires follow a chip that is dragged.
class Schematic : public Window::KeyEventListener, public Window::Window {
  struct PinPos { std::string name; bool right; olc::vi2d inner, outer; };      // relative to the top-left corner of the box; wires attach at `outer`
  struct PartT  { std::string id, title; Datasheet sheet; olc::vf2d pos; olc::vi2d size; std::vector<PinPos> pins; };   // sheet = a copy of the Library's, named `title`
  struct RouteT { std::string from, to; bool hasDx = false; float dx = 0; };      // pin keys "D1.RD"
  struct LineT  { std::vector<olc::vf2d> pts; bool tag = false; };                // one drawn polyline (a route, or the stub of a tag)
  struct LabelT { std::string text; olc::vf2d pos; };
  struct NetT   {
    std::string name;
    std::vector<std::string> pins, tags;                                          // all pins / the pins that are in no route
    std::vector<RouteT> routes;
    std::vector<LineT> lines; std::vector<LabelT> labels;                         // filled by Resolve()
  };
  struct NoteT  { olc::vf2d pos; std::string text; };

public:
  Schematic(PixelGameEngine* engine, std::string id): Window::Base(engine), Window::Window(engine, std::move(id)), ::Window::KeyEventListener(engine) {
    auto src = GameEngine->lua.GetOr("SCHEMATIC", "");
    if (lua.DoFile(src)) Load();

    menu = std::make_unique<::Window::Menu>(
      engine,
      ::Window::MenuT::Init({
      { "Layout", {
        { "Print moved parts", [this](void*) { PrintLayout(false); return nullptr; } },
        { "Print all parts",   [this](void*) { PrintLayout(true);  return nullptr; } },
        { "Reset moves",       [this](void*) { ResetMoves();       return nullptr; } },
      } },
      { "View", {
        { "Fit",        [this](void*) { Fit();                    return nullptr; } },
        { "Colour wires by level", [this](void*) { tintWires = !tintWires; return nullptr; } },
      } },
    }));
  }

  // ---------------------------------------------------------------- live signals
  void Bind(const std::string& net, std::function<bool()> fn) { signals[net] = std::move(fn); }

  // one bit of a register; active-low latches ("0 = requested") are shown as asserted when the bit is 0
  void BindBit(const std::string& net, std::function<uint8_t()> reg, int bit, bool activeLow = false) {
    signals[net] = [reg = std::move(reg), bit, activeLow] { const bool b = (reg() >> bit) & 1; return activeLow ? !b : b; };
  }

  // push the level of a wire from C++:  Set("INT7", true)  ->  the wire turns green, false -> red, Clear() -> back to plain teal.
  // Safe from any thread (the emulator thread can call it every clock); returns false if there is no wire with that id.
  // A wire with a level is drawn green (1) / red (0); hover or select it to see the 0 / 1 written next to it. Bind()/BindBit() are polled once per frame and win over a pushed level; Hit() shows a 1 on top of both.
  bool Set(const std::string& wire, bool level) { auto it = states.find(wire); if (it == states.end()) return false; it->second.store(level ? 1 : 0); return true; }
  bool Clear(const std::string& wire) { auto it = states.find(wire); if (it == states.end()) return false; it->second.store(kUnset); return true; }

  // a bus: SetBus("A", 0x1234, 16) sets A0..A15, one bit each (bits that are no wire on the sheet are skipped)
  void SetBus(const std::string& prefix, uint32_t value, int bits) { for (int i = 0; i < bits; i++) Set(prefix + std::to_string(i), (value >> i) & 1); }

  // -1 = not set, 0 = low, 1 = high (what Set() stored; not the Bind() result)
  int State(const std::string& wire) const { auto it = states.find(wire); return it == states.end() ? kUnset : it->second.load(); }

  // one-clock pulses (IO8, KEYBOARD, ...) are far too short to see; Hit() keeps the wire lit for `frames` frames
  void Strobe(const std::string& net) { strobes.try_emplace(net).first->second.store(0); }   // UI thread, once, before the CPU runs
  void Hit(const std::string& net, int frames = 30) { auto it = strobes.find(net); if (it != strobes.end()) it->second.store(frames); }   // any thread (map is not modified)

  // ---------------------------------------------------------------- loading

  bool Load(const std::string& root = "SCHEMATIC") {
    if (lua.TypeOf(root) != "table") return false;

    std::vector<PartT> newParts; std::vector<NetT> newNets; std::vector<NoteT> newNotes;
    std::map<std::string, olc::vf2d> newBase;
    std::vector<std::pair<size_t, bool>> unplaced;                                // (part index, has no position at all)
    std::map<std::string, std::set<std::string>> newPins;

    for (size_t i = 1, n = lua.Length(root + ".parts"); i <= n; i++) {
      const std::string p = root + ".parts." + std::to_string(i);
      const std::string id = lua.GetOr(p + ".id", ""), type = lua.GetOr(p + ".type", "");
      const Datasheet* ds = Library::Find(type);
      if (!ds) { Warn("unknown part type \"" + type + "\""); continue; }

      PartT part; part.id = id;
      part.title = lua.GetOr(p + ".title", id);
      part.sheet = *ds; part.sheet.name = part.title;                              // the symbol writes the datasheet name above the box: make it "D5 AND"
      part.size = SymbolSize(part.sheet); part.pins = PinsOf(part.sheet);
      const auto x = lua.Get<float>(p + ".x"), y = lua.Get<float>(p + ".y"), bx = lua.Get<float>(p + ".bx"), by = lua.Get<float>(p + ".by");
      if (x && y) part.pos = { *x, *y };
      if (bx && by) newBase[id] = { *bx, *by };                                   // placed in the Lua file
      else unplaced.push_back({ newParts.size(), !(x && y) });                    // ... or not: parked below, once every size is known
      newParts.push_back(std::move(part));
    }

    float parkX = 200.f, parkY = 0.f;                                             // chips with no position: a column to the right of the placed ones
    for (const auto& p : newParts) if (const auto b = newBase.find(p.id); b != newBase.end()) parkX = std::max(parkX, b->second.x + p.size.x + 200.f);
    for (const auto& [i, noPos] : unplaced) {
      auto& p = newParts[i];
      newBase[p.id] = { parkX, parkY };
      if (noPos) p.pos = newBase[p.id];                                           // (a chip that is only in SCHEMATIC_LAYOUT keeps that position)
      parkY += p.size.y + 40.f;
    }

    // "ID.PIN" -> (part, pin), the same key as a net uses (marks stripped)
    std::map<std::string, std::pair<size_t, size_t>> newIndex;
    for (size_t i = 0; i < newParts.size(); i++) for (size_t k = 0; k < newParts[i].pins.size(); k++)
      newIndex.emplace(newParts[i].id + "." + StripMarks(newParts[i].pins[k].name), std::make_pair(i, k));

    std::set<std::string> reported;                                               // each bad pin is reported once
    std::map<std::string, std::string> owner;                                     // pin key -> the net it is in (a pin is in one net)
    auto names = lua.Keys(root + ".nets");
    std::sort(names.begin(), names.end());                                        // Lua's table order is arbitrary, the drawing should not be
    for (const auto& name : names) {
      const std::string p = root + ".nets." + name;
      NetT net; net.name = name;

      const auto pinOf = [&](const std::string& ref) -> std::string {            // "D5.<A" -> "D5.A", added to the net once; "" if there is no such pin
        const auto dot = ref.find('.');
        const std::string key = dot == std::string::npos ? "" : ref.substr(0, dot) + "." + StripMarks(ref.substr(dot + 1));
        if (!newIndex.count(key)) { if (reported.insert(name + " " + key).second) Warn("net " + name + ": no pin \"" + ref + "\""); return ""; }
        if (newPins[name].insert(key).second) {
          if (const auto o = owner.emplace(key, name); !o.second) Warn("pin " + key + " is in two nets: " + o.first->second + " and " + name);
          net.pins.push_back(key);
        }
        return key;
      };

      if (const auto pins = lua.Get<std::vector<std::string>>(p + ".pins")) for (const auto& ref : *pins) pinOf(ref);
      std::set<std::string> routed;
      for (size_t j = 1, m = lua.Length(p + ".routes"); j <= m; j++) {
        const std::string rp = p + ".routes." + std::to_string(j);
        RouteT r; r.from = pinOf(lua.GetOr(rp + ".from", "")); r.to = pinOf(lua.GetOr(rp + ".to", ""));
        if (r.from.empty() || r.to.empty() || r.from == r.to) continue;
        if (const auto dx = lua.Get<float>(rp + ".dx")) r.hasDx = true, r.dx = *dx;
        routed.insert(r.from); routed.insert(r.to);
        net.routes.push_back(std::move(r));
      }
      for (const auto& key : net.pins) if (!routed.count(key)) net.tags.push_back(key);
      newNets.push_back(std::move(net));
    }

    for (size_t i = 1, n = lua.Length(root + ".notes"); i <= n; i++) {
      const std::string p = root + ".notes." + std::to_string(i);
      NoteT note; note.text = lua.GetOr(p + ".text", ""); note.pos = { lua.GetOr(p + ".x", 0.f), lua.GetOr(p + ".y", 0.f) };
      newNotes.push_back(std::move(note));
    }

    parts = std::move(newParts); nets = std::move(newNets); notes = std::move(newNotes); netPins = std::move(newPins);
    base = std::move(newBase); pinIndex = std::move(newIndex);
    hoverNet.clear(); hoverPart = -1; drag = -1; if (!netPins.count(selectedNet)) selectedNet.clear();
    for (const auto& [wire, pins] : netPins) states.try_emplace(wire).first->second.store(kUnset);   // (a reload must not run while another thread calls Set)
    Resolve();
    return true;
  }

  // ---------------------------------------------------------------- layout (what you paste into Lua)
  std::vector<std::string> LayoutLines(bool all) const {
    std::vector<std::string> out; int count = 0;
    for (const auto& p : parts) {
      const auto b = base.find(p.id);
      if (!all && b != base.end() && b->second == p.pos) continue;
      char buf[160]; const std::string key = IsIdent(p.id) ? p.id : "[\"" + p.id + "\"]";
      std::snprintf(buf, sizeof buf, "  %s = { x = %g, y = %g },", key.c_str(), p.pos.x, p.pos.y);
      out.push_back(buf); count++;
    }
    out.insert(out.begin(), "SCHEMATIC_LAYOUT = {");
    out.insert(out.begin(), std::string("-- paste into config.lua (pixels): ") + std::to_string(count) + (all ? " part(s)" : " moved part(s)"));
    out.push_back("}");
    return out;
  }

  void PrintLayout(bool all) { for (const auto& l : LayoutLines(all)) std::puts(l.c_str()); std::fflush(stdout); }

  void ResetMoves() { for (auto& p : parts) if (auto b = base.find(p.id); b != base.end()) p.pos = b->second; Resolve(); }

  // ---------------------------------------------------------------- view
  // the view is only panned (pixels): drag, two-finger scroll (wheel), Shift + wheel for sideways, H J K L
  void Fit() {
    if (parts.empty()) return;
    olc::vf2d lo = parts[0].pos;
    for (const auto& p : parts) lo = { std::min(lo.x, p.pos.x), std::min(lo.y, p.pos.y) };
    offset = olc::vf2d(kStub + 24.f, 36.f) - lo;
  }

  bool IsMouseOver() const {
    const olc::vf2d mouse = GameEngine->GetMousePos(), pos = Origin(), area = Area();
    return !(mouse.x < pos.x || mouse.y < pos.y || mouse.x >= pos.x + area.x || mouse.y >= pos.y + area.y);
  }

  inline void Focus(bool state) override { ::Window::KeyEventListener::Focus(state); menu->Focus(false); }

  // ---------------------------------------------------------------- frame
  void Preprocess() override {
    if (IsMouseOver()) {
      if (const int wheel = GameEngine->GetMouseWheelH()) offset.x -= wheel / 120.f * kWheelPx;   // swipe right = view moves right
      if (const int wheel = GameEngine->GetMouseWheel()) offset.y += wheel / 120.f * kWheelPx;
    }

    menu->Preprocess();
  }

  void Process() override {
    menu->Process();

    values.clear();                                                               // sample every signal once per frame: pushed levels, then bound ones, then strobes
    for (auto& [name, st] : states) if (const int v = st.load(); v != kUnset) values[name] = v;
    for (auto& [name, fn] : signals) values[name] = fn() ? 1 : 0;
    for (auto& [name, cnt] : strobes) { int v = cnt.load(); if (v > 0) { values[name] = 1; cnt.compare_exchange_strong(v, v - 1); } }

    const olc::vf2d mouse = GameEngine->GetMousePos();
    if (menu->bEnabled) { prevMouse = mouse; return; }                            // the menu owns the mouse

    const olc::vf2d world = ToWorld(mouse);

    if (drag >= 0) {                                                              // moving a part (keeps working outside the window)
      if (GameEngine->GetMouse(0).bHeld) {
        const olc::vf2d want = Snap(world - grab);
        if (want != parts[drag].pos) parts[drag].pos = want, Resolve();
      } else drag = -1;
    }

    if (IsMouseOver() && drag < 0) {

      if (GameEngine->GetMouse(0).bHeld || GameEngine->GetMouse(2).bHeld) offset += mouse - prevMouse;

      hoverNet.clear(); hoverPart = -1;
      for (int i = (int)parts.size() - 1; i >= 0; i--) {                          // last drawn = on top
        const auto& p = parts[i];
        if (world.x >= p.pos.x && world.y >= p.pos.y && world.x < p.pos.x + p.size.x && world.y < p.pos.y + p.size.y) { hoverPart = i; break; }
      }
      if (hoverPart < 0) {
        float best = 5.f;                                                          // 5 pixels
        for (const auto& n : nets) for (const auto& l : n.lines) for (size_t i = 1; i < l.pts.size(); i++) {
          const float d = DistSeg(world, l.pts[i - 1], l.pts[i]);
          if (d < best) best = d, hoverNet = n.name;
        }
      }

      if (GameEngine->GetMouse(0).bPressed) {
        if (hoverPart >= 0 && edit) drag = hoverPart, grab = world - parts[hoverPart].pos;
        else selectedNet = hoverNet;                                              // click empty space -> clear
      }
    } else if (drag < 0) {
      hoverNet.clear(); hoverPart = -1;
    }
    prevMouse = mouse;
  }

  // The scene is drawn 1:1 into an off-screen sprite (olc has no scissor, so this is what clips the symbols to the window),
  // then the sprite is blitted into the window.
  void Draw() override {
    const olc::vi2d lo = absolute + vOffset, hi = absolute + size - olc::vi2d(1, 1);
    const olc::vi2d view = olc::vi2d(Area());
    const olc::vi2d scene(std::max(1, view.x), std::max(1, view.y - kStatusH));

    for (size_t i = 0; i < parts.size(); i++) {
      auto& p = parts[i];
      const bool moved = base.count(p.id) && base.at(p.id) != p.pos;
      const olc::Pixel col = (int)i == drag ? olc::WHITE : (int)i == hoverPart ? olc::YELLOW : moved ? olc::Pixel(220, 160, 60) : Color::GREY;
      p.sheet.Symbol(GameEngine, I(p.pos + offset), col);
    }

    // nets: normal first, highlighted on top
    for (int pass = 0; pass < 2; pass++) for (const auto& n : nets) {
      const bool lit = n.name == hoverNet || n.name == selectedNet;
      if (lit != (pass == 1)) continue;
      const olc::Pixel col = NetColor(n.name);
      for (const auto& l : n.lines) for (size_t i = 1; i < l.pts.size(); i++) GameEngine->DrawLine(I(l.pts[i - 1] + offset), I(l.pts[i] + offset), col);
    }

    for (const auto& t : notes) GameEngine->DrawString(I(t.pos + offset), t.text, olc::Pixel(230, 220, 100));
    for (const auto& n : nets) for (const auto& l : n.labels) GameEngine->DrawString(I(l.pos + offset), l.text, olc::Pixel(230, 220, 100));

    // the 0 / 1 is written only for the hovered or selected wire (that wire is drawn yellow / white, so the digit carries the level):
    // at both ends of a routed wire, at the pin of a tag
    for (const auto& n : nets) {
      if (n.name != hoverNet && n.name != selectedNet) continue;
      const auto v = values.find(n.name);
      if (v == values.end()) continue;
      for (const auto& l : n.lines) {
        const auto& pts = l.pts; const size_t last = pts.size() - 1;
        DrawLevel(pts[0], pts[1].x - pts[0].x, v->second);
        if (!l.tag) DrawLevel(pts[last], pts[last - 1].x - pts[last].x, v->second);
      }
    }

    // status bar
    const int rows = (int)((size.x - vOffset.x) / 8);
    std::string line; olc::Pixel lineCol(200, 200, 200);
    if (!hoverNet.empty() || !selectedNet.empty()) {
      const std::string& n = hoverNet.empty() ? selectedNet : hoverNet;
      const auto v = values.find(n);
      line = n + (v == values.end() ? "" : v->second ? " = 1" : " = 0") + "  |";
      if (auto it = netPins.find(n); it != netPins.end()) for (const auto& pin : it->second) line += " " + pin;
    } else if (hoverPart >= 0 || drag >= 0) {
      const auto& p = parts[drag >= 0 ? drag : hoverPart]; char pos[48]; std::snprintf(pos, sizeof pos, "  (%g, %g)", p.pos.x, p.pos.y);
      line = p.title + pos;
    }
    if ((int)line.size() > rows) line.resize(rows);
    GameEngine->DrawString(olc::vi2d(lo.x + 2, hi.y - 9), line, lineCol);
    if (edit) GameEngine->DrawString(olc::vi2d(hi.x - 8 * 4 - 4, lo.y + 2), "EDIT", olc::Pixel(220, 160, 60));

    menu->Draw();
  }

public:
  ::Window::StrokeAction Process(Int2Type<olc::Key::ESCAPE>) { return menu->Reset(), menu->Focus(false), nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::OEM_2>) { return menu->bEnabled = true, menu->Focus(true), nullptr; }

  ::Window::StrokeAction Process(Int2Type<olc::Key::H>) { return menu->bEnabled ? nullptr : (offset.x += kPanStep, nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) { return menu->bEnabled ? nullptr : (offset.x -= kPanStep, nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) { return menu->bEnabled ? nullptr : (offset.y += kPanStep, nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) { return menu->bEnabled ? nullptr : (offset.y -= kPanStep, nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::K0>) { return menu->bEnabled ? nullptr : (Fit(), nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::F>)  { return menu->bEnabled ? nullptr : (Fit(), nullptr); }

  const ::Window::StrokeT<>& GetCommands() const override { return Schematic::commands; }

private:
  // ---------------------------------------------------------------- helpers
  olc::vf2d Area() const { return olc::vf2d(size - vOffset); }                    // drawable content area
  olc::vf2d Origin() const { return olc::vf2d(absolute + vOffset); }
  olc::vf2d ToScreen(olc::vf2d w) const { return Origin() + w + offset; }
  olc::vf2d ToWorld(olc::vf2d s) const { return s - Origin() - offset; }
  olc::vf2d Snap(olc::vf2d w) const { return snap > 0.f ? olc::vf2d(std::round(w.x / snap) * snap, std::round(w.y / snap) * snap) : w; }
  static std::string StripMarks(std::string s) {                                  // "<A", "A>", ">A", "/A", "<A>" -> "A"
    const auto mark = [](char c) { return c == '<' || c == '>' || c == '/'; };
    while (!s.empty() && mark(s.front())) s.erase(0, 1);
    while (!s.empty() && mark(s.back())) s.pop_back();
    return s;
  }
  static bool IsIdent(const std::string& s) { if (s.empty() || std::isdigit((unsigned char)s[0])) return false; for (char c : s) if (!std::isalnum((unsigned char)c) && c != '_') return false; return true; }
  static void Warn(const std::string& s) { std::fprintf(stderr, "schematic: %s\n", s.c_str()); }
  static olc::vi2d I(olc::vf2d v) { return olc::vi2d((int)std::lround(v.x), (int)std::lround(v.y)); }

  // Geometry of Datasheet::Symbol (8x12 px cells, 40 px pins, one spare row, each side centred): the symbol draws itself, but the schematic also needs the box size
  // and where each pin ends to hang wires on it. Keep these numbers equal to the ones in Datasheet::Symbol.
  static constexpr int kCharW = 8, kRowH = 12, kStub = 40;
  static olc::vi2d SymbolSize(const Datasheet& d) { return (d.GetSize() + olc::vi2d(1, 1)) * olc::vi2d(kCharW, kRowH); }
  static std::vector<PinPos> PinsOf(const Datasheet& d) {                         // "_" takes a row but has no pin
    const olc::vi2d size = SymbolSize(d);
    int rows[2] = { 0, 0 }, row[2] = { 0, 0 };
    for (const auto& pin : d.pins) rows[pin.orientation]++;                       // each side is centred in the box, like Datasheet::Symbol does
    std::vector<PinPos> out;
    for (const auto& pin : d.pins) {
      const int r = row[pin.orientation]++;
      if (pin.name == "_") continue;
      const bool right = pin.orientation == Datasheet::RIGHT;
      const olc::vi2d inner(right ? size.x + 1 : -1, (size.y - rows[pin.orientation] * kRowH) / 2 + kRowH * r + 2 + 3);
      out.push_back({ pin.name, right, inner, olc::vi2d(inner.x + (right ? kStub : -kStub), inner.y) });
    }
    return out;
  }

  struct PinAtT { olc::vf2d pos; bool right; };                                   // where a pin's wire attaches, and which way the stub points
  PinAtT PinAt(const std::string& key) const {
    const auto it = pinIndex.find(key); if (it == pinIndex.end()) return { { 0.f, 0.f }, true };
    const auto& p = parts[it->second.first]; const auto& pp = p.pins[it->second.second];
    return { p.pos + olc::vf2d(pp.outer), pp.right };
  }

  void Resolve() {                                                                // after load or after a part moved: route every net
    for (auto& n : nets) {
      n.lines.clear(); n.labels.clear();
      for (const auto& r : n.routes) {                                            // out of the pin, a vertical leg, into the other pin
        const auto a = PinAt(r.from), b = PinAt(r.to);
        LineT l; l.pts.push_back(a.pos);
        if (a.pos.y != b.pos.y) {
          float leg = (a.pos.x + b.pos.x) / 2.f;                                  // default: half way
          if (a.right && b.right) leg = std::max(a.pos.x, b.pos.x) + kLeg;       // both stubs point right
          else if (!a.right && !b.right) leg = std::min(a.pos.x, b.pos.x) - kLeg;   // both point left
          if (r.hasDx) leg = a.pos.x + r.dx;                                      // explicit: distance from the first pin
          l.pts.push_back({ leg, a.pos.y }); l.pts.push_back({ leg, b.pos.y });
        }
        l.pts.push_back(b.pos);
        n.lines.push_back(std::move(l));
      }
      for (const auto& key : n.tags) {                                            // tag: a stub with the net name at its end
        const auto a = PinAt(key);
        n.lines.push_back({ { a.pos, a.pos + olc::vf2d(a.right ? kTagLen : -kTagLen, 0.f) }, true });
        n.labels.push_back({ n.name, a.pos + olc::vf2d(a.right ? kTagLen + 3.f : -kTagLen - 3.f - 8.f * (float)n.name.size(), -4.f) });
      }
    }
  }

  static float DistSeg(olc::vf2d p, olc::vf2d a, olc::vf2d b) {
    const olc::vf2d ab = b - a; const float l2 = ab.dot(ab);
    const float t = l2 > 0 ? std::clamp((p - a).dot(ab) / l2, 0.f, 1.f) : 0.f;
    return (p - (a + ab * t)).mag();
  }

  static olc::Pixel LevelColor(int level) { return level ? olc::Pixel(70, 230, 110) : olc::Pixel(230, 80, 80); }   // 1 = green, 0 = red

  // "0" / "1" just above the wire, right next to the pin; `towards` = which way the wire runs from `at` (decides the side of the pin it is written on)
  void DrawLevel(olc::vf2d at, float towards, int level) const {
    const olc::vf2d pos(towards >= 0.f ? at.x + 3.f : at.x - 3.f - 8.f, at.y - 10.f);
    GameEngine->DrawString(I(pos + offset), level ? "1" : "0", LevelColor(level));
  }

  olc::Pixel NetColor(const std::string& n) const {
    if (n == selectedNet) return olc::WHITE;
    if (n == hoverNet) return olc::YELLOW;
    if (tintWires) if (auto it = values.find(n); it != values.end()) return LevelColor(it->second);   // 1 = green, 0 = red
    return olc::Pixel(60, 150, 140);                                              // no level (never Set / Bound): plain teal
  }

private:
  static constexpr float kPanStep = 48.f, kWheelPx = 30.f, kTagLen = 12.f, kLeg = 16.f;   // tag stub length / how far a leg sticks out beyond two pins that face the same way
  static constexpr int kUnset = -1;
  static constexpr int kStatusH = 12;                               // bottom 12 px belong to the status bar
  const olc::vi2d vOffset = olc::vi2d(2, 14);

  LuaScript lua;

  std::vector<PartT> parts; std::vector<NetT> nets; std::vector<NoteT> notes;
  std::map<std::string, std::set<std::string>> netPins;
  std::map<std::string, olc::vf2d> base;                                          // positions as written in SCHEMATIC.layout
  std::map<std::string, std::pair<size_t, size_t>> pinIndex;                      // "ID.PIN" -> (part, pin)

  std::unordered_map<std::string, std::function<bool()>> signals;
  std::unordered_map<std::string, std::atomic<int>> strobes;
  std::unordered_map<std::string, std::atomic<int>> states;                       // one per wire id, created in Load() and never modified after, so Set() is thread safe
  std::unordered_map<std::string, int> values;                                    // sampled once per frame in Process()

  // std::unique_ptr<olc::Sprite> layer;                                             // the scene, drawn off-screen so that it is clipped to the window
  olc::vf2d offset = { 40.f, 40.f }, prevMouse = { 0.f, 0.f }, grab = { 0.f, 0.f };   // where the scene is scrolled to, in pixels
  float snap = 4.f;
  bool edit = true, tintWires = true; int drag = -1;
  std::string hoverNet, selectedNet; int hoverPart = -1;

  static const ::Window::StrokeT<> commands;
};

inline const Window::StrokeT<> Schematic::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::H>(),      [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::J>(),      [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::K>(),      [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::L>(),      [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::K0>(),     [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::F>(),      [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::ESCAPE>(), [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::SHIFT>(), {
    { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  }}
});

};