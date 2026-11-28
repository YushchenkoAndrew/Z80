#pragma once

#include "src/Bus/Datasheet.h"
#include "src/Bus/Library.h"
#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Menu.h"
#include "src/Window/Window.h"
#include <algorithm>
#include <atomic>
#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
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
  struct Net;
  struct Node { size_t chip, pin; };
  struct Box { olc::vi2d a, b, tip; };                                              // the name's rectangle, and the tip of the pin it belongs to
  struct Tag { olc::vi2d pos; int32_t w; olc::vi2d tip; };                          // top-left of the name, its width, the tip of its pin
  struct Route { std::string from, to; float dx = 0; };
  struct Port { olc::vi2d pos; bool right; int32_t top, bottom; };   // free end of the stub, which way it points, the chip's top / bottom edge
  struct Chip { std::string id; olc::vi2d pos; const Datasheet* datasheet; std::vector<const Net*> tags; };

  struct Net   {
    std::string name, id;
    std::vector<std::string> pins, tags;                                          // all pins / the pins that are in no route
    std::vector<Route> routes;
    std::vector<olc::vi2d> dots; 
    std::vector<std::vector<olc::vi2d>> lines;
    olc::vi2d label; 
  };

  struct Note  { olc::vf2d pos; std::string text; olc::Pixel color; };

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
        // { "Fit",        [this](void*) { Fit();                    return nullptr; } },
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

    chips.clear(); notes.clear(); nets.clear(); nodes.clear();

    // std::map<std::string, olc::vf2d> newBase;
    std::vector<std::pair<size_t, bool>> unplaced;                                // (part index, has no position at all)
    std::map<std::string, std::set<std::string>> newPins;

    for (size_t i = 1, n = lua.Length(root + ".parts"); i <= n; i++) {
      const std::string p = root + ".parts." + std::to_string(i);
      const std::string id = lua.GetOr(p + ".id", ""), type = lua.GetOr(p + ".type", "");
      const auto x = lua.Get<int32_t>(p + ".x"), y = lua.Get<int32_t>(p + ".y");

      const Datasheet* ds = Library::Find(type);
      if (!ds) { Warn("unknown part type \"" + type + "\""); continue; }

      Chip chip; chip.id = id; chip.datasheet = ds;
      chip.tags.assign(ds->pins.size(), nullptr); 

      if (x && y) chip.pos = { *x, *y };
      chips.push_back(std::move(chip));

      for (size_t k = 0; k < ds->pins.size(); k++) {
        if (ds->pins[k].name == "_") continue;
        nodes.emplace(id + "." + ds->pins[k].name, Node{ chips.size() - 1, k });
      }
    }

    // for (auto& c : chips) {
    // }

    // const size_t ci = chips.size();

    // float parkX = 200.f, parkY = 0.f;                                             // chips with no position: a column to the right of the placed ones
    // for (const auto& p : newParts) if (const auto b = newBase.find(p.id); b != newBase.end()) parkX = std::max(parkX, b->second.x + p.size.x + 200.f);
    // for (const auto& [i, noPos] : unplaced) {
    //   auto& p = newParts[i];
    //   newBase[p.id] = { parkX, parkY };
    //   if (noPos) p.pos = newBase[p.id];                                           // (a chip that is only in SCHEMATIC_LAYOUT keeps that position)
    //   parkY += p.size.y + 40.f;
    // }

    // "ID.PIN" -> (part, pin), the same key as a net uses (marks stripped)
    // std::map<std::string, std::pair<size_t, size_t>> newIndex;
    // for (size_t i = 0; i < newParts.size(); i++) for (size_t k = 0; k < newParts[i].pins.size(); k++)
    //   newIndex.emplace(newParts[i].id + "." + StripMarks(newParts[i].pins[k].name), std::make_pair(i, k));

    std::set<std::string> reported;                                               // each bad pin is reported once
    std::map<std::string, std::string> owner;                                     // pin key -> the net it is in (a pin is in one net)
    auto names = lua.Keys(root + ".nets");
    std::sort(names.begin(), names.end());                                        // Lua's table order is arbitrary, the drawing should not be
    for (const auto& name : names) {
      const std::string p = root + ".nets." + name;
      Net net; net.name = name; net.id = name;
      if (!net.id.empty() && net.id.front() == '/') net.id.erase(0, 1);

      const auto pinOf = [&](const std::string& ref) -> std::string {            // "D5.<A" -> "D5.A", added to the net once; "" if there is no such pin
        const auto dot = ref.find('.');
        const std::string key = dot == std::string::npos ? "" : ref.substr(0, dot) + "." + StripMarks(ref.substr(dot + 1));
        if (!nodes.count(key)) { if (reported.insert(name + " " + key).second) Warn("net " + name + ": no pin \"" + ref + "\""); return ""; }
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
        Route r; r.from = pinOf(lua.GetOr(rp + ".from", "")); r.to = pinOf(lua.GetOr(rp + ".to", ""));
        if (r.from.empty() || r.to.empty() || r.from == r.to) continue;
        if (const auto dx = lua.Get<float>(rp + ".dx")) r.dx = *dx;
        routed.insert(r.from); routed.insert(r.to);
        net.routes.push_back(std::move(r));
      }
      for (const auto& key : net.pins) if (!routed.count(key)) net.tags.push_back(key);
      nets.push_back(std::move(net));
    }

    for (size_t i = 1, n = lua.Length(root + ".notes"); i <= n; i++) {
      const std::string p = root + ".notes." + std::to_string(i);
      auto text = lua.GetOr(p + ".text", "");

      Note note; note.color = Color::DARK_GREY; note.pos = { lua.GetOr(p + ".x", 0.f), lua.GetOr(p + ".y", 0.f) };

      if (!text.empty() && text.front() == '?') text.erase(0, 1), note.color = Color::BLUE;
      if (!text.empty() && text.front() == '!') text.erase(0, 1), note.color = Color::RED;
      if (!text.empty() && text.front() == '^') text.erase(0, 1), note.color = Color::DARK_YELLOW;
      if (!text.empty() && text.front() == '*') text.erase(0, 1), note.color = Color::GREEN;

      note.text = text; notes.push_back(std::move(note));
    }

    netPins = std::move(newPins);
    // base = std::move(newBase); 
    for (const auto& n : nets) for (const auto& key : n.tags) { const auto& r = nodes.at(key); chips[r.chip].tags[r.pin] = &n; }
    for (const auto& [wire, pins] : netPins) states.try_emplace(wire).first->second.store(kUnset);   // (a reload must not run while another thread calls Set)
    // hoverNet.clear(); hoverPart = -1; drag = -1; if (!netPins.count(selectedNet)) selectedNet.clear();

    return Resolve(), true;
  }

  // ---------------------------------------------------------------- layout (what you paste into Lua)
  std::vector<std::string> LayoutLines(bool all) const {
    std::vector<std::string> out; int count = 0;
    for (const auto& p : chips) {
      const auto b = base.find(p.id);
      if (!all && b != base.end() && b->second == p.pos) continue;
      char buf[160]; const std::string key = IsIdent(p.id) ? p.id : "[\"" + p.id + "\"]";
      std::snprintf(buf, sizeof buf, "  %s = { x = %d, y = %d },", key.c_str(), p.pos.x, p.pos.y);
      out.push_back(buf); count++;
    }
    out.insert(out.begin(), "SCHEMATIC_LAYOUT = {");
    out.insert(out.begin(), std::string("-- paste into config.lua (pixels): ") + std::to_string(count) + (all ? " part(s)" : " moved part(s)"));
    out.push_back("}");
    return out;
  }

  void PrintLayout(bool all) { for (const auto& l : LayoutLines(all)) std::puts(l.c_str()); std::fflush(stdout); }

  void ResetMoves() { for (auto& p : chips) if (auto b = base.find(p.id); b != base.end()) p.pos = b->second; Resolve(); }

  // ---------------------------------------------------------------- view
  // the view is only panned (pixels): drag, two-finger scroll (wheel), Shift + wheel for sideways, H J K L
  bool IsMouseOver() const {
    const olc::vi2d mouse = GameEngine->GetMousePos(), pos = Origin(), area = Area();
    return !(mouse.x < pos.x || mouse.y < pos.y || mouse.x >= pos.x + area.x || mouse.y >= pos.y + area.y);
  }

  inline void Focus(bool state) override { ::Window::KeyEventListener::Focus(state); menu->Focus(false); }

  // ---------------------------------------------------------------- frame
  void Preprocess() override {
    if (IsMouseOver()) {
      if (const int wheel = GameEngine->GetMouseWheelH()) offset.x -= wheel / 120.f * kWheelPx;   // swipe right = view moves right
      if (const int wheel = GameEngine->GetMouseWheel())  offset.y += wheel / 120.f * kWheelPx;
    }

    menu->Preprocess();
  }

  void Process() override {
    menu->Process();

    values.clear();                                                               // sample every signal once per frame: pushed levels, then bound ones, then strobes
    for (auto& [name, st] : states) if (const int v = st.load(); v != kUnset) values[name] = v;
    for (auto& [name, fn] : signals) values[name] = fn() ? 1 : 0;
    for (auto& [name, cnt] : strobes) { int v = cnt.load(); if (v > 0) { values[name] = 1; cnt.compare_exchange_strong(v, v - 1); } }

    const olc::vi2d mouse = GameEngine->GetMousePos();
    if (menu->bEnabled) { prevMouse = mouse; return; }                            // the menu owns the mouse

    const olc::vi2d world = ToWorld(mouse);

    if (drag >= 0) {                                                              // moving a part (keeps working outside the window)
      if (GameEngine->GetMouse(0).bHeld) {
        const olc::vi2d want = Snap(world - grab);
        if (want != chips[drag].pos) chips[drag].pos = want, Resolve();
      } else drag = -1;
    }

    if (IsMouseOver() && drag < 0) {

      // if (GameEngine->GetMouse(0).bHeld || GameEngine->GetMouse(2).bHeld) offset += mouse - prevMouse;

      // hoverNet.clear(); hoverPart = -1;
      // for (int i = (int)parts.size() - 1; i >= 0; i--) {                          // last drawn = on top
      //   const auto& p = parts[i];
      //   if (world.x >= p.pos.x && world.y >= p.pos.y && world.x < p.pos.x + p.size.x && world.y < p.pos.y + p.size.y) { hoverPart = i; break; }
      // }
      // if (hoverPart < 0) {
      //   float best = 5.f;                                                          // 5 pixels
      //   for (const auto& n : nets) for (const auto& l : n.lines) for (size_t i = 1; i < l.pts.size(); i++) {
      //     const float d = DistSeg(world, l.pts[i - 1], l.pts[i]);
      //     if (d < best) best = d, hoverNet = n.name;
      //   }
      // }

    //   if (GameEngine->GetMouse(0).bPressed) {
    //     if (hoverPart >= 0 && edit) drag = hoverPart, grab = world - parts[hoverPart].pos;
    //     else selectedNet = hoverNet;                                              // click empty space -> clear
    //   }
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

    for (const auto chip : chips) {
      DrawSymbol(chip, chip.pos + offset, Color::GREY);
    }

    for (const auto& note : notes) GameEngine->DrawString(note.pos + offset, note.text, note.color);

    // nets: normal first, highlighted on top
    for (int pass = 0; pass < 2; pass++) for (const auto& n : nets) {
      const bool lit = n.name == hoverNet || n.name == selectedNet;
      if (lit != (pass == 1)) continue;
      const olc::Pixel col = Color(&n);
      // const olc::Pixel col = Color::GREY;

      for (const auto& l : n.lines) for (size_t i = 1; i < l.size(); i++) GameEngine->DrawLine(l[i - 1] + offset, l[i] + offset, col);
      for (const auto& d : n.dots) GameEngine->FillCircle(d + offset, 2, col);

      if (n.label.x || n.label.y) {
        const int w = (int)n.name.size() * vStep.x;
        GameEngine->DrawString(olc::vi2d(n.label.x - w / 2, n.label.y - 10) + offset, n.name, col);
      }
    }

    // for (const auto& n : nets) for (const auto& l : n.labels) GameEngine->DrawString(I(l.pos + offset), l.text, olc::Pixel(230, 220, 100));

    // the 0 / 1 is written only for the hovered or selected wire (that wire is drawn yellow / white, so the digit carries the level):
    // at both ends of a routed wire, at the pin of a tag
    // for (const auto& n : nets) {
    //   if (n.name != hoverNet && n.name != selectedNet) continue;
    //   const auto v = values.find(n.name);
    //   if (v == values.end()) continue;
    //   for (const auto& l : n.lines) {
    //     const auto& pts = l.pts; const size_t last = pts.size() - 1;
    //     DrawLevel(pts[0], pts[1].x - pts[0].x, v->second);
    //     if (!l.tag) DrawLevel(pts[last], pts[last - 1].x - pts[last].x, v->second);
    //   }
    // }

    // // status bar
    // const int rows = (int)((size.x - vOffset.x) / 8);
    // std::string line; olc::Pixel lineCol(200, 200, 200);
    // if (!hoverNet.empty() || !selectedNet.empty()) {
    //   const std::string& n = hoverNet.empty() ? selectedNet : hoverNet;
    //   const auto v = values.find(n);
    //   line = n + (v == values.end() ? "" : v->second ? " = 1" : " = 0") + "  |";
    //   if (auto it = netPins.find(n); it != netPins.end()) for (const auto& pin : it->second) line += " " + pin;
    // } else if (hoverPart >= 0 || drag >= 0) {
    //   const auto& p = chips[drag >= 0 ? drag : hoverPart]; char pos[48]; std::snprintf(pos, sizeof pos, "  (%g, %g)", p.pos.x, p.pos.y);
    //   // line = p.title + pos;
    // }
    // if ((int)line.size() > rows) line.resize(rows);
    // GameEngine->DrawString(olc::vi2d(lo.x + 2, hi.y - 9), line, lineCol);
    // if (edit) GameEngine->DrawString(olc::vi2d(hi.x - 8 * 4 - 4, lo.y + 2), "EDIT", olc::Pixel(220, 160, 60));

    menu->Draw();
  }

  void DrawSymbol(const Chip& chip, olc::vi2d absolute, olc::Pixel color = Color::GREY) {
    const auto size = chip.datasheet->GetSize();
    auto pos = absolute - olc::vi2d(0, vStep.y);
 
    GameEngine->DrawRect(absolute, size, color);
    GameEngine->DrawString(pos, chip.datasheet->name, Color::WHITE);

    const int32_t w = (int32_t)chip.datasheet->label.size() * vStep.x;
    GameEngine->DrawString(pos + olc::vi2d((size.x - w) / 2, vStep.y + 6), chip.datasheet->label, Color::DARK_GREY);

    pos.x += (chip.datasheet->name.size() + 1) * vStep.x;
    GameEngine->DrawString(pos, "[" + chip.id + "]", Color::VERY_DARK_YELLOW);


    for (int32_t i = 0; i < chip.datasheet->pins.size(); ++i) {
      const auto& pin = chip.datasheet->pins[i];
      if (pin.name == "_") continue;

      const auto a = chip.datasheet->AnchorOf(pin, absolute);
      const auto p = std::to_string(pin.pin);

      const int32_t y = a.inner.y - 3;
      const int32_t w = (int32_t)pin.name.size() * vStep.x;
      const int32_t tx = pin.side ? absolute.x + size.x - w - 6 : absolute.x + 8;
      const int32_t px = pin.side ? absolute.x + size.x + 4 : absolute.x - p.size() * vStep.x - 2;

      GameEngine->DrawString(olc::vi2d(tx, y), pin.name, olc::DARK_GREY);
      if (pin.pin) GameEngine->DrawString(olc::vi2d(px, y - 7), p, olc::VERY_DARK_GREY);
      if (pin.inverted) GameEngine->DrawLine(olc::vi2d(tx, y - 2), olc::vi2d(tx + w - 2, y - 2), olc::DARK_GREY);

      const olc::vi2d start = pin.side ? a.inner : a.outer;
      const olc::vi2d end   = pin.side ? a.outer : a.inner;
 
      if (!pin.state) GameEngine->DrawLine(start, end, color);
      if (pin.inverted) GameEngine->DrawCircle(a.inner + olc::vi2d(pin.side ? -1 : 1, 0), 3, color);
      if (pin.state & Datasheet::State::INPUT)  GameEngine->DrawArrow(end, start, color);
      if (pin.state & Datasheet::State::OUTPUT) GameEngine->DrawArrow(start, end, color);


      if (const Net* n = chip.tags[i]) {
        const Tag t = TagAt(chip, i, absolute);
        const auto pulled = n->id == VCC || n->id == GND;
        const auto inverted = n->id.size() != n->name.size();

        GameEngine->DrawString(t.pos, n->id, Color(n));
        if (!pulled && pin.inverted ^ inverted) GameEngine->DrawLine(olc::vi2d(t.pos.x, t.pos.y - 2), olc::vi2d(t.pos.x + t.w - 2, t.pos.y - 2), Color(n));
      }
    }
  }

public:
  ::Window::StrokeAction Process(Int2Type<olc::Key::ESCAPE>) { return menu->Reset(), menu->Focus(false), nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::OEM_2>) { return menu->bEnabled = true, menu->Focus(true), nullptr; }

  ::Window::StrokeAction Process(Int2Type<olc::Key::H>) { return menu->bEnabled ? nullptr : (offset.x += kPanStep, nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) { return menu->bEnabled ? nullptr : (offset.x -= kPanStep, nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) { return menu->bEnabled ? nullptr : (offset.y += kPanStep, nullptr); }
  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) { return menu->bEnabled ? nullptr : (offset.y -= kPanStep, nullptr); }

  const ::Window::StrokeT<>& GetCommands() const override { return Schematic::commands; }

private:
  // ---------------------------------------------------------------- helpers
  olc::vi2d Area() const { return olc::vi2d(size - vOffset); }                    // drawable content area
  olc::vi2d Origin() const { return olc::vi2d(absolute + vOffset); }
  olc::vi2d ToScreen(olc::vi2d w) const { return Origin() + w + offset; }
  olc::vi2d ToWorld(olc::vi2d s) const { return s - Origin() - offset; }
  olc::vi2d Snap(olc::vi2d w) const { return snap > 0.f ? olc::vi2d(std::round(w.x / snap) * snap, std::round(w.y / snap) * snap) : w; }
  static std::string StripMarks(std::string s) {                                  // "<A", "A>", ">A", "/A", "<A>" -> "A"
    const auto mark = [](char c) { return c == '<' || c == '>' || c == '/'; };
    while (!s.empty() && mark(s.front())) s.erase(0, 1);
    while (!s.empty() && mark(s.back())) s.pop_back();
    return s;
  }
  static bool IsIdent(const std::string& s) { if (s.empty() || std::isdigit((unsigned char)s[0])) return false; for (char c : s) if (!std::isalnum((unsigned char)c) && c != '_') return false; return true; }
  static void Warn(const std::string& s) { std::fprintf(stderr, "schematic: %s\n", s.c_str()); }
  // static olc::vi2d I(olc::vi2d v) { return olc::vi2d((int)std::lround(v.x), (int)std::lround(v.y)); }

  // do two straight segments (horizontal or vertical) run along each other for more than one point?
  static bool Overlap(olc::vi2d a, olc::vi2d b, olc::vi2d c, olc::vi2d d) {
    if (a.y == b.y && c.y == d.y && a.y == c.y) return std::min(std::max(a.x, b.x), std::max(c.x, d.x)) > std::max(std::min(a.x, b.x), std::min(c.x, d.x));
    if (a.x == b.x && c.x == d.x && a.x == c.x) return std::min(std::max(a.y, b.y), std::max(c.y, d.y)) > std::max(std::min(a.y, b.y), std::min(c.y, d.y));
    return false;
  }

  // p lies on the segment a-b, but not at an end (the segments are horizontal or vertical)
  static bool Inside(olc::vi2d p, olc::vi2d a, olc::vi2d b) {
    return (a.y == b.y && p.y == a.y && std::min(a.x, b.x) < p.x && p.x < std::max(a.x, b.x))
        || (a.x == b.x && p.x == a.x && std::min(a.y, b.y) < p.y && p.y < std::max(a.y, b.y));
  }

  static bool Facing(const Port& a, const Port& b) { return a.right != b.right && (a.right ? a.pos.x <= b.pos.x : b.pos.x <= a.pos.x); }
  static int Dir(olc::vi2d from, olc::vi2d to) { return to.x > from.x ? 2 : to.x < from.x ? 1 : to.y < from.y ? 4 : to.y > from.y ? 8 : 0; }
  static int Arms(olc::vi2d p, olc::vi2d a, olc::vi2d b) {                          // the ways the segment a-b leaves p (0 = p is not on it)
    if (p == a) return Dir(a, b);
    if (p == b) return Dir(b, a);
    return Inside(p, a, b) ? (Dir(a, b) | Dir(b, a)) : 0;
  }

  static bool Hits(olc::vi2d p, olc::vi2d q, const Box& r) {                        // does the straight segment p-q touch the box?
    return std::max(std::min(p.x, q.x), r.a.x) <= std::min(std::max(p.x, q.x), r.b.x)
        && std::max(std::min(p.y, q.y), r.a.y) <= std::min(std::max(p.y, q.y), r.b.y);
  }

  Tag TagAt(const Chip& c, size_t i, olc::vi2d origin) const {                      // `origin` = where the chip is drawn
    const auto& pin = c.datasheet->pins[i];
    const auto an = c.datasheet->AnchorOf(pin, origin);
    const olc::vi2d outer(an.outer), inner(an.inner);
    const int32_t w = (int32_t)c.tags[i]->name.size() * vStep.x;
    return { olc::vi2d(pin.side ? outer.x + 4 : outer.x - 4 - w, inner.y - 3), w, outer };
  }

  Port PinAt(const std::string& key) const {
    const Node& r = nodes.at(key);
    const Chip& c = chips[r.chip];
    const auto& pin = c.datasheet->pins[r.pin];
    return { olc::vi2d(c.datasheet->AnchorOf(pin, c.pos).outer), pin.side == Datasheet::RIGHT, c.pos.y, c.pos.y + (int32_t)c.datasheet->GetSize().y };
  }

  // the path of one route; `shift` > 0 = try again, further out
  std::vector<olc::vi2d> Path(const Port& a, const Port& b, float dx, int shift) const {
    std::vector<olc::vi2d> pts{ a.pos };
    const int32_t push = shift * 6, ext = (int32_t)kLeg + push;
    const bool facing = a.right != b.right && (a.right ? a.pos.x <= b.pos.x : b.pos.x <= a.pos.x);

    if (a.right != b.right && !facing && dx == 0.f) {                               // not facing: round the chip(s)
      const int32_t top = std::min(a.top, b.top) - 20 - push, bottom = std::max(a.bottom, b.bottom) + 20 + push;
      const int32_t lane = (a.pos.y + b.pos.y) / 2 < (top + bottom) / 2 ? top : bottom;
      const int32_t xa = a.pos.x + (a.right ? ext : -ext), xb = b.pos.x + (b.right ? ext : -ext);
      pts.push_back({ xa, a.pos.y }); pts.push_back({ xa, lane }); pts.push_back({ xb, lane }); pts.push_back({ xb, b.pos.y });
    } else if (a.pos.y != b.pos.y) {
      int32_t leg;
      if (a.right != b.right) {                                                     // facing: half way, nudged either side of the middle on a retry
        const int32_t lo = std::min(a.pos.x, b.pos.x), hi = std::max(a.pos.x, b.pos.x);
        leg = std::clamp((a.pos.x + b.pos.x) / 2 + (shift % 2 ? 1 : -1) * ((shift + 1) / 2) * 6, lo, hi);
      } else {                                                                      // same side: "[" or, for close pins in line, just "|"
        const int32_t out = (a.pos.x == b.pos.x && std::abs(a.pos.y - b.pos.y) <= 28 ? 0 : (int32_t)kLeg) + push;
        leg = a.right ? std::max(a.pos.x, b.pos.x) + out : std::min(a.pos.x, b.pos.x) - out;
      }
      if (dx != 0.f) leg = a.pos.x + (int32_t)dx;
      pts.push_back({ leg, a.pos.y }); pts.push_back({ leg, b.pos.y });
    }
    pts.push_back(b.pos);
    pts.erase(std::unique(pts.begin(), pts.end()), pts.end());                      // a "|" would otherwise carry zero-length pieces
    return pts;
  }

  void Resolve() {
    struct Seg { olc::vi2d a, b; size_t net; };
    std::vector<Seg> placed;                                                        // every wire piece (and stub tip) placed so far, all nets

    boxes.clear();                                                                  // the tag names: no wire may cross them
    for (const auto& c : chips) for (size_t i = 0; i < c.tags.size(); i++) if (c.tags[i]) {
      const Tag t = TagAt(c, i, c.pos);
      boxes.push_back({ t.pos - olc::vi2d(2, 4), t.pos + olc::vi2d(t.w + 2, 10), t.tip });
    }

    for (size_t ni = 0; ni < nets.size(); ni++) {
      Net& n = nets[ni];
      n.lines.clear(); n.dots.clear(); n.label = { 0, 0 };

      const auto clash = [&](const std::vector<olc::vi2d>& p, const Port& a, const Port& b) {   // on another net's wire, or across a tag name?
        for (size_t i = 1; i < p.size(); i++) {
          for (const auto& s : placed) if (s.net != ni && Overlap(p[i - 1], p[i], s.a, s.b)) return true;
          for (const auto& t : boxes) if (t.tip != a.pos && t.tip != b.pos && Hits(p[i - 1], p[i], t)) return true;   // the names at its own two pins are skipped
        }
        return false;
      };

      // first pass: one path per route, pushed further out while it clashes; if nothing is clean the first path is kept
      std::vector<std::pair<Port, Port>> ends;                                      // the two ports of each route, parallel to n.lines
      for (const auto& r : n.routes) {
        const Port a = PinAt(r.from), b = PinAt(r.to);
        auto pts = Path(a, b, r.dx, 0);
        if (r.dx == 0.f && clash(pts, a, b))
          for (int shift = 1; shift < 12; shift++) if (auto t = Path(a, b, r.dx, shift); !clash(t, a, b)) { pts = std::move(t); break; }
        ends.push_back({ a, b });
        n.lines.push_back(std::move(pts));
      }

      // second pass: how far out does another route of this net leave this tip? (the tip itself if none)
      const auto reach = [&](size_t self, const olc::vi2d& tip, bool right) {
        int32_t x = tip.x;
        for (size_t j = 0; j < n.lines.size(); j++) {
          const auto& l = n.lines[j]; const auto& [pa, pb] = ends[j];
          if (j == self || l.size() < 3 || (pa.right != pb.right && Facing(pa, pb))) continue;   // a straight or a Z wire does not stick out
          if (l.front() == tip) x = right ? std::max(x, l[1].x) : std::min(x, l[1].x);
          if (l.back() == tip)  x = right ? std::max(x, l[l.size() - 2].x) : std::min(x, l[l.size() - 2].x);
        }
        return x;
      };
      for (size_t i = 0; i < n.lines.size(); i++) {                                 // a "|" or "[" follows the wire that leaves its pins further out
        const auto& [a, b] = ends[i];
        if (a.right != b.right || a.pos.y == b.pos.y || n.routes[i].dx != 0.f) continue;
        const int32_t own = n.lines[i].size() >= 3 ? n.lines[i][1].x : a.pos.x;
        const int32_t want = a.right ? std::max({ own, reach(i, a.pos, true),  reach(i, b.pos, true)  })
                                     : std::min({ own, reach(i, a.pos, false), reach(i, b.pos, false) });
        if (want == own) continue;
        auto moved = Path(a, b, (float)(want - a.pos.x), 0);
        if (!clash(moved, a, b)) n.lines[i] = std::move(moved);
      }

      const size_t first = placed.size();                                           // the net is final: now the next nets can see it
      for (size_t i = 0; i < n.lines.size(); i++) {
        const auto& l = n.lines[i];
        for (size_t k = 1; k < l.size(); k++) placed.push_back({ l[k - 1], l[k], ni });
        for (const Port* p : { &ends[i].first, &ends[i].second }) placed.push_back({ p->pos, p->pos + olc::vi2d(p->right ? -8 : 8, 0), ni });   // a stub is an arm of a junction too
      }

      int best = 0;                                                                 // the name goes above the longest horizontal piece of the net
      for (const auto& l : n.lines) for (size_t i = 1; i < l.size(); i++)
        if (const int len = std::abs(l[i].x - l[i - 1].x); l[i].y == l[i - 1].y && len > best)
          best = len, n.label = { (l[i].x + l[i - 1].x) / 2, l[i].y };
      if (n.name.front() == '_' || best < (int)n.name.size() * vStep.x + 16) n.label = { 0, 0 };   // auto name, or too short for the text

      if (n.lines.size() < 2) continue;                                             // one route cannot branch
      for (const auto& l : n.lines) for (const auto& p : l) {                       // a vertex where 3 or more arms meet is a junction
        int mask = 0;
        for (size_t k = first; k < placed.size(); k++) mask |= Arms(p, placed[k].a, placed[k].b);
        if (std::bitset<4>(mask).count() >= 3 && std::find(n.dots.begin(), n.dots.end(), p) == n.dots.end()) n.dots.push_back(p);
      }
    }
  }

  olc::Pixel Color(const Net* net) const {
    if (net->name == VCC) return Color::RED;
    if (net->name == GND) return Color::BLUE;
    // if (net->name == hoverNet) return olc::YELLOW;
    // if (tintWires) if (auto it = values.find(net->name); it != values.end()) return LevelColor(it->second);   // 1 = green, 0 = red
    // return olc::Pixel(60, 150, 140);                                              // no level (never Set / Bound): plain teal
    if (net->name.empty() || net->name.front() == '_') return Color::GREY;

    return Color::YELLOW;
  }

private:
  inline static const char* VCC = "Vcc";
  inline static const char* GND = "GND";

  const olc::vi2d vStep = olc::vi2d(8, 14);

  static constexpr float kPanStep = 48.f, kWheelPx = 30.f, kTagLen = 12.f, kLeg = 16.f;   // tag stub length / how far a leg sticks out beyond two pins that face the same way
  static constexpr int kUnset = -1;
  static constexpr int kStatusH = 12;                               // bottom 12 px belong to the status bar
  const olc::vi2d vOffset = olc::vi2d(2, 14);

  LuaScript lua;

  std::vector<Net>  nets;
  std::vector<Chip> chips;
  std::vector<Box>  boxes;
  std::vector<Note> notes;

  std::map<std::string, std::set<std::string>> netPins;
  std::map<std::string, olc::vi2d> base;                                          // positions as written in SCHEMATIC.layout
  std::unordered_map<std::string, Node> nodes;

  std::unordered_map<std::string, std::function<bool()>> signals;
  std::unordered_map<std::string, std::atomic<int>> strobes;
  std::unordered_map<std::string, std::atomic<int>> states;                       // one per wire id, created in Load() and never modified after, so Set() is thread safe
  std::unordered_map<std::string, int> values;                                    // sampled once per frame in Process()

  // std::unique_ptr<olc::Sprite> layer;                                             // the scene, drawn off-screen so that it is clipped to the window
  olc::vi2d offset = { 40, 40 }, prevMouse = { 0, 0 }, grab = { 0, 0 };   // where the scene is scrolled to, in pixels
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
  { Int2Type<olc::Key::ESCAPE>(), [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  { Int2Type<olc::Key::SHIFT>(), {
    { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return Self<Schematic>(self)->Process(keys...); } },
  }}
});

};