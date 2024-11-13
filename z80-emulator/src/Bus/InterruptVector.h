#pragma once
#include "PPI.h"

namespace Bus {

class InterruptVector : public Window::Window, public Device {

public:
  enum IRQ { NONE1, NONE2, CT2, CT1, RLT, TxRDT, RxRDY, KEYBOARD };

  InterruptVector(Bus* b): bus(b) {}

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {
    if (state ^ 0xff) Interrupt();
  }

  void Draw(PixelGameEngine* GameEngine) {
    olc::vi2d pos = absolute;
    const olc::vi2d vOffset = olc::vi2d(3 * 8 + vStep.x, 0);

    GameEngine->DrawString(pos, "IRQ", *AnyType<DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + vOffset, Utils::Int2Bin(state), *AnyType<GREY, ColorT>::GetValue());

    pos += olc::vi2d(0, vStep.y + vStep.y / 2);
    GameEngine->DrawString(pos, "REG", *AnyType<DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + vOffset, Utils::Int2Bin(enabled), *AnyType<GREY, ColorT>::GetValue());
  }

  void Interrupt();

  uint8_t Read(uint32_t, bool) { return state; }
  uint8_t Write(uint32_t, uint8_t data, bool) { return enabled = data; }


  inline void SetFlag(uint8_t flag) { state |= (uint8_t)(enabled & (1 << flag)); }
  inline void ResetFlag(uint8_t flag) { state &= ~(uint8_t)(enabled & (1 << flag)); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(16, 0);

  Bus* bus;

  uint8_t state   = 0xff;
  uint8_t enabled = 0x80;

  // std::array<uint8_t, 3> regs;
  // std::array<std::pair<ModeT, bool>, 4> options;
};

};
