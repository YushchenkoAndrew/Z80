#include "src/Window/Window.h"
#include "src/Window/Menu.h"

namespace Window {
  Window::Window(PixelGameEngine* engine, std::string id): Base(engine), windowId(id) {}

  void Window::Select(DimensionT dimensions) {
    this->absolute = dimensions.absolute; this->size = dimensions.size;
    if (menu != nullptr) menu->Select({ dimensions.absolute, olc::vi2d(0, 0) });
  }

  void Window::Menu() {
    if (menu == nullptr) return;

    if (menu->bEnabled) menu->Reset();
    else menu->bEnabled = true;
  }
};