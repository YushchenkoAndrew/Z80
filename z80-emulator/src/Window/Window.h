#pragma once

#include "src/Defs.h"
#include "src/Window/Base.h"
#include "src/Window/Types/StrokeAction.h"
#include <memory>

namespace Window {
  typedef std::pair<olc::vi2d, olc::vi2d> DimensionT;

  class Menu;

class Window : virtual public Base {
public:

  Window(PixelGameEngine* engine, std::string id): Base(engine), windowId(std::move(id)) {}

  virtual void Select(DimensionT dimensions);
  virtual void Preprocess() {};
  virtual void Process() {};
  virtual void Draw() {};

  virtual const StrokeT<>& GetCommands() const {
    static const StrokeT<> empty = ::Window::StrokeT<>::Init({});
    return empty;
  };

public:
  const std::string windowId;
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);
  std::unique_ptr<::Window::Menu> menu = nullptr;
};
}; // namespace Window
