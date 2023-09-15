#pragma once
#include "src/Defs.h"

namespace Window {

class Window {
public:

  virtual void Initialize(DimensionT) = 0;
  virtual void Process(PixelGameEngine* GameEngine) = 0;
  virtual void Draw(PixelGameEngine* GameEngine) = 0;

};
};
