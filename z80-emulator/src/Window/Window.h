#pragma once

#include "src/Defs.h"
#include "src/Window/Base.h"
#include "src/Window/Event/StrokeAction.h"

namespace Window {

class Window : virtual public Base {
public:
  Window(PixelGameEngine* engine, std::string id): Base(engine), windowId(std::move(id)) {}

  // virtual void Select(DimensionT) = 0;
  virtual void Select(DimensionT dimensions) { this->absolute = dimensions.first; this->size = dimensions.second; }
  virtual void Process() {};
  virtual void Draw() {};

  virtual const StrokeT<>& GetCommands() const {
    static const StrokeT<> empty = ::Window::StrokeT<>::Init({});
    return empty;
  };

public:
  const std::string windowId;
  
protected:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);
};
}; // namespace Window
