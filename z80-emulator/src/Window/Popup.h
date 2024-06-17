#pragma once
#include "Terminal.h"

namespace Window {

class Popup : public Window {
public:

  // virtual void Preinitialize() {};
  // virtual void Initialize(DimensionT) = 0;
  // virtual void Preprocess() {};
  // virtual void Highlight(PixelGameEngine* GameEngine) {};
  // virtual void SelectHighlight(int32_t i) {};

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    GameEngine->FillRect(olc::vi2d(10, 10), { 50, 58 }, *AnyType<Colors::BLUE, ColorT>::GetValue());

  }

  // virtual void Lock() {};
  // virtual void Unlock() {};

};
};

