#pragma once
#include "HexDisplay.h"

namespace Bus {

#define BIT_M(flag, n) (((uint8_t)flag) << n)
#define IS_BIT(byte, n) (byte & (0x01 << n))

class LCD : public Window::Window, public Device {
  enum { SEGMENT };
  enum { CLEAR=1, RETURN=2, MODE=4, DISPLAY=8, CURSOR=16, FUNCTION=32, CGRAM=64, DDRAM=128 };

public:
  LCD(Bus* b): bus(b) { Instruction(Int2Type<CLEAR>(), 0); }

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

    Draw(Int2Type<CURSOR>(), GameEngine);
  }

  uint8_t Read(uint32_t addr, bool) { return 0; }
  uint8_t Write(uint32_t addr, uint8_t data, bool bDataRegister) {
    if (!bDataRegister) {
      if (IS_BIT(data, 7)) return Instruction(Int2Type<DDRAM>(), data);
      if (IS_BIT(data, 6)) return Instruction(Int2Type<CGRAM>(), data);
      if (IS_BIT(data, 5)) return Instruction(Int2Type<FUNCTION>(), data);
      if (IS_BIT(data, 4)) return Instruction(Int2Type<CURSOR>(), data);
      if (IS_BIT(data, 3)) return Instruction(Int2Type<DISPLAY>(), data);
      if (IS_BIT(data, 2)) return Instruction(Int2Type<MODE>(), data);
      if (IS_BIT(data, 1)) return Instruction(Int2Type<RETURN>(), data);
      if (IS_BIT(data, 0)) return Instruction(Int2Type<CLEAR>(), data);

      return 0x00;
    }

    AnyType<-1 , int32_t>::GetValue() = data;
    auto c = foreach<LcdChars, AnyType<-1 , int32_t>>::Key2Value();
    for (int32_t j = 0; j < LCD_SEGMENT_SIZE; j++) buffer[cursor.second][j] = c.val[j];

    Instruction(Int2Type<CURSOR>(), BIT_M(shift, 3) | BIT_M(inc, 2));
    return data;
  }


  void Draw(Int2Type<CURSOR>, PixelGameEngine* GameEngine) {
    if (fBlink.first && (fBlink.second += AnyType<-1, float>::GetValue()) > 1.2f) fBlink.second -= 1.2f;
    if (fBlink.first && fBlink.second > 0.6f) return;

    auto pos = absolute + olc::vi2d(cursor.second % 16 , cursor.second / 16) * vStep;
    GameEngine->FillRect(pos, size, ~AnyType<WHITE, ColorT>::GetValue());
  }

  void Draw(Int2Type<SEGMENT>, PixelGameEngine* GameEngine, SegmentT& buffer, const olc::vi2d& absolute) {
    for (int32_t j = 0; j < buffer.size(); j++) {
      for (int32_t i = 0; i < sizeof(uint8_t) * 8; i++) {
        const auto pos = absolute + olc::vi2d(j, i) * vStep;
        ColorT color = (buffer[j] & (0x80 >> i)) ? AnyType<WHITE, ColorT>::GetValue() : bg;

        GameEngine->FillRectDither(pos, vStep, ~color, 1);
      }
    }
  }

  inline uint8_t Instruction(Int2Type<CLEAR>, uint8_t data) {
    for (auto& seg : buffer) {
      for (auto& byte : seg) byte = 0x00;
    }

    cursor.second = (cursor.second / 16) * 16;
    return data;
  }

  inline uint8_t Instruction(Int2Type<RETURN>, uint8_t data) { cursor.second = 0x00; return data; }
  inline uint8_t Instruction(Int2Type<MODE>, uint8_t data) { inc = IS_BIT(data, 1); shift = IS_BIT(data, 0); return data; }

  inline uint8_t Instruction(Int2Type<DISPLAY>, uint8_t data) {
    bg = IS_BIT(data, 2) ? AnyType<DARK_GREY, ColorT>::GetValue() : AnyType<VERY_DARK_GREY, ColorT>::GetValue();
    cursor.first = IS_BIT(data, 1);
    fBlink.first = IS_BIT(data, 0);
    return data;
  }

  uint8_t Instruction(Int2Type<CURSOR>, uint8_t data) {
    if (IS_BIT(data, 3)) {
      for (int32_t i = buffer.size() - 1 ; IS_BIT(data, 2) && i >= cursor.second; i--) buffer[i] = buffer[i - 1];
      for (int32_t i = 0; !IS_BIT(data, 2) && i < cursor.second; i--) buffer[i] = buffer[i + 1];
    } else { cursor.second = (cursor.second + (IS_BIT(data, 2) ? 1 : -1)) % buffer.size(); }

    return data;
  }

  // TODO: For now this doest make sense to impl
  uint8_t Instruction(Int2Type<FUNCTION>, uint8_t data) { return 0x00; }
  uint8_t Instruction(Int2Type<CGRAM>, uint8_t data) { return 0x00; }
  uint8_t Instruction(Int2Type<DDRAM>, uint8_t data) { return 0x00; }


private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(2, 2);
  const olc::vi2d vOffset = olc::vi2d(2, 2);

  bool inc = true; // addr shift inc/dec
  bool shift = false; // shift whole display

  ColorT bg = AnyType<VERY_DARK_GREY, ColorT>::GetValue();
  std::pair<bool, uint32_t> cursor = std::pair(true, 0); // first - show cursor on/off

  // Variables defines animation duration
  std::pair<bool, float> fBlink = std::pair(true, 0.f);

  Bus* bus;
  uint16_t data = 0x0000;

  std::array<SegmentT, 32> buffer;
};

#undef BIT_M
#undef IS_BIT
};
