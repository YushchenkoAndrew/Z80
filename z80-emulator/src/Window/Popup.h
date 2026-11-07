#pragma once

#include "src/Defs.h"
#include "src/Window/Event/KeyboardEvent.h"
#include "src/Window/Window.h"
#include <memory>

namespace Window {

class Popup : public ::Window::KeyboardEvent, public Window::Window {
public:
  Popup(PixelGameEngine* engine, std::string id, std::shared_ptr<::Window::Window> window): ::Window::Base(engine), ::Window::Window(engine, id), ::Window::KeyboardEvent(engine), window(window) {}

  // inline void Preinitialize() { Lock(); }
  // inline void Select(DimensionT dimensions) {
  //   this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  // }

  // void Load(std::vector<std::string>& dst) {
  //   vim.Load(lines = dst); vim.Process(Int2Type<olc::Key::G>(), Int2Type<olc::Key::G>());
  // }

  void Select(DimensionT dimensions) override { ::Window::Window::Select(dimensions); window->Select({ dimensions.first, olc::vi2d()}); }
  void Focus(bool state) override {
    ::Window::KeyboardEvent::Focus(state); window->Focus(state);

    if (state) window->Select({ absolute, size });
    else window->Select({ absolute, olc::vi2d() });
  }

  void Preprocess() override { window->Preprocess(); }
  void Process()    override { window->Process();    }

  void Draw() override {
    if (!window->size.x || !window->size.y) return;

    GameEngine->FillRect(absolute, size, Color::BLACK);
    // TODO: impl popup style ....


    window->Draw();
  }

public:
  std::shared_ptr<::Window::Window> window;

private:
  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  std::vector<std::string> lines;
};
};

