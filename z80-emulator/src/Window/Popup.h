#pragma once

#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Window.h"
#include <memory>

namespace Window {

class Popup : public ::Window::KeyEventListener, public Window::Window {
public:
  Popup(PixelGameEngine* engine, std::string id, std::shared_ptr<::Window::Window> window): ::Window::Base(engine), ::Window::Window(engine, id), ::Window::KeyEventListener(engine), window(window) {}

  // inline void Preinitialize() { Lock(); }
  // inline void Select(DimensionT dimensions) {
  //   this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  // }

  // void Load(std::vector<std::string>& dst) {
  //   vim.Load(lines = dst); vim.Process(Int2Type<olc::Key::G>(), Int2Type<olc::Key::G>());
  // }

  void Select(DimensionT dimensions) override {
    const auto absolute = dimensions.first + vOffset;
    const auto size = dimensions.second - vOffset;

    ::Window::Window::Select({ absolute, size });
    window->Select({ absolute, olc::vi2d()});
  }

  void Focus(bool state) override {
    ::Window::KeyEventListener::Focus(state); window->Focus(state);

    if (state) window->Select({ absolute, size });
    else window->Select({ absolute, olc::vi2d() });
  }

  void Preprocess() override { window->Preprocess(); }
  void Process()    override { window->Process();    }

  void Draw() override {
    if (!window->size.x || !window->size.y) return;

    constexpr int32_t STEP = 5;
    constexpr int32_t GAP  = 2;

    GameEngine->DrawRect(absolute, size - olc::vi2d(1, 1), Color::DARK_GREY);

    GameEngine->FillRect({ absolute.x + STEP,                absolute.y },                       { GAP, size.y }, Color::BLACK);  // left
    GameEngine->FillRect({ absolute.x + size.x - STEP - GAP, absolute.y },                       { GAP, size.y }, Color::BLACK);  // right
    GameEngine->FillRect({ absolute.x,                       absolute.y + STEP },                { size.x, GAP }, Color::BLACK);  // top
    GameEngine->FillRect({ absolute.x,                       absolute.y + size.y - STEP - GAP }, { size.x, GAP }, Color::BLACK); // bottom

    window->Draw();
  }

public:
  std::shared_ptr<::Window::Window> window;

private:
  const olc::vi2d vStep = olc::vi2d(4, 4);
  const olc::vi2d vOffset = olc::vi2d(12, 8);
};
};

