#pragma once
#include "Lcd.h"

namespace Bus {


#define BIT(word, pos) ((word >> pos) & 0x01)

class PPI : public Window::Window, public Device {
  enum ModeT { MODE0, MODE1, MODE2 };
  // typedef std::tuple<ModeT, bool> RegT;

public:
  enum REG { A, B, C, CONTROL, CH = 2, CL = 3 };
  enum { INPUT, OUTPUT };

  PPI(Bus* b): bus(b) {}

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    int32_t index = 0;
    olc::vi2d pos = absolute;

    auto DrawReg = [&](std::string name, uint16_t val) {
      GameEngine->DrawString(pos, name, ~AnyType<DARK_GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + vOffset, Int2Hex(val), ~AnyType<GREY, ColorT>::GetValue());

      pos.x += vOffset.x + vStep.x * 3;
    };

    DrawReg("A", regA(Int2Type<OUTPUT>()));
    DrawReg("B", regB(Int2Type<OUTPUT>()));
    DrawReg("C", regC(Int2Type<OUTPUT>()));
  }


  uint8_t Read(uint32_t addr, bool) { 
    switch (addr & 3) {
      case REG::A: return regA(Int2Type<INPUT>());
      case REG::B: return regB(Int2Type<INPUT>());
      case REG::C: return regC(Int2Type<INPUT>());
    }

    return 0x00;
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) {
    switch (addr & 3) {
      case REG::A: return regA(data);
      case REG::B: return regB(data);
      case REG::C: return regC(data);
    }

    // Mode set
    if (BIT(data, 7)) {
      if (BIT(data, 2)) {} // TODO:
      else { options[REG::CL].second = BIT(data, 0); options[REG::B].second = BIT(data, 1); }

      if (BIT(data, 6)) {} // TODO:
      else if (BIT(data, 5)) {} // TODO:
      else { options[REG::CH].second = BIT(data, 3);  options[REG::A].second = BIT(data, 4); }
    }

    // Bit set/reset
    if (BIT(data, 7)) {
      uint8_t mask = 0x01 << ((data >> 1) & 0x07);

      // TODO:
      // if (!regC.first.first) regC.first.second = (data >> 4) & 0x0F;
      // if (!regC.second.first) regC.first.second = (data & 0x0F);
    }

    return data;
  }

  inline uint8_t regA(Int2Type<INPUT>) { return options[REG::A].second ? regs[REG::A] : 0x00; }
  inline uint8_t regB(Int2Type<INPUT>) { return options[REG::B].second ? regs[REG::B] : 0x00; }
  inline uint8_t regC(Int2Type<INPUT>) { 
    uint8_t acc = 0x00;

    if (options[REG::CH].second) acc = acc | (regs[REG::C] & 0xF0);
    if (options[REG::CL].second) acc = acc | (regs[REG::C] & 0x0F);

    return acc;
  }

  inline uint8_t regCH() { return (regs[REG::C] >> 4) & 0x0F; }
  inline uint8_t regCL() { return regs[REG::C] & 0x0F; }

  inline uint8_t regA(Int2Type<OUTPUT>) { return regs[REG::A]; }
  inline uint8_t regB(Int2Type<OUTPUT>) { return regs[REG::B]; }
  inline uint8_t regC(Int2Type<OUTPUT>) { return regs[REG::C]; }

  inline uint8_t regA(uint8_t v) { return options[REG::A].second ? 0x00 : (regs[REG::A] = v); }
  inline uint8_t regB(uint8_t v) { return options[REG::B].second ? 0x00 : (regs[REG::B] = v); }
  inline uint8_t regC(uint8_t v) {
    if (!options[REG::CH].second) regs[REG::C] = (regs[REG::C] & 0x0F) | (v & 0xF0);
    if (!options[REG::CL].second) regs[REG::C] = (regs[REG::C] & 0xF0) | (v & 0x0F);
    return v;
  }

private:
  inline std::string Int2Hex(int32_t i, int32_t width = 2) { 
    std::stringstream ss; ss << std::setfill('0') << std::setw(width) << std::hex << std::uppercase << +i;
    return ss.str();
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(16, 0);

  Bus* bus;

  std::array<uint8_t, 3> regs;
  std::array<std::pair<ModeT, bool>, 4> options;
};

#undef BIT

};
