#pragma once
#include "Lcd.h"

namespace Bus {


#define BIT(word, pos) ((word >> pos) & 0x01)
#define MASK(pos) (1 << (pos))

class PPI : public Window::Window, public Device {
  enum ModeT { MODE0, MODE1, MODE2 };
  enum CtrlSign { INTR_B, IBF_B, STB_B, INTR_A, STB_A, IBF_A, ACK_A, OBF_A };

public:
  struct REG { enum { A, B, C, CONTROL, CH = 2, CL = 3 }; };
  enum { DATA2REG, REG2DATA, PORT2REG, REG2PORT };

  PPI(Bus* b): bus(b) {}

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    int32_t index = 0;
    olc::vi2d pos = absolute;

    auto DrawReg = [&](std::string name, uint16_t val) {
      GameEngine->DrawString(pos, name, *AnyType<DARK_GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + vOffset, Utils::Int2Hex(val), *AnyType<GREY, ColorT>::GetValue());

      pos.x += vOffset.x + vStep.x * 3;
    };

    DrawReg("A", regA()); DrawReg("B", regB()); DrawReg("C", regC());
  }


  uint8_t Read(uint32_t addr, bool) { 
    switch (addr & 3) {
      case REG::A: return regA(Int2Type<REG2DATA>());
      case REG::B: return regB(Int2Type<REG2DATA>());
      case REG::C: return regC(Int2Type<REG2DATA>());
    }

    return 0x00;
  }


  uint8_t Write(uint32_t addr, uint8_t data, bool) {
    switch (addr & 3) {
      case REG::A: return regA(Int2Type<DATA2REG>(), data);
      case REG::B: return regB(Int2Type<DATA2REG>(), data);
      case REG::C: return regC(Int2Type<DATA2REG>(), data);
    }


    // Mode set
    if (BIT(data, 7)) {
      options[REG::CL].first = options[REG::B].first = BIT(data, 2) ? MODE1 : MODE0;
      options[REG::CH].first = options[REG::A].first = BIT(data, 6) ? MODE2 : BIT(data, 5) ? MODE1 : MODE0;

      options[REG::CL].second = BIT(data, 0); options[REG::B].second = BIT(data, 1);
      options[REG::CH].second = BIT(data, 3);  options[REG::A].second = BIT(data, 4);
    } else {
      uint8_t mask = MASK((data >> 1) & 0x07);
      regC() = regC() & (mask ^ 0xFF) | (BIT(data, 0) ? mask : 0x00);
    }

    return data;
  }

public:
  inline uint8_t regA(Int2Type<REG2DATA>) {
    if (options[REG::A].first == MODE1) regC() = (regC() | MASK(STB_A)) & ((MASK(IBF_A) | MASK(INTR_A)) ^ 0xFF);
    return regA();
  }

  inline uint8_t regA(Int2Type<DATA2REG>, uint8_t v) {
    if (options[REG::A].first == MODE1) regC() = regC() & ((MASK(OBF_A) | MASK(ACK_A) | MASK(INTR_A)) ^ 0xFF);
    return (regA() = v);
  }

  inline uint8_t regA(Int2Type<PORT2REG>, uint8_t v) {
    if (!options[REG::A].second) return 0x00;
    if (options[REG::A].first == MODE1) regC() = (regC() & (MASK(STB_A) ^ 0xFF)) | MASK(IBF_A) | MASK(INTR_A);

    return (regs[REG::A] = v);
  }

  inline uint8_t regA(Int2Type<REG2PORT>) {
    if (options[REG::A].second) return 0x00;
    if (options[REG::A].first == MODE1) regC() = regC() | MASK(OBF_A) | MASK(ACK_A) | MASK(INTR_A);
    return regs[REG::A];
  }


  inline uint8_t regB(Int2Type<REG2DATA>) { 
    if (options[REG::B].first == MODE1) regC() = (regC() | MASK(STB_B)) & ((MASK(IBF_B) | MASK(INTR_B)) ^ 0xFF);
    return regB();
  }

  inline uint8_t regB(Int2Type<DATA2REG>, uint8_t v) { 
    if (options[REG::B].first == MODE1) regC() = regC() & ((MASK(IBF_B) | MASK(STB_B) | MASK(INTR_B)) ^ 0xFF);
    return (regB() = v);
  }

  inline uint8_t regB(Int2Type<PORT2REG>, uint8_t v) {
    if (!options[REG::B].second) return 0x00;
    if (options[REG::B].first == MODE1) regC() = (regC() & (MASK(STB_B) ^ 0xFF)) | MASK(IBF_B) | MASK(INTR_B);

    return (regs[REG::B] = v);
  }

  inline uint8_t regB(Int2Type<REG2PORT>) { 
    if (options[REG::B].second) return 0x00;
    if (options[REG::B].first == MODE1) regC() = regC() | MASK(IBF_B) | MASK(STB_B) | MASK(INTR_B);
    return options[REG::B].second ? 0x00 : regs[REG::B];
  }

  inline uint8_t regC(Int2Type<REG2DATA>) { return regC(); }
  inline uint8_t regC(Int2Type<DATA2REG>, uint8_t v) { return (regC() = v); }
  inline uint8_t regC(Int2Type<PORT2REG>, uint8_t v) {
    if (options[REG::B].first != MODE0) return 0x00;

    if (options[REG::CH].second) regs[REG::C] = (regs[REG::C] & 0x0F) | (v & 0xF0);
    if (options[REG::CL].second) regs[REG::C] = (regs[REG::C] & 0xF0) | (v & 0x0F);
    return regs[REG::C];
  }

  inline uint8_t regC(Int2Type<REG2PORT>) {
    uint8_t acc = 0x00;

    if (options[REG::CH].first == MODE0 && !options[REG::CH].second) acc = acc | (regs[REG::C] & 0xF0);
    if (options[REG::CL].first == MODE0 && !options[REG::CL].second) acc = acc | (regs[REG::C] & 0x0F);

    return acc;
  }

public:
  inline uint8_t& regA() { return regs[REG::A]; }
  inline uint8_t& regB() { return regs[REG::B]; }
  inline uint8_t& regC() { return regs[REG::C]; }

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
#undef MASK

};
