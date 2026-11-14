#pragma once

#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Types/MenuAction.h"
#include "src/Window/Window.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace Window {

class Menu : public ::Window::KeyEventListener, public Window::Window {
private:
  struct LevelView {
    const MenuT* items = nullptr;
    olc::vi2d position;
    olc::vi2d size;
    int32_t rows = 0;
    int32_t selected = 0;
  };

public:
  Menu(PixelGameEngine* engine, std::string id, MenuT menu): ::Window::Base(engine), ::Window::Window(engine, id), ::Window::KeyEventListener(engine), menu(std::move(menu)) {}

  void Reset() { path.clear(); line = 0; }

  bool IsMouseOver() const {
    auto mouse = GameEngine->GetMousePos();

    const auto item = View(path.size());
    if (!item.items || !item.rows) return false;
    if (mouse.x < item.position.x || mouse.y < item.position.y || mouse.x >= item.position.x + item.size.x || mouse.y >= item.position.y + item.size.y) return false;

    return true;
  }

  void Preprocess() override {
    if (!IsMouseOver()) return;

    const auto mouse = GameEngine->GetMousePos();
    const auto item = View(path.size());
    line = (mouse.y - item.position.y) / vStep.y;
  }

  void Process() override {
    if (GameEngine->GetMouse(0).bPressed && IsMouseOver()) Process(Int2Type<olc::Key::L>());
  }

  void Draw() override {
    for (int32_t depth = 0; depth <= (int32_t)path.size(); depth++) {
      const auto v = View(depth);
      if (!v.items || !v.rows) break;
      DrawLevel(v, depth == (int32_t)path.size());
    }
  }

  void DrawLevel(const LevelView& v, bool active) {
    const auto at = v.position - vPad;
    const auto sz = v.size + vPad * 2;

    DrawBorder(at, sz);
    GameEngine->FillRect({ at.x + 1, v.position.y + v.selected * vStep.y - 3 }, { sz.x - 2, 11 }, Color::VERY_DARK_GREY);

    for (int32_t i = 0; i < v.rows; i++) {
      const auto& item = v.items->children[i];
      const auto pos = olc::vi2d(v.position.x, v.position.y + i * vStep.y - 1);
      const auto color = (active && i == v.selected) ? Color::WHITE : Color::GREY;

      GameEngine->DrawString(pos, item.name, color);
      if (!item.children.empty()) GameEngine->DrawString(pos + olc::vi2d(v.size.x - vStep.x, 0), ">", color);
    }
  }


  void DrawBorder(olc::vi2d at, olc::vi2d sz) {
    constexpr int32_t STEP = 5;
    constexpr int32_t GAP  = 2;
    sz = sz - olc::vi2d(0, vOffset.y - 1);

    GameEngine->FillRect(at, sz, Color::BLACK);
    GameEngine->DrawRect(at, sz - olc::vi2d(1, 1), Color::DARK_GREY);

    GameEngine->FillRect({ at.x + STEP,                at.y },                       { GAP, sz.y }, Color::BLACK);  // left
    GameEngine->FillRect({ at.x + sz.x - STEP - GAP,   at.y },                       { GAP, sz.y }, Color::BLACK);  // right
    GameEngine->FillRect({ at.x,                       at.y + STEP },                { sz.x, GAP }, Color::BLACK);  // top
    GameEngine->FillRect({ at.x,                       at.y + sz.y - STEP - GAP },   { sz.x, GAP }, Color::BLACK);  // bottom
  }

public:
  ::Window::StrokeAction Process(Int2Type<olc::Key::ESCAPE>) { return Reset(), nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) {
    const auto* items = Level(path.size());
    if (items && line + 1 < (int32_t)items->children.size()) line++;
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) {
    if (line > 0) line--;
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) {
    const auto* items = Level(path.size());
    if (!items || line < 0 || line >= (int32_t)items->children.size()) return nullptr;

    const auto& item = items->children[line];
    if (!item.children.empty()) return path.push_back(line), line = 0, nullptr;
    if (!item.func) return nullptr;

    const auto action = item.func;
    Reset(); action(this);
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::H>) {
    if (path.empty()) return nullptr;
    return line = path.back(), path.pop_back(), nullptr;
  }

  const MenuT* Level(size_t depth) const {
    const MenuT* m = &menu;
    for (size_t i = 0; i < depth; i++) {
      if (i >= path.size() || path[i] >= m->children.size()) return nullptr;
      m = &m->children[path[i]];
    }

    return m;
  }

  LevelView View(int32_t depth) const {
    LevelView v;
    v.items = Level(depth);
    if (!v.items) return v;

    size_t len = 0;
    for (const auto& c : v.items->children) len = std::max(len, c.name.size());

    v.rows = v.items->children.size();
    v.position = absolute + vOffset + vPopup * depth;
    v.size = olc::vi2d((len + 2) * vStep.x, v.rows * vStep.y);

    v.selected = depth < path.size() ? path[depth] : line;
    return v;
  }

  const ::Window::StrokeT<>& GetCommands() const override { return Menu::commands; }

private:
  uint8_t line = 0;

  const olc::vi2d vPad = olc::vi2d(6, 4);
  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vPopup = olc::vi2d(12, 8);
  const olc::vi2d vOffset = olc::vi2d(12, 8);

  MenuT menu;

  std::vector<uint8_t> path;
  static const ::Window::StrokeT<> commands;
};

inline const ::Window::StrokeT<> Menu::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::ESCAPE>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
});
};

