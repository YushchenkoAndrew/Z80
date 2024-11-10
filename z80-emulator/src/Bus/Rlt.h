#pragma once
#include "Keyboard.h"

namespace Bus {

#define CT0(p)  std::get<0>(p)
#define CT1(p)  std::get<1>(p)
#define CT2(p)  std::get<2>(p)

class RLT : public Window::Window, public Device {

typedef std::tuple<uint16_t, uint16_t, uint16_t> RegT;

public:
  RLT(Bus* b): bus(b) {}

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {
    // const auto mouse = GameEngine->GetMousePos();
    // const bool bPressed = GameEngine->GetMouse(0).bPressed;

    // olc::vi2d pos = absolute - olc::vi2d(1, 1);
    // olc::vi2d size = name.second + olc::vi2d(2, 2);

    // if (bPressed && mouse.x > pos.x && mouse.y > pos.y && mouse.x < pos.x + size.x && mouse.y < pos.y + size.y) {
    //   bEnabled = bEnabled ^ true;
    // }

    // if (!bEnabled) return;
  }

  void Draw(PixelGameEngine* GameEngine) {
    Utils::Lock l(mutex);

    olc::vi2d pos = absolute; olc::vi2d vNextLine = olc::vi2d(0, vStep.y);

    GameEngine->DrawString(pos, name.first, *AnyType<DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + vNextLine, "CTRL", *AnyType<DARK_GREY, ColorT>::GetValue());

    const olc::vi2d vOffset = olc::vi2d(name.second.x + vStep.x, 0);

    for (uint8_t i = 0; i < 3; i++) {
      GameEngine->DrawString(pos + vOffset, Utils::Int2Hex(GetValue(counter, i)), *AnyType<GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + vOffset + vNextLine, Utils::Int2Hex(GetValue(control, i)), *AnyType<GREY, ColorT>::GetValue());

      pos.x += vStep.x * 3;
    }
  }

  void Interrupt();

public:
  uint8_t Read(uint32_t addr, bool) { 
    uint16_t& ctl = GetValue(control, addr);
    const uint16_t value = GetValue(counter, addr);
    
    switch ((ctl & 0x0300) >> 8) {
      case 0x00: return (uint8_t)(value & 0x00FF);
      case 0x01: return (uint8_t)((value & 0xFF00) >> 8);
      case 0x02: {
        ctl = (ctl & 0x00FF);
        return (uint8_t)(value & 0x00FF);
      }
    }

    return 0x00;
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) {
    switch (addr & 0xFF) {
      // TODO: Make it possible to write into high/low byte
      case 0x00: return CT0(counter) = data;
      case 0x01: return CT1(counter) = data;
      case 0x02: return CT2(counter) = data;
      case 0x03: break;
      default: return data;
    }

    const uint8_t index = (data & 0xC0) >> 6;
    uint16_t& ctrl = GetValue(control, index);

    switch ((data & 0x30) >> 4) {
      case 0x00: {
        ctrl = (uint16_t)data | (uint16_t)(0x0200);
        GetValue(copy, index) = GetValue(counter, index); // Copy current state of the reg to the copy reg
        break;
      }

      case 0x01: ctrl = (uint16_t)data | (uint16_t)(0x0000); break; // LOW BYTE
      case 0x02: ctrl = (uint16_t)data | (uint16_t)(0x0100); break; // HIGH BYTE
      case 0x03: ctrl = (uint16_t)data | (uint16_t)(0x0200); break; // LOW BYTE + HIGH BYTE
    }

    return data;
  }

private:
  uint16_t& GetValue(RegT& reg, uint8_t addr) {
    switch (addr & 0xFF) {
      case 0x00: return CT0(reg);
      case 0x01: return CT1(reg);
      case 0x02: return CT2(reg);
    }

    static uint16_t none;
    return none;
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);

  Bus* bus;

  std::pair<const char*, olc::vi2d> name = std::pair("RLT", olc::vi2d(8 * 4, 8));

  std::mutex mutex;
  
  RegT copy;
  RegT counter;

  RegT control;
};
};
