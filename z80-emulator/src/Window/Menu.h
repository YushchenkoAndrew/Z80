#pragma once

#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Types/MenuAction.h"
#include "src/Window/Window.h"
#include <cstdint>

namespace Window {

class Menu : public ::Window::KeyEventListener, public Window::Window {
public:
  Menu(PixelGameEngine* engine, std::string id, MenuT menu): ::Window::Base(engine), ::Window::Window(engine, id), ::Window::KeyEventListener(engine), menu(std::move(menu)) {}

  void Preprocess() override {
    const auto mouse = GameEngine->GetMousePos();

    const auto nStartX = absolute.x + vOffset.x;
    const auto nStartY = absolute.y + vOffset.y;

    const auto nZeroX = nStartX - 3 * vStep.x;

    const auto nEndX = nStartX + size.x;
    const auto nEndY = nStartY + size.y;

    const auto offset = (mouse - absolute - vOffset) / vStep + olc::vi2d(0, line) - olc::vi2d(1 , 0);

    // FIXME:
    // if (mouse.x > nStartX && mouse.y > nStartY && mouse.x < nEndX   && mouse.y < nEndY) line = line + offset.y;
  }

  void Process() override {}

  void Draw() override {
    // TODO: Think about the line
    olc::vi2d pos = olc::vi2d(absolute.x + vOffset.x, absolute.y + line * vStep.y + vOffset.y - 2);
    GameEngine->FillRect(pos, { size.x, 11 }, Color::VERY_DARK_GREY);

    DrawBorder();

    MenuT* menu = &this->menu; DrawMenu(menu); auto index = 1;
    for (const auto& i : path) {
      auto offset = olc::vi2d(index, index) * vStep;

      DrawMenu(menu = &menu->children[i], offset); index++;
    }
  }

  void DrawMenu(MenuT* menu, olc::vi2d offset = {}) {
    const auto nHeight = size.y - vStep.y;
    for (int32_t i = 0; i < menu->children.size(); i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * vStep + vOffset + offset;
      const auto& item = menu->children[i];

      if (pos.x > absolute.x + vOffset.x + size.x + vStep.x) continue;
      if (pos.y > absolute.y + vOffset.y + nHeight) break;

      auto nWidth = absolute.x + vOffset.x + vStep.x;
      if (pos.x < nWidth && pos.x + item.name.size() * vStep.x <= nWidth) continue;

      auto startAt = pos.x < nWidth ? (nWidth - pos.x) / vStep.x : 0;
      auto adj = pos + olc::vi2d(startAt * vStep.x, 0);
      nWidth = absolute.x + vOffset.x + size.x + vStep.x;

      if (pos.x < nWidth && pos.x + item.name.size() * vStep.x > nWidth) {
        if (startAt > item.name.size()) continue;
        GameEngine->DrawString(pos, item.name, Color::GREY); // Fix this to cut string that are to big
      } GameEngine->DrawString(adj, item.name, Color::GREY);
    }
  }

  void DrawBorder() {
    constexpr int32_t STEP = 5;   // length of each corner arm
    constexpr int32_t GAP  = 2;   // thickness of the cut

    GameEngine->DrawRect(absolute, size - olc::vi2d(1, 1), Color::DARK_GREY);

    GameEngine->FillRect({ absolute.x + STEP,                absolute.y },                       { GAP, size.y }, Color::BLACK);  // left
    GameEngine->FillRect({ absolute.x + size.x - STEP - GAP, absolute.y },                       { GAP, size.y }, Color::BLACK);  // right
    GameEngine->FillRect({ absolute.x,                       absolute.y + STEP },                { size.x, GAP }, Color::BLACK);  // top
    GameEngine->FillRect({ absolute.x,                       absolute.y + size.y - STEP - GAP }, { size.x, GAP }, Color::BLACK); // bottom
  }

public:
  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) {
    auto item = GetItem();
    if (item == nullptr || line + 1 >= item->children.size()) return nullptr;
    return line++, nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) {
    if (line <= 0) return nullptr;
    return line--, nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) {
    auto item = GetItem();
    if (item == nullptr) return nullptr;

    if (item->children[line].func == nullptr) {
      return path.push_back(line), line = 0, nullptr;
    }

    // TODO: Think about how to impl this
    auto temp = item->children[line].func(this);
    
    return path.clear(), nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::H>) {
    auto item = GetItem();
    if (path.empty() || item == nullptr) return nullptr;

    return line = path.back(), path.pop_back(), nullptr;
  }

  const ::Window::StrokeT<>& GetCommands() const override { return Menu::commands; }

  MenuT* GetItem() {
    MenuT* menu = &this->menu;
    for (auto& i : path) {
      if (i < menu->children.size()) menu = &menu->children[i];
      else return nullptr;
    }

    return menu;
  }

private:
  uint8_t line = 0;

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(12, 8);

  MenuT menu;

  std::list<uint8_t> path;
  static const ::Window::StrokeT<> commands;
};

inline const ::Window::StrokeT<> Menu::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return static_cast<Menu*>(self)->Process(keys...); } },
});
};

