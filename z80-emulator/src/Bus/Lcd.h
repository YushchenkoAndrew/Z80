#pragma once
#include "HexDisplay.h"

namespace Bus {

class LCD : public Window::Window, public Device {
  enum { SEGMENT };

public:
  LCD(Bus* b): bus(b) {
    // buffer[0] = { 0b01111101, 0b10000011, 0b10010011, 0b10010011,  0b01011101 };
    // buffer[1] = { 0b01111110, 0b10001000, 0b10001000, 0b10001000,  0b01111110  };

    std::string str = "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmno";

    for (int32_t i = 0; i < buffer.size(); i++) {
      AnyType<-1 , int32_t>::GetValue() = str[i];
      auto c = foreach<LcdChars, AnyType<-1 , int32_t>>::Key2Value();
      for (int32_t j = 0; j < LCD_SEGMENT_SIZE; j++) buffer[i][j] = c.val[j];
    }
  }

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    for (int32_t i = 0; i < buffer.size(); i++) {
      const auto index = olc::vi2d(i % 16, i / 16);
      const auto pos = absolute + index * (olc::vi2d(LCD_SEGMENT_SIZE, sizeof(uint8_t) * 8) * vStep + vOffset);

      Draw(Int2Type<SEGMENT>(), GameEngine, buffer[i], pos);
    }
  }

  uint8_t Read(uint32_t addr, bool) { return 0; }
  uint8_t Write(uint32_t addr, uint8_t data, bool) { return 0; }

private:
  void Draw(Int2Type<SEGMENT>, PixelGameEngine* GameEngine, SegmentT& buffer, const olc::vi2d& absolute) {
    for (int32_t j = 0; j < buffer.size(); j++) {
      for (int32_t i = 0; i < sizeof(uint8_t) * 8; i++) {
        const auto pos = absolute + olc::vi2d(j, i) * vStep;
        ColorT color = (buffer[j] & (0x80 >> i)) ? AnyType<WHITE, ColorT>::GetValue() : AnyType<DARK_GREY, ColorT>::GetValue();

        GameEngine->FillRectDither(pos, vStep, ~color, 1);
      }
    }
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(2, 2);
  const olc::vi2d vOffset = olc::vi2d(2, 2);

  Bus* bus;
  uint16_t data = 0x0000;

  // std::array<SegmentT, 32> buffer;
  
  // FIXME: Bitmask debug
  std::array<SegmentT, 16 * 6> buffer;
};

};
