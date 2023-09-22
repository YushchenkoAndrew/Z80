#pragma once
#include "Defs.h"

namespace Bus {

class Led : public Window::Window, public Device {
public:
  Led(Bus* b): bus(b) {}

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    for (int32_t i = 0, data = this->data; i < 8; i++, data = data << 1) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * vStep + vOffset;

      if (data & 0x80) GameEngine->FillRectDither(pos, olc::vi2d(9, 9), ~AnyType<WHITE, ColorT>::GetValue());
      else GameEngine->FillRectDither(pos, olc::vi2d(9, 9), ~AnyType<DARK_GREY, ColorT>::GetValue());
    }
  }

  uint8_t Read(uint32_t addr, bool) { return 0; }
  uint8_t Write(uint32_t addr, uint8_t data, bool) { return (this->data = data); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(12, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  Bus* bus;
  uint8_t data = 0x00;
};

};
