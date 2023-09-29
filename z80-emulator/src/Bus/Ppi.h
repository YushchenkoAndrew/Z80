#pragma once
#include "Lcd.h"

namespace Bus {

#define BIT(word, pos) ((word >> pos) & 0x01)
#define MODE(p)     std::get<0>(p)
#define IS_INPUT(p) std::get<1>(p)
#define VALUE(p)    std::get<2>(p)

class PPI : public Window::Window, public Device {
  enum ModeT { MODE0, MODE1, MODE2 };
  typedef std::tuple<ModeT, bool, uint8_t> RegT;

public:
  enum REG { A, B, C, CONTROL };

  PPI(Bus* b): bus(b) {}

  void Initialize(DimensionT) {}
  void Process(PixelGameEngine* GameEngine) {}
  void Draw(PixelGameEngine* GameEngine) {}

  uint8_t Read(uint32_t addr, bool) { 
    switch (addr & 3) {
      case REG::A: return IS_INPUT(regA) ? VALUE(regA) : 0x00;
      case REG::B: return IS_INPUT(regB) ? VALUE(regB) : 0x00;
      case REG::C: {
        uint8_t acc = 0x00;
        if (IS_INPUT(regC.first)) acc = acc | (VALUE(regC.first)  << 4);
        if (IS_INPUT(regC.second)) acc = acc | VALUE(regC.second);
        return acc;
      }
    }

    return 0x00;
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) {
    switch (addr & 3) {
      case REG::A: return IS_INPUT(regA) ? 0x00 : (VALUE(regA) = data);
      case REG::B: return IS_INPUT(regB) ? 0x00 : (VALUE(regB) = data);
      case REG::C: {
        if (!IS_INPUT(regC.first))  VALUE(regC.first) = (data >> 4) & 0x0F;
        if (!IS_INPUT(regC.second)) VALUE(regC.second) = (data & 0x0F);
        return data;
      }
    }

    // Mode set
    if (BIT(data, 7)) {
      if (BIT(data, 2)) {} // TODO:
      else { IS_INPUT(regC.second) = BIT(data, 0); IS_INPUT(regB) = BIT(data, 0); }

      if (BIT(data, 6)) {} // TODO:
      else if (BIT(data, 5)) {} // TODO:
      else { IS_INPUT(regC.first) = BIT(data, 3); IS_INPUT(regA) = BIT(data, 4); }
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
  
  inline bool IsReg(uint32_t addr, int reg) { return (addr & 0x03) == reg; }

private:
  Bus* bus;

  RegT regA = { MODE0, true, 0 };
  RegT regB = { MODE0, true, 0 };
  std::pair<RegT, RegT> regC;
};

#undef BIT

};
