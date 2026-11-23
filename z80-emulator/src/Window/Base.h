#pragma once
#include "src/Defs.h"

namespace Window {

class Base {
public:
  Base(PixelGameEngine* engine): GameEngine(engine) {}
  virtual ~Base() = default; // Good practice: virtual destructor for inheritance
  virtual void Focus(bool state) { bFocused = state; };

protected:
  bool bFocused = false;
  PixelGameEngine* GameEngine;
};
}; 
