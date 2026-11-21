#pragma once

#include "src/Defs.h"
#include "src/Window/Base.h"
#include "src/Window/Types/StrokeAction.h"
#include <memory>

namespace Window {
  struct DimensionT { olc::vi2d absolute; olc::vi2d size; };

class Menu;

class Window : virtual public Base {
public:

  Window(PixelGameEngine* engine, std::string id);

  virtual void Select(DimensionT dimensions);
  virtual void Preprocess() {};
  virtual void Process() {};
  virtual void Draw() {};

  virtual void Menu();

  virtual olc::vi2d GetSize() { return size; }
  virtual olc::vi2d GetAbsolute() { return absolute; }

  virtual const StrokeT<>& GetCommands() const {
    static const StrokeT<> empty = ::Window::StrokeT<>::Init({});
    return empty;
  };

public:
  const std::string windowId;

protected:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);
  std::unique_ptr<::Window::Menu> menu = nullptr;
};
}; // namespace Window
