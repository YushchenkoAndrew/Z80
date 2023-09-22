#pragma once
#include "Switch.h"

namespace Bus {

class HexDisplay : public Window::Window, public Device {
  enum { HEX };

public:
  HexDisplay(Bus* b): bus(b) {}

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    olc::vi2d pos = absolute + vOffset;

    Draw(Int2Type<HEX>(), GameEngine, data >> 8, pos);
    Draw(Int2Type<HEX>(), GameEngine, data, pos + olc::vi2d(vStep.x * 2, 0));
  }

  uint8_t Read(uint32_t addr, bool) { return 0; }
  uint8_t Write(uint32_t addr, uint8_t data, bool) { return (this->data = (this->data << 8) | data); }

private:
  void Draw(Int2Type<HEX>, PixelGameEngine* GameEngine, uint8_t data, olc::vi2d pos) {
    if (data & 0x01) GameEngine->FillRectDither(pos, olc::vi2d(vStep.x, 2), ~AnyType<DARK_GREY, ColorT>::GetValue());
    else GameEngine->FillRectDither(pos, olc::vi2d(vStep.x, 2), ~AnyType<WHITE, ColorT>::GetValue());

    if ((data = data >> 1) & 0x01) GameEngine->FillRectDither(pos + olc::vi2d(vStep.x, 0), olc::vi2d(2, vStep.y), ~AnyType<DARK_GREY, ColorT>::GetValue());
    else GameEngine->FillRectDither(pos + olc::vi2d(vStep.x, 0), olc::vi2d(2, vStep.y), ~AnyType<WHITE, ColorT>::GetValue());

    if ((data = data >> 1) & 0x01) GameEngine->FillRectDither(pos + olc::vi2d(vStep.x, vStep.y), olc::vi2d(2, vStep.y), ~AnyType<DARK_GREY, ColorT>::GetValue());
    else GameEngine->FillRectDither(pos + olc::vi2d(vStep.x, vStep.y), olc::vi2d(2, vStep.y), ~AnyType<WHITE, ColorT>::GetValue());

    if ((data = data >> 1) & 0x01) GameEngine->FillRectDither(pos + olc::vi2d(0, vStep.y << 1), olc::vi2d(vStep.x, 2), ~AnyType<DARK_GREY, ColorT>::GetValue());
    else GameEngine->FillRectDither(pos + olc::vi2d(0, vStep.y << 1), olc::vi2d(vStep.x, 2), ~AnyType<WHITE, ColorT>::GetValue());

    if ((data = data >> 1) & 0x01) GameEngine->FillRectDither(pos + olc::vi2d(0, vStep.y), olc::vi2d(2, vStep.y), ~AnyType<DARK_GREY, ColorT>::GetValue());
    else GameEngine->FillRectDither(pos + olc::vi2d(0, vStep.y), olc::vi2d(2, vStep.y), ~AnyType<WHITE, ColorT>::GetValue());

    if ((data = data >> 1) & 0x01) GameEngine->FillRectDither(pos + olc::vi2d(0, 0), olc::vi2d(2, vStep.y), ~AnyType<DARK_GREY, ColorT>::GetValue());
    else GameEngine->FillRectDither(pos + olc::vi2d(0, 0), olc::vi2d(2, vStep.y), ~AnyType<WHITE, ColorT>::GetValue());

    if ((data = data >> 1) & 0x01) GameEngine->FillRectDither(pos + olc::vi2d(0, vStep.y), olc::vi2d(vStep.x, 2), ~AnyType<DARK_GREY, ColorT>::GetValue());
    else GameEngine->FillRectDither(pos + olc::vi2d(0, vStep.y), olc::vi2d(vStep.x, 2), ~AnyType<WHITE, ColorT>::GetValue());
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(12, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  Bus* bus;
  uint16_t data = 0x0000;
};

};
