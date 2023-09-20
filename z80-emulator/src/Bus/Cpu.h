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

  #define SIGN(byte) (byte & 0x80)

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
  CPU(Bus* b): bus(b) { regSP() = 0xFFFF; }

  void Clock() {
    if (cycles > 0) { cycles--; return; }

    AnyType<-1, int32_t>::GetValue() = Read();
    foreach<Instructions, CPU>::Key2Process(this);
  }

  DisassembleT Disassemble();
  
private:
  uint8_t Read();
  inline uint16_t Word() { return Read() | (Read() << 8); }

  uint8_t Read(uint32_t addr);
  uint8_t Write(uint32_t addr, uint8_t data);

public:
  template<int32_t T>
  inline void Process(Int2Type<T>) {
    // TODO:
    // error(peekPrev(), "Unknown operation");
    // return std::make_shared<Statement>();
  }

  inline void Process(Int2Type<Instruction::NOP>) { cycles = 4; }

  inline void Process(Int2Type<Instruction::LD_BC_NN>) { cycles = 10; regBC() = Word(); }
  inline void Process(Int2Type<Instruction::LD_DE_NN>) { cycles = 10; regDE() = Word(); }
  inline void Process(Int2Type<Instruction::LD_HL_NN>) { cycles = 10; regHL() = Word(); }
  inline void Process(Int2Type<Instruction::LD_SP_NN>) { cycles = 10; regSP() = Word(); }

  inline void Process(Int2Type<Instruction::POP_BC>) { cycles = 10; regBC() = Pop(); }
  inline void Process(Int2Type<Instruction::POP_DE>) { cycles = 10; regDE() = Pop(); }
  inline void Process(Int2Type<Instruction::POP_HL>) { cycles = 10; regHL() = Pop(); }
  inline void Process(Int2Type<Instruction::POP_AF>) { cycles = 10; regAF() = Pop(); }

  inline void Process(Int2Type<Instruction::PUSH_BC>) { cycles = 11; Push(regBC()); }
  inline void Process(Int2Type<Instruction::PUSH_DE>) { cycles = 11; Push(regDE()); }
  inline void Process(Int2Type<Instruction::PUSH_HL>) { cycles = 11; Push(regHL()); }
  inline void Process(Int2Type<Instruction::PUSH_AF>) { cycles = 11; Push(regAF()); }



  inline void Process(Int2Type<Instruction::ADD_A_A>) { cycles = 4; regA(Add8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::ADD_A_B>) { cycles = 4; regA(Add8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::ADD_A_C>) { cycles = 4; regA(Add8(regA(), regC())); }
  inline void Process(Int2Type<Instruction::ADD_A_D>) { cycles = 4; regA(Add8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::ADD_A_E>) { cycles = 4; regA(Add8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::ADD_A_H>) { cycles = 4; regA(Add8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::ADD_A_L>) { cycles = 4; regA(Add8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::ADD_A_N>) { cycles = 7; regA(Add8(regA(), Read())); }

  inline void Process(Int2Type<Instruction::SUB_A>) { cycles = 4; regA(Sub8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::SUB_B>) { cycles = 4; regA(Sub8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::SUB_C>) { cycles = 4; regA(Sub8(regA(), regC())); }
  inline void Process(Int2Type<Instruction::SUB_D>) { cycles = 4; regA(Sub8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::SUB_E>) { cycles = 4; regA(Sub8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::SUB_H>) { cycles = 4; regA(Sub8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::SUB_L>) { cycles = 4; regA(Sub8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::SUB_N>) { cycles = 7; regA(Sub8(regA(), Read())); }

  inline void Process(Int2Type<Instruction::AND_A>) { cycles = 4; regA(And8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::AND_B>) { cycles = 4; regA(And8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::AND_C>) { cycles = 4; regA(And8(regA(), regC())); }
  inline void Process(Int2Type<Instruction::AND_D>) { cycles = 4; regA(And8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::AND_E>) { cycles = 4; regA(And8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::AND_H>) { cycles = 4; regA(And8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::AND_L>) { cycles = 4; regA(And8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::AND_N>) { cycles = 7; regA(And8(regA(), Read())); }

  inline void Process(Int2Type<Instruction::OR_A>) { cycles = 4; regA(Or8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::OR_B>) { cycles = 4; regA(Or8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::OR_C>) { cycles = 4; regA(Or8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::OR_D>) { cycles = 4; regA(Or8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::OR_E>) { cycles = 4; regA(Or8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::OR_H>) { cycles = 4; regA(Or8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::OR_L>) { cycles = 4; regA(Or8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::OR_N>) { cycles = 7; regA(Or8(regA(), Read())); }


  inline void Process(Int2Type<Instruction::LD_B_A>) { cycles = 4; regB(regA()); }
  inline void Process(Int2Type<Instruction::LD_B_B>) { cycles = 4; regB(regB()); }
  inline void Process(Int2Type<Instruction::LD_B_C>) { cycles = 4; regB(regC()); }
  inline void Process(Int2Type<Instruction::LD_B_D>) { cycles = 4; regB(regD()); }
  inline void Process(Int2Type<Instruction::LD_B_E>) { cycles = 4; regB(regE()); }
  inline void Process(Int2Type<Instruction::LD_B_H>) { cycles = 4; regB(regH()); }
  inline void Process(Int2Type<Instruction::LD_B_L>) { cycles = 4; regB(regL()); }
  inline void Process(Int2Type<Instruction::LD_B_N>) { cycles = 7; regB(Read()); }

  inline void Process(Int2Type<Instruction::LD_D_A>) { cycles = 4; regD(regA()); }
  inline void Process(Int2Type<Instruction::LD_D_B>) { cycles = 4; regD(regB()); }
  inline void Process(Int2Type<Instruction::LD_D_C>) { cycles = 4; regD(regC()); }
  inline void Process(Int2Type<Instruction::LD_D_D>) { cycles = 4; regD(regD()); }
  inline void Process(Int2Type<Instruction::LD_D_E>) { cycles = 4; regD(regE()); }
  inline void Process(Int2Type<Instruction::LD_D_H>) { cycles = 4; regD(regH()); }
  inline void Process(Int2Type<Instruction::LD_D_L>) { cycles = 4; regD(regL()); }
  inline void Process(Int2Type<Instruction::LD_D_N>) { cycles = 7; regD(Read()); }

  inline void Process(Int2Type<Instruction::LD_H_A>) { cycles = 4; regH(regA()); }
  inline void Process(Int2Type<Instruction::LD_H_B>) { cycles = 4; regH(regB()); }
  inline void Process(Int2Type<Instruction::LD_H_C>) { cycles = 4; regH(regC()); }
  inline void Process(Int2Type<Instruction::LD_H_D>) { cycles = 4; regH(regD()); }
  inline void Process(Int2Type<Instruction::LD_H_E>) { cycles = 4; regH(regE()); }
  inline void Process(Int2Type<Instruction::LD_H_H>) { cycles = 4; regH(regH()); }
  inline void Process(Int2Type<Instruction::LD_H_L>) { cycles = 4; regH(regL()); }
  inline void Process(Int2Type<Instruction::LD_H_N>) { cycles = 7; regH(Read()); }




  // TODO:
  inline void Process(Int2Type<Instruction::DJNZ_D>) {}
  inline void Process(Int2Type<Instruction::JR_NZ_D>) {}
  inline void Process(Int2Type<Instruction::JR_NC_D>) {}
  inline void Process(Int2Type<Instruction::LD_hl_B>) {}
  inline void Process(Int2Type<Instruction::RET_NZ>) {}
  inline void Process(Int2Type<Instruction::RET_NC>) {}
  inline void Process(Int2Type<Instruction::RET_PO>) {}
  inline void Process(Int2Type<Instruction::RET_P>) {}

  inline void Process(Int2Type<Instruction::LD_hl_C>) {}

  inline void Process(Int2Type<Instruction::LD_bc_A>) {}
  inline void Process(Int2Type<Instruction::LD_de_A>) {}
  inline void Process(Int2Type<Instruction::LD_nn_HL>) {}
  inline void Process(Int2Type<Instruction::LD_nn_A>) {}
  inline void Process(Int2Type<Instruction::LD_hl_D>) {}
  inline void Process(Int2Type<Instruction::JP_NZ_NN>) {}
  inline void Process(Int2Type<Instruction::JP_NC_NN>) {}
  inline void Process(Int2Type<Instruction::JP_PO_NN>) {}
  inline void Process(Int2Type<Instruction::JP_P_NN>) {}


  inline void Process(Int2Type<Instruction::INC_BC>) {}
  inline void Process(Int2Type<Instruction::INC_DE>) {}
  inline void Process(Int2Type<Instruction::INC_HL>) {}
  inline void Process(Int2Type<Instruction::INC_SP>) {}
  inline void Process(Int2Type<Instruction::LD_hl_E>) {}
  inline void Process(Int2Type<Instruction::JP_NN>) {}
  inline void Process(Int2Type<Instruction::OUT_n_A>) {}
  inline void Process(Int2Type<Instruction::EX_sp_HL>) {}
  inline void Process(Int2Type<Instruction::DI>) {}


  inline void Process(Int2Type<Instruction::INC_B>) {}
  inline void Process(Int2Type<Instruction::INC_D>) {}
  inline void Process(Int2Type<Instruction::INC_H>) {}
  inline void Process(Int2Type<Instruction::INC_hl>) {}
  inline void Process(Int2Type<Instruction::LD_hl_H>) {}
  inline void Process(Int2Type<Instruction::CALL_NZ_NN>) {}
  inline void Process(Int2Type<Instruction::CALL_NC_NN>) {}
  inline void Process(Int2Type<Instruction::CALL_PO_NN>) {}
  inline void Process(Int2Type<Instruction::CALL_P_NN>) {}


  inline void Process(Int2Type<Instruction::DEC_B>) {}
  inline void Process(Int2Type<Instruction::DEC_D>) {}
  inline void Process(Int2Type<Instruction::DEC_H>) {}
  inline void Process(Int2Type<Instruction::DEC_hl>) {}
  inline void Process(Int2Type<Instruction::LD_hl_L>) {}


  inline void Process(Int2Type<Instruction::LD_hl_n>) {}
  inline void Process(Int2Type<Instruction::LD_B_hl>) {}
  inline void Process(Int2Type<Instruction::LD_D_hl>) {}
  inline void Process(Int2Type<Instruction::LD_H_hl>) {}
  inline void Process(Int2Type<Instruction::HALT>) {}
  inline void Process(Int2Type<Instruction::ADD_A_hl>) {}
  inline void Process(Int2Type<Instruction::SUB_hl>) {}
  inline void Process(Int2Type<Instruction::AND_hl>) {}
  inline void Process(Int2Type<Instruction::OR_hl>) {}

  inline void Process(Int2Type<Instruction::RLCA>) {}
  inline void Process(Int2Type<Instruction::RLA>) {}
  inline void Process(Int2Type<Instruction::DAA>) {}
  inline void Process(Int2Type<Instruction::SCF>) {}
  inline void Process(Int2Type<Instruction::LD_hl_A>) {}
  inline void Process(Int2Type<Instruction::RST_00h>) {}
  inline void Process(Int2Type<Instruction::RST_10h>) {}
  inline void Process(Int2Type<Instruction::RST_20h>) {}
  inline void Process(Int2Type<Instruction::RST_30h>) {}






  // TODO:





private:
  std::string sprintf(std::string str, int32_t byte);

  inline uint8_t Add8(uint8_t a, uint8_t b) {
    uint16_t acc = (uint16_t)a + b;

    // When adding operands with similar signs and the result contains a different sign,
    // the Overflow Flag is set
    flagPV(SIGN(a) == SIGN(b) && SIGN(a) != SIGN(acc));

    flagN(false)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagC(acc & 0xFF00)->flagH(((regA() & 0x0F) + (regB() & 0x0F)) & 0xF0);

    return (uint8_t)(acc & 0xFF);
  }

  // TODO:
  inline uint8_t Sub8(uint8_t a, uint8_t b) {
    uint16_t acc = (uint16_t)a + b;

    // // When adding operands with similar signs and the result contains a different sign,
    // // the Overflow Flag is set
    // flagPV(SIGN(a) == SIGN(b) && SIGN(a) != SIGN(acc));

    // flagN(false)->flagS(SIGN(acc))->flagZ((acc & 0xFF) == 0)->flagC(acc & 0xFF00)->flagH(((regA() & 0x0F) + (regB() & 0x0F)) & 0xF0);

    return (uint8_t)(acc & 0xFF);
  }

  inline uint8_t Or8(uint8_t a, uint8_t b)  { return BitOperation(a, b, a | b); }
  inline uint8_t Xor8(uint8_t a, uint8_t b) { return BitOperation(a, b, a ^ b); }
  inline uint8_t And8(uint8_t a, uint8_t b) { auto res = BitOperation(a, b, a & b); regH(true); return res; }


  inline uint8_t BitOperation(uint8_t a, uint8_t b, uint8_t acc) {
    // When adding operands with similar signs and the result contains a different sign,
    // the Overflow Flag is set
    flagPV(SIGN(a) == SIGN(b) && SIGN(a) != SIGN(acc));
    flagN(false)->flagS(SIGN(acc))->flagZ(!acc)->flagC(false)->flagH(false);

    return acc;
  }

  inline uint16_t Pop() { regSP() += 2; return (Read(regSP() - 1) << 8) | Read(regSP() - 2); }
  inline void Push(uint16_t word) { Write(regSP() - 1, HIGH(word)); Write(regSP() -= 2, LOW(word)); }

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


