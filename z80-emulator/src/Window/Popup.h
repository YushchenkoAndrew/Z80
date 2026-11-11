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

    constexpr auto STEP = 5;
    const auto width = olc::vi2d(size.x, 0);
    const auto height = olc::vi2d(0, size.y);

    GameEngine->DrawRect(absolute, absolute + size, Color::DARK_GREY);
    GameEngine->DrawRect(absolute + olc::vi2d(STEP, 0),         height + olc::vi2d(2, absolute.y), Color::BLACK);
    GameEngine->DrawRect(absolute + olc::vi2d(STEP, 0) + width, height + olc::vi2d(2, absolute.y), Color::BLACK);
    GameEngine->DrawRect(absolute + olc::vi2d(0, STEP),         width +  olc::vi2d(absolute.x, 2), Color::BLACK);
    GameEngine->DrawRect(absolute + olc::vi2d(0,    0) + height,width +  olc::vi2d(absolute.x, 2), Color::BLACK);

    window->Draw();
  }

public:
  std::shared_ptr<::Window::Window> window;

private:
  const olc::vi2d vStep = olc::vi2d(4, 4);
  const olc::vi2d vOffset = olc::vi2d(12, 8);
};
};

