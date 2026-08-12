#pragma once
#include "src/Window/Stroke.h"

namespace Window {

class Window {
public:
  virtual void Initialize() {};
  virtual void Select(DimensionT) = 0;
  virtual void Preprocess() {};
  virtual void Process(PixelGameEngine *GameEngine) = 0;
  virtual void Draw(PixelGameEngine *GameEngine) = 0;
  virtual void Highlight(PixelGameEngine *GameEngine) {};
  virtual void SelectHighlight(int32_t i) {};

  virtual std::string GetId() { return ""; };
  virtual std::string GetName() { return ""; };
  virtual std::string GetBindings() { return ""; };

  virtual const StrokeT<>& GetCommands() const {};

  virtual void Lock() {};
  virtual void Unlock() {};
};
}; // namespace Window
