#include "src/Window/Window.h"
#include "src/Window/Menu.h"

namespace Window {
  void Window::Select(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
    if (menu != nullptr) menu->Select({ dimensions.first, olc::vi2d(0, 0) });
  }

  void Window::Menu() {
    if (menu == nullptr) return;

    if (menu->bEnabled) menu->Reset();
    else menu->bEnabled = true;
  }
};