#pragma once
#include "src/Utils/Utils.h"

namespace Window {

class Window {
public:

  virtual void Preinitialize() {};
  virtual void Initialize(DimensionT) = 0;
  virtual void Preprocess() {};
  virtual void Process(PixelGameEngine* GameEngine) = 0;
  virtual void Draw(PixelGameEngine* GameEngine) = 0;

  virtual void Lock() {};
  virtual void Unlock() {};

};
};
