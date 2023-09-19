#pragma once
#include "Memory.h"


namespace Bus {

namespace Z80 {
  #define LOW_M(word) (word & 0x00FF)
  #define HIGH_M(word) (word & 0xFF00)
  #define BIT_M(pos, state) ((uint8_t)state << pos)

  #define LOW(word) (LOW_M(word))
  #define HIGH(word) (HIGH_M(word) >> 8)
  #define BIT(word, pos) ((word >> pos) & 0x01)

  #define LOW_SET(word, byte) (HIGH_M(word) | LOW_M(byte))
  #define HIGH_SET(word, byte) (LOW_M(word) | HIGH_M((byte << 8)))
  #define BIT_SET(word, pos, state) ((word & ~BIT_M(pos, 1)) | BIT_M(pos, state))

class CPU : public Device {
private:
  /**
   * Flags annotation
   * C   - Carry Flag
   * N   - Add/Subtract
   * P/V - Parity/Overflow Flag
   * H   - Half Carry Flag
   * Z   - Zero Flag
   * S   - Sign Flag
   * X   - Not Used 
   */
  enum FLAG { C, N, PV, X, H, x, Z, S };

  enum REG { AF, BC, DE, HL, ALTERNATIVE, IX = 8, IY, SP, IR, PC };
  
public:
  CPU(Bus* b): bus(b) {}

  void Clock() {
    if (cycles > 0) { cycles--; return; }

    AnyType<-1, int32_t>::GetValue() = Read();
    foreach<Instructions, CPU>::Key2Process(this);
  }

  uint8_t Read(uint32_t addr) { return 0x00; }
  uint8_t Write(uint32_t addr, uint8_t data) { return 0x00; }

  std::string Disassemble();
  
private:
  uint8_t Read();

public:
  template<int32_t T>
  inline void Process(Int2Type<T>) {
    // TODO:
    // error(peekPrev(), "Unknown operation");
    // return std::make_shared<Statement>();
  }

  inline void Process(Int2Type<Instruction::NOP>) { cycles = 4; }
  inline void Process(Int2Type<Instruction::DJNZ_D>) {}
  inline void Process(Int2Type<Instruction::JR_NZ_D>) {}
  inline void Process(Int2Type<Instruction::JR_NC_D>) {}
  inline void Process(Int2Type<Instruction::LD_B_B>) {}
  inline void Process(Int2Type<Instruction::LD_D_B>) {}
  inline void Process(Int2Type<Instruction::LD_H_B>) {}
  inline void Process(Int2Type<Instruction::LD_hl_B>) {}
  inline void Process(Int2Type<Instruction::ADD_A_B>) {}
  inline void Process(Int2Type<Instruction::SUB_B>) {}
  inline void Process(Int2Type<Instruction::AND_B>) {}
  inline void Process(Int2Type<Instruction::OR_B>) {}
  inline void Process(Int2Type<Instruction::RET_NZ>) {}
  inline void Process(Int2Type<Instruction::RET_NC>) {}
  inline void Process(Int2Type<Instruction::RET_PO>) {}
  inline void Process(Int2Type<Instruction::RET_P>) {}

  inline void Process(Int2Type<Instruction::LD_BC_NN>) {}
  inline void Process(Int2Type<Instruction::LD_DE_NN>) {}
  inline void Process(Int2Type<Instruction::LD_HL_NN>) {}
  inline void Process(Int2Type<Instruction::LD_SP_NN>) {}
  inline void Process(Int2Type<Instruction::LD_B_C>) {}
  inline void Process(Int2Type<Instruction::LD_D_C>) {}
  inline void Process(Int2Type<Instruction::LD_H_C>) {}
  inline void Process(Int2Type<Instruction::LD_hl_C>) {}
  inline void Process(Int2Type<Instruction::ADD_A_C>) {}
  inline void Process(Int2Type<Instruction::SUB_C>) {}
  inline void Process(Int2Type<Instruction::AND_C>) {}
  inline void Process(Int2Type<Instruction::OR_C>) {}
  inline void Process(Int2Type<Instruction::POP_BC>) {}
  inline void Process(Int2Type<Instruction::POP_DE>) {}
  inline void Process(Int2Type<Instruction::POP_HL>) {}
  inline void Process(Int2Type<Instruction::POP_AF>) {}

