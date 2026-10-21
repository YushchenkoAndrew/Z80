#pragma once

#include "src/Defs.h"
#include "src/Window/Base.h"
#include "src/Window/Event/StrokeAction.h"

namespace Window {

class Window : public Base {
public:
  Window(PixelGameEngine* engine): Base(engine) {}

  virtual void Select(DimensionT) = 0;
  virtual void Process() = 0;
  virtual void Draw() = 0;

  virtual const StrokeT<>& GetCommands() const {};

  // virtual void Lock() {};
  // virtual void Unlock() {};
  // virtual void Initialize() {};
  // virtual void Preprocess() {}; // TODO DELETE THIS 
  // virtual void Highlight(PixelGameEngine *GameEngine) {};
  // virtual void SelectHighlight(int32_t i) {};
};
}; // namespace Window
