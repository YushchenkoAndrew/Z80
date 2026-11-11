#pragma once

#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Types/MenuAction.h"
#include "src/Window/Window.h"

namespace Window {

class Menu : public ::Window::KeyEventListener, public Window::Window {
public:
  Menu(PixelGameEngine* engine, std::string id, MenuT menu): ::Window::Base(engine), ::Window::Window(engine, id), ::Window::KeyEventListener(engine), menu(std::move(menu)) {}

  // inline void Preinitialize() { Lock(); }
  // inline void Select(DimensionT dimensions) {
  //   this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  // }

  // void Load(std::vector<std::string>& dst) {
  //   vim.Load(lines = dst); vim.Process(Int2Type<olc::Key::G>(), Int2Type<olc::Key::G>());
  // }

  // void Preprocess() override { window->Preprocess(); }
  void Process() override {
    // TODO: Impl select
  }

  void Draw() override {
    // Fix this to add a recursion
    auto items = &menu.children;
    const auto nHeight = size.y - vStep.y;


    for (int32_t i = 0; i < items->size(); i++) {
      olc::vi2d pos = absolute + olc::vi2d(1, i) * vStep + vOffset;
      const auto& item = items->at(i);

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

private:
  const olc::vi2d vStep = olc::vi2d(4, 4);
  const olc::vi2d vOffset = olc::vi2d(12, 8);

  MenuT menu;
  std::list<std::string> path;
};
};