  // TODO:





private:
  std::string sprintf(std::string str, int32_t byte);

  inline uint16_t& regAF() { return reg[REG::AF]; }
  inline uint16_t& regBC() { return reg[REG::BC]; }
  inline uint16_t& regDE() { return reg[REG::DE]; }
  inline uint16_t& regHL() { return reg[REG::HL]; }
  inline uint16_t& regIX() { return reg[REG::IX]; }
  inline uint16_t& regIY() { return reg[REG::IY]; }
  inline uint16_t& regSP() { return reg[REG::SP]; }
  inline uint16_t& regPC() { return reg[REG::PC]; }

  inline uint8_t regA() { return HIGH(reg[REG::AF]); }
  inline uint8_t regB() { return HIGH(reg[REG::BC]); }
  inline uint8_t regD() { return HIGH(reg[REG::DE]); }
  inline uint8_t regH() { return HIGH(reg[REG::HL]); }
  inline uint8_t regI() { return HIGH(reg[REG::IR]); }

  inline uint8_t regF() { return LOW(reg[REG::AF]); }
  inline uint8_t regC() { return LOW(reg[REG::BC]); }
  inline uint8_t regE() { return LOW(reg[REG::DE]); }
  inline uint8_t regL() { return LOW(reg[REG::HL]); }
  inline uint8_t regR() { return LOW(reg[REG::IR]); }


  inline CPU* regA(uint8_t b) { reg[REG::AF] = HIGH_SET(reg[REG::AF], b); return this; }
  inline CPU* regB(uint8_t b) { reg[REG::BC] = HIGH_SET(reg[REG::BC], b); return this; }
  inline CPU* regD(uint8_t b) { reg[REG::DE] = HIGH_SET(reg[REG::DE], b); return this; }
  inline CPU* regH(uint8_t b) { reg[REG::HL] = HIGH_SET(reg[REG::HL], b); return this; }
  inline CPU* regI(uint8_t b) { reg[REG::IR] = HIGH_SET(reg[REG::IR], b); return this; }

  inline CPU* regF(uint8_t b) { reg[REG::AF] = LOW_SET(reg[REG::AF], b); return this; }
  inline CPU* regC(uint8_t b) { reg[REG::BC] = LOW_SET(reg[REG::BC], b); return this; }
  inline CPU* regE(uint8_t b) { reg[REG::DE] = LOW_SET(reg[REG::DE], b); return this; }
  inline CPU* regL(uint8_t b) { reg[REG::HL] = LOW_SET(reg[REG::HL], b); return this; }
  inline CPU* regR(uint8_t b) { reg[REG::IR] = LOW_SET(reg[REG::IR], b); return this; }


  inline bool flagC()  { return BIT(regF(), FLAG::C); }
  inline bool flagN()  { return BIT(regF(), FLAG::N); }
  inline bool flagPV() { return BIT(regF(), FLAG::PV); }
  inline bool flagH()  { return BIT(regF(), FLAG::H); }
  inline bool flagZ()  { return BIT(regF(), FLAG::Z); }
  inline bool flagS()  { return BIT(regF(), FLAG::S); }

  inline CPU* flagC(bool f)  { regF(BIT_SET(regF(), FLAG::C, f)); return this; }
  inline CPU* flagN(bool f)  { regF(BIT_SET(regF(), FLAG::N, f)); return this; }
  inline CPU* flagPV(bool f) { regF(BIT_SET(regF(), FLAG::PV,f)); return this; }
  inline CPU* flagH(bool f)  { regF(BIT_SET(regF(), FLAG::H, f)); return this; }
  inline CPU* flagZ(bool f)  { regF(BIT_SET(regF(), FLAG::Z, f)); return this; }
  inline CPU* flagS(bool f)  { regF(BIT_SET(regF(), FLAG::S, f)); return this; }

private:
  uint32_t cycles; // Show how many clock cycles are required to run specific command

  std::array<uint16_t, 13> reg;

  Bus* bus;
};

};
};


