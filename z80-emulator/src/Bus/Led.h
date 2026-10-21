#pragma once

#include "src/Bus/Bus.h"
#include "src/Bus/Device.h"
#include "src/Defs.h"
#include "src/Window/Window.h"
#include <memory>

namespace Bus {

class Led : public Window::Window, public Device {
public:
  Led(PixelGameEngine* engine, std::shared_ptr<Bus> bus): bus(std::move(bus)), Window::Window(engine) {}

  void Select(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process() {}

  void Draw() {
    for (int32_t i = 7, val = this->data; i >= 0; i--, val = val << 1) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * vStep + vOffset;

      if (val & 0x80) GameEngine->FillRectDither(pos, olc::vi2d(9, 9), Color::WHITE);
      else GameEngine->FillRectDither(pos, olc::vi2d(9, 9), Color::DARK_GREY);
    }
  }

  uint8_t Read(uint32_t addr, bool) { return 0; }
  uint8_t Write(uint32_t addr, uint8_t data, bool) { return (this->data = data); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(12, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  std::shared_ptr<Bus> bus;
  uint8_t data = 0x00;
};

};
