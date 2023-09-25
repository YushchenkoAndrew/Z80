#pragma once
#include "Memory.h"


namespace Bus {

namespace Z80 {
  #define LOW_M(word) (word & 0x00FF)
  #define HIGH_M(word) (word & 0xFF00)
  #define BIT_M(pos, state) ((uint8_t)state << pos)

  #define LOW(word) ((uint8_t)(LOW_M(word)))
  #define HIGH(word) ((uint8_t)(HIGH_M(word) >> 8))
  #define BIT(word, pos) ((word >> pos) & 0x01)

  #define LOW_SET(word, byte) (HIGH_M(word) | LOW_M(byte))
  #define HIGH_SET(word, byte) (LOW_M(word) | HIGH_M((byte << 8)))
  #define BIT_SET(word, pos, state) ((word & ~BIT_M(pos, 1)) | BIT_M(pos, state))

  #define SIGN(byte) (byte & 0x80)

class CPU : public Window::Window, public Device {
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
  enum ModeT { NORMAL, DEBUG };

  CPU(Bus* b): bus(b) { Reset(); }
  ~CPU() {
    bExec = false; sync.second.notify_all();

    if (runtime != nullptr && runtime->joinable()) runtime->join();
  }

  inline void Reset() { regSP() = 0xFFFF; regPC() = 0x0000; }

  void Preinitialize() { 
    if (runtime == nullptr) runtime = std::make_unique<std::thread>(std::thread(&CPU::Runtime, this));
  }

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

      if (++index % 2) pos.x += vOffset.x + vStep.x * 6;
      else pos = olc::vi2d(absolute.x, pos.y + vStep.y);
    };

    auto DrawFlag = [&](std::string name, bool val) {
      GameEngine->DrawString(pos - olc::vi2d(vStep.x * (name.size() - 1) / 2, 0), name, ~AnyType<DARK_GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + olc::vi2d(0, vStep.y), val ? "1" : "0", ~AnyType<GREY, ColorT>::GetValue());

      pos.x += vStep.x * 3;
    };

    DrawReg("AF", regAF()); DrawReg("AF'", regALT(REG::AF));
    DrawReg("BC", regBC()); DrawReg("BC'", regALT(REG::BC));
    DrawReg("DE", regDE()); DrawReg("DE'", regALT(REG::DE));
    DrawReg("HL", regHL()); DrawReg("HL'", regALT(REG::HL));

    pos += olc::vi2d(0, vStep.y);
    DrawReg("SP", regSP()); DrawReg("IX'", regIX());
    DrawReg("PC", regPC()); DrawReg("IY'", regIY());
    DrawReg("I",  regI());  DrawReg("R",   regR());

    pos = olc::vi2d(absolute.x, pos.y + vStep.y);
    DrawFlag("C", flagC()); DrawFlag("N", flagN()); DrawFlag("PV", flagPV());
    DrawFlag("H", flagH()); DrawFlag("Z", flagZ()); DrawFlag("S", flagS());
  }

  DisassembleT Disassemble();

  
private:
  void Runtime() {
    while (bExec) {
      if (cycles > 0 && cycles--) continue;

      AnyType<-1, int32_t>::GetValue() = Read();
      foreach<Instructions, CPU>::Key2Process(this);
      std::this_thread::sleep_for(std::chrono::milliseconds(2));

      if (mode != DEBUG) continue;
      callback.second.notify_all();

      std::unique_lock<std::mutex> lock(sync.first);
      sync.second.wait(lock);
    }
  }

private:
  uint8_t Read();
  inline uint16_t Word() { return Read() | (Read() << 8); }
  inline uint16_t Word(uint32_t addr) { return Read(addr) | (Read(addr + 1) << 8); }

  uint8_t Read(uint32_t addr, bool mreq = true);
  uint8_t Write(uint32_t addr, uint8_t data, bool mreq = true);
  uint16_t Write(uint32_t addr, uint16_t data, bool mreq = true) {
    Write(addr + 1, HIGH(data), mreq); Write(addr, LOW(data), mreq);
    return data;
  }


public:
  template<int32_t T>
  inline void Process(Int2Type<T>) {
    // TODO:
    // error(peekPrev(), "Unknown operation");
    // return std::make_shared<Statement>();
  }

  template<int32_t T>
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<T>) {
    // TODO:
    // error(peekPrev(), "Unknown operation");
    // return std::make_shared<Statement>();
  }

  template<int32_t T>
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<T>) {
    // TODO:
    // error(peekPrev(), "Unknown operation");
    // return std::make_shared<Statement>();
  }

  template<int32_t T>
  inline void Process(Int2Type<Instruction::IX_INSTR>, Int2Type<T>) {
    // TODO:
    // error(peekPrev(), "Unknown operation");
    // return std::make_shared<Statement>();
  }

  template<int32_t T>
  inline void Process(Int2Type<Instruction::IY_INSTR>, Int2Type<T>) {
    // TODO:
    // error(peekPrev(), "Unknown operation");
    // return std::make_shared<Statement>();
  }


  inline void Process(Int2Type<Instruction::BIT_INSTR>) {
    AnyType<-1, int32_t>::GetValue() = Read();
    foreach<BitInstructions, CPU>::Key2Process(this, Int2Type<Instruction::BIT_INSTR>());
  }

  inline void Process(Int2Type<Instruction::MISC_INSTR>) {
    AnyType<-1, int32_t>::GetValue() = Read();
    foreach<MiscMInstructions, CPU>::Key2Process(this, Int2Type<Instruction::MISC_INSTR>());
  }

  inline void Process(Int2Type<Instruction::IX_INSTR>) {
    AnyType<-1, int32_t>::GetValue() = Read();
    foreach<IxInstructions, CPU>::Key2Process(this, Int2Type<Instruction::IX_INSTR>());
  }

  inline void Process(Int2Type<Instruction::IY_INSTR>) {
    AnyType<-1, int32_t>::GetValue() = Read();
    foreach<IyInstructions, CPU>::Key2Process(this, Int2Type<Instruction::IY_INSTR>());
  }


  inline void Process(Int2Type<Instruction::NOP>) { cycles = 4; }

  inline void Process(Int2Type<Instruction::IN_A_n>) { cycles = 11; regA(Read(HIGH_SET(Read(), regA()), false)); }
  inline void Process(Int2Type<Instruction::OUT_n_A>) { cycles = 11; Write(HIGH_SET(Read(), regA()), regA(), false); }


  inline void Process(Int2Type<Instruction::RST_00h>) { cycles = 11; Push(regPC()); regPC() = 0x00; }
  inline void Process(Int2Type<Instruction::RST_08h>) { cycles = 11; Push(regPC()); regPC() = 0x08; }
  inline void Process(Int2Type<Instruction::RST_10h>) { cycles = 11; Push(regPC()); regPC() = 0x10; }
  inline void Process(Int2Type<Instruction::RST_18h>) { cycles = 11; Push(regPC()); regPC() = 0x18; }
  inline void Process(Int2Type<Instruction::RST_20h>) { cycles = 11; Push(regPC()); regPC() = 0x20; }
  inline void Process(Int2Type<Instruction::RST_28h>) { cycles = 11; Push(regPC()); regPC() = 0x28; }
  inline void Process(Int2Type<Instruction::RST_30h>) { cycles = 11; Push(regPC()); regPC() = 0x30; }
  inline void Process(Int2Type<Instruction::RST_38h>) { cycles = 11; Push(regPC()); regPC() = 0x38; }


  inline void Process(Int2Type<Instruction::INC_BC>) { cycles = 6; regBC()++; }
  inline void Process(Int2Type<Instruction::INC_DE>) { cycles = 6; regDE()++; }
  inline void Process(Int2Type<Instruction::INC_HL>) { cycles = 6; regHL()++; }
  inline void Process(Int2Type<Instruction::INC_SP>) { cycles = 6; regSP()++; }

  inline void Process(Int2Type<Instruction::DEC_BC>) { cycles = 6; regBC()--; }
  inline void Process(Int2Type<Instruction::DEC_DE>) { cycles = 6; regDE()--; }
  inline void Process(Int2Type<Instruction::DEC_HL>) { cycles = 6; regHL()--; }
  inline void Process(Int2Type<Instruction::DEC_SP>) { cycles = 6; regSP()--; }

  inline void Process(Int2Type<Instruction::ADD_HL_BC>) { cycles = 11; regHL() = Add16(regHL(), regBC()); }
  inline void Process(Int2Type<Instruction::ADD_HL_DE>) { cycles = 11; regHL() = Add16(regHL(), regDE()); }
  inline void Process(Int2Type<Instruction::ADD_HL_HL>) { cycles = 11; regHL() = Add16(regHL(), regHL()); }
  inline void Process(Int2Type<Instruction::ADD_HL_SP>) { cycles = 11; regHL() = Add16(regHL(), regSP()); }

  inline void Process(Int2Type<Instruction::POP_BC>) { cycles = 10; regBC() = Pop(); }
  inline void Process(Int2Type<Instruction::POP_DE>) { cycles = 10; regDE() = Pop(); }
  inline void Process(Int2Type<Instruction::POP_HL>) { cycles = 10; regHL() = Pop(); }
  inline void Process(Int2Type<Instruction::POP_AF>) { cycles = 10; regAF() = Pop(); }

  inline void Process(Int2Type<Instruction::PUSH_BC>) { cycles = 11; Push(regBC()); }
  inline void Process(Int2Type<Instruction::PUSH_DE>) { cycles = 11; Push(regDE()); }
  inline void Process(Int2Type<Instruction::PUSH_HL>) { cycles = 11; Push(regHL()); }
  inline void Process(Int2Type<Instruction::PUSH_AF>) { cycles = 11; Push(regAF()); }

  inline void Process(Int2Type<Instruction::LD_BC_NN>) { cycles = 10; regBC() = Word(); }
  inline void Process(Int2Type<Instruction::LD_DE_NN>) { cycles = 10; regDE() = Word(); }
  inline void Process(Int2Type<Instruction::LD_HL_NN>) { cycles = 10; regHL() = Word(); }
  inline void Process(Int2Type<Instruction::LD_SP_NN>) { cycles = 10; regSP() = Word(); }

  inline void Process(Int2Type<Instruction::EX_AF_AF>) { cycles = 4; Ex16(regAF(), regALT(REG::AF)); }
  inline void Process(Int2Type<Instruction::EX_DE_HL>) { cycles = 4; Ex16(regDE(), regHL()); }
  inline void Process(Int2Type<Instruction::EX_sp_HL>) { cycles = 19; uint16_t word = Word(regSP()); Write(regSP(), regHL()); regHL() = word; }
  inline void Process(Int2Type<Instruction::EXX>) { cycles = 4; Ex16(regBC(), regALT(REG::BC)); Ex16(regDE(), regALT(REG::DE)); Ex16(regHL(), regALT(REG::HL)); }

  inline void Process(Int2Type<Instruction::LD_bc_A>) { cycles = 7; Write(regBC(), regA()); }
  inline void Process(Int2Type<Instruction::LD_de_A>) { cycles = 7; Write(regDE(), regA()); }
  inline void Process(Int2Type<Instruction::LD_nn_A>) { cycles = 13;Write(Word(),  regA()); }

  inline void Process(Int2Type<Instruction::LD_A_bc>) { cycles = 7; regA(Read(regBC())); }
  inline void Process(Int2Type<Instruction::LD_A_de>) { cycles = 7; regA(Read(regDE())); }
  inline void Process(Int2Type<Instruction::LD_A_nn>) { cycles = 13;regA(Read(Word())); }

  inline void Process(Int2Type<Instruction::LD_SP_HL>) { cycles = 6;  regSP() = +regHL(); }
  inline void Process(Int2Type<Instruction::LD_HL_nn>) { cycles = 16; regHL() = Word(Word()); }
  inline void Process(Int2Type<Instruction::LD_nn_HL>) { cycles = 16; Write(Word(), regHL()); }




  inline void Process(Int2Type<Instruction::CALL_NN>)    { cycles = 17; Call(true, Word()); }
  inline void Process(Int2Type<Instruction::CALL_NZ_NN>) { cycles = Call(!flagZ(), Word()) ? 17 : 10; }
  inline void Process(Int2Type<Instruction::CALL_Z_NN>)  { cycles = Call(flagZ(),  Word()) ? 17 : 10; }
  inline void Process(Int2Type<Instruction::CALL_NC_NN>) { cycles = Call(!flagC(), Word()) ? 17 : 10; }
  inline void Process(Int2Type<Instruction::CALL_C_NN>)  { cycles = Call(flagC(),  Word()) ? 17 : 10; }
  inline void Process(Int2Type<Instruction::CALL_PO_NN>) { cycles = Call(!flagPV(),Word()) ? 17 : 10; }
  inline void Process(Int2Type<Instruction::CALL_PE_NN>) { cycles = Call(flagPV(), Word()) ? 17 : 10; }
  inline void Process(Int2Type<Instruction::CALL_P_NN>)  { cycles = Call(!flagS(), Word()) ? 17 : 10; }
  inline void Process(Int2Type<Instruction::CALL_M_NN>)  { cycles = Call(flagS(),  Word()) ? 17 : 10; }

  inline void Process(Int2Type<Instruction::RET>)    { cycles = 10; Ret(true); }
  inline void Process(Int2Type<Instruction::RET_NZ>) { cycles = Ret(!flagZ()) ? 11 : 5; }
  inline void Process(Int2Type<Instruction::RET_Z>)  { cycles = Ret(flagZ())  ? 11 : 5; }
  inline void Process(Int2Type<Instruction::RET_NC>) { cycles = Ret(!flagC()) ? 11 : 5; }
  inline void Process(Int2Type<Instruction::RET_C>)  { cycles = Ret(flagC())  ? 11 : 5; }
  inline void Process(Int2Type<Instruction::RET_PO>) { cycles = Ret(!flagPV())? 11 : 5; }
  inline void Process(Int2Type<Instruction::RET_PE>) { cycles = Ret(flagPV()) ? 11 : 5; }
  inline void Process(Int2Type<Instruction::RET_P>)  { cycles = Ret(!flagS()) ? 11 : 5; }
  inline void Process(Int2Type<Instruction::RET_M>)  { cycles = Ret(flagS())  ? 11 : 5; }

  inline void Process(Int2Type<Instruction::JP_NN>)    { cycles = 10; Jump(true,     Word()); }
  inline void Process(Int2Type<Instruction::JP_NZ_NN>) { cycles = 10; Jump(!flagZ(), Word()); }
  inline void Process(Int2Type<Instruction::JP_Z_NN>)  { cycles = 10; Jump(flagZ(),  Word()); }
  inline void Process(Int2Type<Instruction::JP_NC_NN>) { cycles = 10; Jump(!flagC(), Word()); }
  inline void Process(Int2Type<Instruction::JP_C_NN>)  { cycles = 10; Jump(flagC(),  Word()); }
  inline void Process(Int2Type<Instruction::JP_PO_NN>) { cycles = 10; Jump(!flagPV(),Word()); }
  inline void Process(Int2Type<Instruction::JP_PE_NN>) { cycles = 10; Jump(flagPV(), Word()); }
  inline void Process(Int2Type<Instruction::JP_P_NN>)  { cycles = 10; Jump(!flagS(), Word()); }
  inline void Process(Int2Type<Instruction::JP_M_NN>)  { cycles = 10; Jump(flagS(),  Word()); }
  inline void Process(Int2Type<Instruction::JP_hl>)    { cycles = 4; Jump(true,      regHL()); }
  

  inline void Process(Int2Type<Instruction::INC_A>) { cycles = 4; regA(Inc8(regA())); }
  inline void Process(Int2Type<Instruction::INC_B>) { cycles = 4; regB(Inc8(regB())); }
  inline void Process(Int2Type<Instruction::INC_C>) { cycles = 4; regC(Inc8(regC())); }
  inline void Process(Int2Type<Instruction::INC_D>) { cycles = 4; regD(Inc8(regD())); }
  inline void Process(Int2Type<Instruction::INC_E>) { cycles = 4; regE(Inc8(regE())); }
  inline void Process(Int2Type<Instruction::INC_H>) { cycles = 4; regH(Inc8(regH())); }
  inline void Process(Int2Type<Instruction::INC_L>) { cycles = 4; regL(Inc8(regL())); }
  inline void Process(Int2Type<Instruction::INC_hl>) { cycles = 11; Write(regHL(), Inc8(Read(regHL()))); }

  inline void Process(Int2Type<Instruction::DEC_A>) { cycles = 4; regA(Dec8(regA())); }
  inline void Process(Int2Type<Instruction::DEC_B>) { cycles = 4; regB(Dec8(regB())); }
  inline void Process(Int2Type<Instruction::DEC_C>) { cycles = 4; regC(Dec8(regC())); }
  inline void Process(Int2Type<Instruction::DEC_D>) { cycles = 4; regD(Dec8(regD())); }
  inline void Process(Int2Type<Instruction::DEC_E>) { cycles = 4; regE(Dec8(regE())); }
  inline void Process(Int2Type<Instruction::DEC_H>) { cycles = 4; regH(Dec8(regH())); }
  inline void Process(Int2Type<Instruction::DEC_L>) { cycles = 4; regL(Dec8(regL())); }
  inline void Process(Int2Type<Instruction::DEC_hl>) { cycles = 11; Write(regHL(), Dec8(Read(regHL()))); }


  inline void Process(Int2Type<Instruction::ADC_A_A>) { cycles = 4; regA(Add8(regA(), regA(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_B>) { cycles = 4; regA(Add8(regA(), regB(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_C>) { cycles = 4; regA(Add8(regA(), regC(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_D>) { cycles = 4; regA(Add8(regA(), regD(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_E>) { cycles = 4; regA(Add8(regA(), regE(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_H>) { cycles = 4; regA(Add8(regA(), regH(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_L>) { cycles = 4; regA(Add8(regA(), regL(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_N>) { cycles = 7; regA(Add8(regA(), Read(), true)); }
  inline void Process(Int2Type<Instruction::ADC_A_hl>) { cycles = 7; regA(Add8(regA(), Read(regHL()), true)); }

  inline void Process(Int2Type<Instruction::SBC_A_A>) { cycles = 4; regA(Sub8(regA(), regA(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_B>) { cycles = 4; regA(Sub8(regA(), regB(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_C>) { cycles = 4; regA(Sub8(regA(), regC(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_D>) { cycles = 4; regA(Sub8(regA(), regD(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_E>) { cycles = 4; regA(Sub8(regA(), regE(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_H>) { cycles = 4; regA(Sub8(regA(), regH(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_L>) { cycles = 4; regA(Sub8(regA(), regL(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_N>) { cycles = 7; regA(Sub8(regA(), Read(), true)); }
  inline void Process(Int2Type<Instruction::SBC_A_hl>) { cycles = 7; regA(Sub8(regA(), Read(regHL()), true)); }

  inline void Process(Int2Type<Instruction::ADD_A_A>) { cycles = 4; regA(Add8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::ADD_A_B>) { cycles = 4; regA(Add8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::ADD_A_C>) { cycles = 4; regA(Add8(regA(), regC())); }
  inline void Process(Int2Type<Instruction::ADD_A_D>) { cycles = 4; regA(Add8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::ADD_A_E>) { cycles = 4; regA(Add8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::ADD_A_H>) { cycles = 4; regA(Add8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::ADD_A_L>) { cycles = 4; regA(Add8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::ADD_A_N>) { cycles = 7; regA(Add8(regA(), Read())); }
  inline void Process(Int2Type<Instruction::ADD_A_hl>) { cycles = 7; regA(Add8(regA(), Read(regHL()))); }

  inline void Process(Int2Type<Instruction::SUB_A>) { cycles = 4; regA(Sub8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::SUB_B>) { cycles = 4; regA(Sub8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::SUB_C>) { cycles = 4; regA(Sub8(regA(), regC())); }
  inline void Process(Int2Type<Instruction::SUB_D>) { cycles = 4; regA(Sub8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::SUB_E>) { cycles = 4; regA(Sub8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::SUB_H>) { cycles = 4; regA(Sub8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::SUB_L>) { cycles = 4; regA(Sub8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::SUB_N>) { cycles = 7; regA(Sub8(regA(), Read())); }
  inline void Process(Int2Type<Instruction::SUB_hl>) { cycles = 7; regA(Sub8(regA(), Read(regHL()))); }

  inline void Process(Int2Type<Instruction::AND_A>) { cycles = 4; regA(And8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::AND_B>) { cycles = 4; regA(And8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::AND_C>) { cycles = 4; regA(And8(regA(), regC())); }
  inline void Process(Int2Type<Instruction::AND_D>) { cycles = 4; regA(And8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::AND_E>) { cycles = 4; regA(And8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::AND_H>) { cycles = 4; regA(And8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::AND_L>) { cycles = 4; regA(And8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::AND_N>) { cycles = 7; regA(And8(regA(), Read())); }
  inline void Process(Int2Type<Instruction::AND_hl>) { cycles = 7; regA(And8(regA(), Read(regHL()))); }

  inline void Process(Int2Type<Instruction::OR_A>) { cycles = 4; regA(Or8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::OR_B>) { cycles = 4; regA(Or8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::OR_C>) { cycles = 4; regA(Or8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::OR_D>) { cycles = 4; regA(Or8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::OR_E>) { cycles = 4; regA(Or8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::OR_H>) { cycles = 4; regA(Or8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::OR_L>) { cycles = 4; regA(Or8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::OR_N>) { cycles = 7; regA(Or8(regA(), Read())); }
  inline void Process(Int2Type<Instruction::OR_hl>) { cycles = 7; regA(Or8(regA(), Read(regHL()))); }

  inline void Process(Int2Type<Instruction::XOR_A>) { cycles = 4; regA(Xor8(regA(), regA())); }
  inline void Process(Int2Type<Instruction::XOR_B>) { cycles = 4; regA(Xor8(regA(), regB())); }
  inline void Process(Int2Type<Instruction::XOR_C>) { cycles = 4; regA(Xor8(regA(), regC())); }
  inline void Process(Int2Type<Instruction::XOR_D>) { cycles = 4; regA(Xor8(regA(), regD())); }
  inline void Process(Int2Type<Instruction::XOR_E>) { cycles = 4; regA(Xor8(regA(), regE())); }
  inline void Process(Int2Type<Instruction::XOR_H>) { cycles = 4; regA(Xor8(regA(), regH())); }
  inline void Process(Int2Type<Instruction::XOR_L>) { cycles = 4; regA(Xor8(regA(), regL())); }
  inline void Process(Int2Type<Instruction::XOR_N>) { cycles = 7; regA(Xor8(regA(), Read())); }
  inline void Process(Int2Type<Instruction::XOR_hl>) { cycles = 7; regA(Xor8(regA(), Read(regHL()))); }

  inline void Process(Int2Type<Instruction::CP_A>) { cycles = 4; Sub8(regA(), regA()); }
  inline void Process(Int2Type<Instruction::CP_B>) { cycles = 4; Sub8(regA(), regB()); }
  inline void Process(Int2Type<Instruction::CP_C>) { cycles = 4; Sub8(regA(), regC()); }
  inline void Process(Int2Type<Instruction::CP_D>) { cycles = 4; Sub8(regA(), regD()); }
  inline void Process(Int2Type<Instruction::CP_E>) { cycles = 4; Sub8(regA(), regE()); }
  inline void Process(Int2Type<Instruction::CP_H>) { cycles = 4; Sub8(regA(), regH()); }
  inline void Process(Int2Type<Instruction::CP_L>) { cycles = 4; Sub8(regA(), regL()); }
  inline void Process(Int2Type<Instruction::CP_N>) { cycles = 7; Sub8(regA(), Read()); }
  inline void Process(Int2Type<Instruction::CP_hl>) { cycles = 7; Sub8(regA(), Read(regHL())); }


  inline void Process(Int2Type<Instruction::LD_A_A>) { cycles = 4; regA(regA()); }
  inline void Process(Int2Type<Instruction::LD_A_B>) { cycles = 4; regA(regB()); }
  inline void Process(Int2Type<Instruction::LD_A_C>) { cycles = 4; regA(regC()); }
  inline void Process(Int2Type<Instruction::LD_A_D>) { cycles = 4; regA(regD()); }
  inline void Process(Int2Type<Instruction::LD_A_E>) { cycles = 4; regA(regE()); }
  inline void Process(Int2Type<Instruction::LD_A_H>) { cycles = 4; regA(regH()); }
  inline void Process(Int2Type<Instruction::LD_A_L>) { cycles = 4; regA(regL()); }
  inline void Process(Int2Type<Instruction::LD_A_N>) { cycles = 7; regA(Read()); }
  inline void Process(Int2Type<Instruction::LD_A_hl>) { cycles = 7; regA(Read(regHL())); }

  inline void Process(Int2Type<Instruction::LD_B_A>) { cycles = 4; regB(regA()); }
  inline void Process(Int2Type<Instruction::LD_B_B>) { cycles = 4; regB(regB()); }
  inline void Process(Int2Type<Instruction::LD_B_C>) { cycles = 4; regB(regC()); }
  inline void Process(Int2Type<Instruction::LD_B_D>) { cycles = 4; regB(regD()); }
  inline void Process(Int2Type<Instruction::LD_B_E>) { cycles = 4; regB(regE()); }
  inline void Process(Int2Type<Instruction::LD_B_H>) { cycles = 4; regB(regH()); }
  inline void Process(Int2Type<Instruction::LD_B_L>) { cycles = 4; regB(regL()); }
  inline void Process(Int2Type<Instruction::LD_B_N>) { cycles = 7; regB(Read()); }
  inline void Process(Int2Type<Instruction::LD_B_hl>) { cycles = 7; regB(Read(regHL())); }

  inline void Process(Int2Type<Instruction::LD_C_A>) { cycles = 4; regC(regA()); }
  inline void Process(Int2Type<Instruction::LD_C_B>) { cycles = 4; regC(regB()); }
  inline void Process(Int2Type<Instruction::LD_C_C>) { cycles = 4; regC(regC()); }
  inline void Process(Int2Type<Instruction::LD_C_D>) { cycles = 4; regC(regD()); }
  inline void Process(Int2Type<Instruction::LD_C_E>) { cycles = 4; regC(regE()); }
  inline void Process(Int2Type<Instruction::LD_C_H>) { cycles = 4; regC(regH()); }
  inline void Process(Int2Type<Instruction::LD_C_L>) { cycles = 4; regC(regL()); }
  inline void Process(Int2Type<Instruction::LD_C_N>) { cycles = 7; regC(Read()); }
  inline void Process(Int2Type<Instruction::LD_C_hl>) { cycles = 7; regC(Read(regHL())); }

  inline void Process(Int2Type<Instruction::LD_D_A>) { cycles = 4; regD(regA()); }
  inline void Process(Int2Type<Instruction::LD_D_B>) { cycles = 4; regD(regB()); }
  inline void Process(Int2Type<Instruction::LD_D_C>) { cycles = 4; regD(regC()); }
  inline void Process(Int2Type<Instruction::LD_D_D>) { cycles = 4; regD(regD()); }
  inline void Process(Int2Type<Instruction::LD_D_E>) { cycles = 4; regD(regE()); }
  inline void Process(Int2Type<Instruction::LD_D_H>) { cycles = 4; regD(regH()); }
  inline void Process(Int2Type<Instruction::LD_D_L>) { cycles = 4; regD(regL()); }
  inline void Process(Int2Type<Instruction::LD_D_N>) { cycles = 7; regD(Read()); }
  inline void Process(Int2Type<Instruction::LD_D_hl>) { cycles = 7; regD(Read(regHL())); }

  inline void Process(Int2Type<Instruction::LD_E_A>) { cycles = 4; regE(regA()); }
  inline void Process(Int2Type<Instruction::LD_E_B>) { cycles = 4; regE(regB()); }
  inline void Process(Int2Type<Instruction::LD_E_C>) { cycles = 4; regE(regC()); }
  inline void Process(Int2Type<Instruction::LD_E_D>) { cycles = 4; regE(regD()); }
  inline void Process(Int2Type<Instruction::LD_E_E>) { cycles = 4; regE(regE()); }
  inline void Process(Int2Type<Instruction::LD_E_H>) { cycles = 4; regE(regH()); }
  inline void Process(Int2Type<Instruction::LD_E_L>) { cycles = 4; regE(regL()); }
  inline void Process(Int2Type<Instruction::LD_E_N>) { cycles = 7; regE(Read()); }
  inline void Process(Int2Type<Instruction::LD_E_hl>) { cycles = 7; regE(Read(regHL())); }

  inline void Process(Int2Type<Instruction::LD_H_A>) { cycles = 4; regH(regA()); }
  inline void Process(Int2Type<Instruction::LD_H_B>) { cycles = 4; regH(regB()); }
  inline void Process(Int2Type<Instruction::LD_H_C>) { cycles = 4; regH(regC()); }
  inline void Process(Int2Type<Instruction::LD_H_D>) { cycles = 4; regH(regD()); }
  inline void Process(Int2Type<Instruction::LD_H_E>) { cycles = 4; regH(regE()); }
  inline void Process(Int2Type<Instruction::LD_H_H>) { cycles = 4; regH(regH()); }
  inline void Process(Int2Type<Instruction::LD_H_L>) { cycles = 4; regH(regL()); }
  inline void Process(Int2Type<Instruction::LD_H_N>) { cycles = 7; regH(Read()); }
  inline void Process(Int2Type<Instruction::LD_H_hl>) { cycles = 7; regH(Read(regHL())); }

  inline void Process(Int2Type<Instruction::LD_L_A>) { cycles = 4; regL(regA()); }
  inline void Process(Int2Type<Instruction::LD_L_B>) { cycles = 4; regL(regB()); }
  inline void Process(Int2Type<Instruction::LD_L_C>) { cycles = 4; regL(regC()); }
  inline void Process(Int2Type<Instruction::LD_L_D>) { cycles = 4; regL(regD()); }
  inline void Process(Int2Type<Instruction::LD_L_E>) { cycles = 4; regL(regE()); }
  inline void Process(Int2Type<Instruction::LD_L_H>) { cycles = 4; regL(regH()); }
  inline void Process(Int2Type<Instruction::LD_L_L>) { cycles = 4; regL(regL()); }
  inline void Process(Int2Type<Instruction::LD_L_N>) { cycles = 7; regL(Read()); }
  inline void Process(Int2Type<Instruction::LD_L_hl>) { cycles = 7; regL(Read(regHL())); }

  inline void Process(Int2Type<Instruction::LD_hl_A>) { cycles = 7; Write(regHL(), regA()); }
  inline void Process(Int2Type<Instruction::LD_hl_B>) { cycles = 7; Write(regHL(), regB()); }
  inline void Process(Int2Type<Instruction::LD_hl_C>) { cycles = 7; Write(regHL(), regC()); }
  inline void Process(Int2Type<Instruction::LD_hl_D>) { cycles = 7; Write(regHL(), regD()); }
  inline void Process(Int2Type<Instruction::LD_hl_E>) { cycles = 7; Write(regHL(), regE()); }
  inline void Process(Int2Type<Instruction::LD_hl_H>) { cycles = 7; Write(regHL(), regH()); }
  inline void Process(Int2Type<Instruction::LD_hl_L>) { cycles = 7; Write(regHL(), regL()); }
  inline void Process(Int2Type<Instruction::LD_hl_n>) { cycles = 7; Write(regHL(), Read()); }

  inline void Process(Int2Type<Instruction::RLCA>) { cycles = 4; auto flags = regF(); regA(Rlc8(regA())); regF(BIT_SET(flags, FLAG::C, flagC()))->flagH(false)->flagN(false); }
  inline void Process(Int2Type<Instruction::RRCA>) { cycles = 4; auto flags = regF(); regA(Rrc8(regA())); regF(BIT_SET(flags, FLAG::C, flagC()))->flagH(false)->flagN(false); }

  inline void Process(Int2Type<Instruction::RLA>) { cycles = 4; auto flags = regF(); regA(Rlc8(regA(), true)); regF(BIT_SET(flags, FLAG::C, flagC()))->flagH(false)->flagN(false); }
  inline void Process(Int2Type<Instruction::RRA>) { cycles = 4; auto flags = regF(); regA(Rrc8(regA(), true)); regF(BIT_SET(flags, FLAG::C, flagC()))->flagH(false)->flagN(false); }

  inline void Process(Int2Type<Instruction::CPL>) { cycles = 4; regA(regA() ^ 0xFF)->flagN(true)->flagH(true); }
  inline void Process(Int2Type<Instruction::SCF>) { cycles = 4; flagC(true)->flagN(false)->flagH(false); }
  inline void Process(Int2Type<Instruction::CCF>) { cycles = 4; flagH(flagC())->flagC(!flagC())->flagN(false); }

  // TODO:
  // NOTE: Do the last one, maybe need to fix bug with asm compiler
  inline void Process(Int2Type<Instruction::DJNZ_D>) {}
  inline void Process(Int2Type<Instruction::JR_NZ_D>) {}
  inline void Process(Int2Type<Instruction::JR_NC_D>) {}
  inline void Process(Int2Type<Instruction::JR_D>) {}
  inline void Process(Int2Type<Instruction::JR_Z_D>) {}
  inline void Process(Int2Type<Instruction::JR_C_D>) {}


  inline void Process(Int2Type<Instruction::DI>) {}
  inline void Process(Int2Type<Instruction::EI>) {}
  inline void Process(Int2Type<Instruction::HALT>) {}

  inline void Process(Int2Type<Instruction::DAA>) {}


  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_A>) { cycles = 8; Bit8(regA(), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_A>) { cycles = 8; Bit8(regA(), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_A>) { cycles = 8; Bit8(regA(), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_A>) { cycles = 8; Bit8(regA(), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_A>) { cycles = 8; Bit8(regA(), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_A>) { cycles = 8; Bit8(regA(), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_A>) { cycles = 8; Bit8(regA(), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_A>) { cycles = 8; Bit8(regA(), 7); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_B>) { cycles = 8; Bit8(regB(), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_B>) { cycles = 8; Bit8(regB(), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_B>) { cycles = 8; Bit8(regB(), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_B>) { cycles = 8; Bit8(regB(), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_B>) { cycles = 8; Bit8(regB(), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_B>) { cycles = 8; Bit8(regB(), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_B>) { cycles = 8; Bit8(regB(), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_B>) { cycles = 8; Bit8(regB(), 7); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_C>) { cycles = 8; Bit8(regC(), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_C>) { cycles = 8; Bit8(regC(), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_C>) { cycles = 8; Bit8(regC(), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_C>) { cycles = 8; Bit8(regC(), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_C>) { cycles = 8; Bit8(regC(), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_C>) { cycles = 8; Bit8(regC(), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_C>) { cycles = 8; Bit8(regC(), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_C>) { cycles = 8; Bit8(regC(), 7); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_D>) { cycles = 8; Bit8(regD(), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_D>) { cycles = 8; Bit8(regD(), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_D>) { cycles = 8; Bit8(regD(), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_D>) { cycles = 8; Bit8(regD(), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_D>) { cycles = 8; Bit8(regD(), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_D>) { cycles = 8; Bit8(regD(), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_D>) { cycles = 8; Bit8(regD(), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_D>) { cycles = 8; Bit8(regD(), 7); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_E>) { cycles = 8; Bit8(regE(), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_E>) { cycles = 8; Bit8(regE(), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_E>) { cycles = 8; Bit8(regE(), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_E>) { cycles = 8; Bit8(regE(), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_E>) { cycles = 8; Bit8(regE(), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_E>) { cycles = 8; Bit8(regE(), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_E>) { cycles = 8; Bit8(regE(), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_E>) { cycles = 8; Bit8(regE(), 7); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_H>) { cycles = 8; Bit8(regH(), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_H>) { cycles = 8; Bit8(regH(), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_H>) { cycles = 8; Bit8(regH(), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_H>) { cycles = 8; Bit8(regH(), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_H>) { cycles = 8; Bit8(regH(), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_H>) { cycles = 8; Bit8(regH(), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_H>) { cycles = 8; Bit8(regH(), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_H>) { cycles = 8; Bit8(regH(), 7); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_L>) { cycles = 8; Bit8(regL(), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_L>) { cycles = 8; Bit8(regL(), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_L>) { cycles = 8; Bit8(regL(), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_L>) { cycles = 8; Bit8(regL(), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_L>) { cycles = 8; Bit8(regL(), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_L>) { cycles = 8; Bit8(regL(), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_L>) { cycles = 8; Bit8(regL(), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_L>) { cycles = 8; Bit8(regL(), 7); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_0_hl>) { cycles = 15; Bit8(Read(regHL()), 0); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_1_hl>) { cycles = 15; Bit8(Read(regHL()), 1); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_2_hl>) { cycles = 15; Bit8(Read(regHL()), 2); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_3_hl>) { cycles = 15; Bit8(Read(regHL()), 3); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_4_hl>) { cycles = 15; Bit8(Read(regHL()), 4); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_5_hl>) { cycles = 15; Bit8(Read(regHL()), 5); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_6_hl>) { cycles = 15; Bit8(Read(regHL()), 6); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::BIT_7_hl>) { cycles = 15; Bit8(Read(regHL()), 7); }



  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_A>) { cycles = 8; regA(Res8(regA(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_A>) { cycles = 8; regA(Res8(regA(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_A>) { cycles = 8; regA(Res8(regA(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_A>) { cycles = 8; regA(Res8(regA(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_A>) { cycles = 8; regA(Res8(regA(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_A>) { cycles = 8; regA(Res8(regA(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_A>) { cycles = 8; regA(Res8(regA(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_A>) { cycles = 8; regA(Res8(regA(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_B>) { cycles = 8; regB(Res8(regB(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_B>) { cycles = 8; regB(Res8(regB(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_B>) { cycles = 8; regB(Res8(regB(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_B>) { cycles = 8; regB(Res8(regB(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_B>) { cycles = 8; regB(Res8(regB(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_B>) { cycles = 8; regB(Res8(regB(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_B>) { cycles = 8; regB(Res8(regB(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_B>) { cycles = 8; regB(Res8(regB(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_C>) { cycles = 8; regC(Res8(regC(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_C>) { cycles = 8; regC(Res8(regC(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_C>) { cycles = 8; regC(Res8(regC(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_C>) { cycles = 8; regC(Res8(regC(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_C>) { cycles = 8; regC(Res8(regC(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_C>) { cycles = 8; regC(Res8(regC(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_C>) { cycles = 8; regC(Res8(regC(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_C>) { cycles = 8; regC(Res8(regC(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_D>) { cycles = 8; regD(Res8(regD(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_D>) { cycles = 8; regD(Res8(regD(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_D>) { cycles = 8; regD(Res8(regD(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_D>) { cycles = 8; regD(Res8(regD(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_D>) { cycles = 8; regD(Res8(regD(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_D>) { cycles = 8; regD(Res8(regD(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_D>) { cycles = 8; regD(Res8(regD(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_D>) { cycles = 8; regD(Res8(regD(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_E>) { cycles = 8; regE(Res8(regE(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_E>) { cycles = 8; regE(Res8(regE(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_E>) { cycles = 8; regE(Res8(regE(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_E>) { cycles = 8; regE(Res8(regE(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_E>) { cycles = 8; regE(Res8(regE(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_E>) { cycles = 8; regE(Res8(regE(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_E>) { cycles = 8; regE(Res8(regE(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_E>) { cycles = 8; regE(Res8(regE(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_H>) { cycles = 8; regH(Res8(regH(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_H>) { cycles = 8; regH(Res8(regH(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_H>) { cycles = 8; regH(Res8(regH(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_H>) { cycles = 8; regH(Res8(regH(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_H>) { cycles = 8; regH(Res8(regH(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_H>) { cycles = 8; regH(Res8(regH(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_H>) { cycles = 8; regH(Res8(regH(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_H>) { cycles = 8; regH(Res8(regH(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_L>) { cycles = 8; regL(Res8(regL(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_L>) { cycles = 8; regL(Res8(regL(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_L>) { cycles = 8; regL(Res8(regL(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_L>) { cycles = 8; regL(Res8(regL(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_L>) { cycles = 8; regL(Res8(regL(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_L>) { cycles = 8; regL(Res8(regL(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_L>) { cycles = 8; regL(Res8(regL(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_L>) { cycles = 8; regL(Res8(regL(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_0_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_1_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_2_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_3_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_4_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_5_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_6_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RES_7_hl>) { cycles = 15; Write(regHL(), Res8(Read(regHL()), 7)); }



  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_A>) { cycles = 8; regA(Set8(regA(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_A>) { cycles = 8; regA(Set8(regA(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_A>) { cycles = 8; regA(Set8(regA(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_A>) { cycles = 8; regA(Set8(regA(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_A>) { cycles = 8; regA(Set8(regA(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_A>) { cycles = 8; regA(Set8(regA(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_A>) { cycles = 8; regA(Set8(regA(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_A>) { cycles = 8; regA(Set8(regA(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_B>) { cycles = 8; regB(Set8(regB(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_B>) { cycles = 8; regB(Set8(regB(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_B>) { cycles = 8; regB(Set8(regB(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_B>) { cycles = 8; regB(Set8(regB(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_B>) { cycles = 8; regB(Set8(regB(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_B>) { cycles = 8; regB(Set8(regB(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_B>) { cycles = 8; regB(Set8(regB(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_B>) { cycles = 8; regB(Set8(regB(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_C>) { cycles = 8; regC(Set8(regC(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_C>) { cycles = 8; regC(Set8(regC(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_C>) { cycles = 8; regC(Set8(regC(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_C>) { cycles = 8; regC(Set8(regC(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_C>) { cycles = 8; regC(Set8(regC(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_C>) { cycles = 8; regC(Set8(regC(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_C>) { cycles = 8; regC(Set8(regC(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_C>) { cycles = 8; regC(Set8(regC(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_D>) { cycles = 8; regD(Set8(regD(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_D>) { cycles = 8; regD(Set8(regD(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_D>) { cycles = 8; regD(Set8(regD(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_D>) { cycles = 8; regD(Set8(regD(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_D>) { cycles = 8; regD(Set8(regD(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_D>) { cycles = 8; regD(Set8(regD(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_D>) { cycles = 8; regD(Set8(regD(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_D>) { cycles = 8; regD(Set8(regD(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_E>) { cycles = 8; regE(Set8(regE(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_E>) { cycles = 8; regE(Set8(regE(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_E>) { cycles = 8; regE(Set8(regE(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_E>) { cycles = 8; regE(Set8(regE(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_E>) { cycles = 8; regE(Set8(regE(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_E>) { cycles = 8; regE(Set8(regE(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_E>) { cycles = 8; regE(Set8(regE(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_E>) { cycles = 8; regE(Set8(regE(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_H>) { cycles = 8; regH(Set8(regH(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_H>) { cycles = 8; regH(Set8(regH(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_H>) { cycles = 8; regH(Set8(regH(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_H>) { cycles = 8; regH(Set8(regH(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_H>) { cycles = 8; regH(Set8(regH(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_H>) { cycles = 8; regH(Set8(regH(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_H>) { cycles = 8; regH(Set8(regH(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_H>) { cycles = 8; regH(Set8(regH(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_L>) { cycles = 8; regL(Set8(regL(), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_L>) { cycles = 8; regL(Set8(regL(), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_L>) { cycles = 8; regL(Set8(regL(), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_L>) { cycles = 8; regL(Set8(regL(), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_L>) { cycles = 8; regL(Set8(regL(), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_L>) { cycles = 8; regL(Set8(regL(), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_L>) { cycles = 8; regL(Set8(regL(), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_L>) { cycles = 8; regL(Set8(regL(), 7)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_0_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 0)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_1_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 1)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_2_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 2)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_3_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 3)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_4_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 4)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_5_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 5)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_6_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 6)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SET_7_hl>) { cycles = 15; Write(regHL(), Set8(Read(regHL()), 7)); }

  
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_A>) { cycles = 8; regA(Rlc8(regA())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_B>) { cycles = 8; regB(Rlc8(regB())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_C>) { cycles = 8; regC(Rlc8(regC())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_D>) { cycles = 8; regD(Rlc8(regD())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_E>) { cycles = 8; regE(Rlc8(regE())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_H>) { cycles = 8; regH(Rlc8(regH())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_L>) { cycles = 8; regL(Rlc8(regL())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RLC_hl>) { cycles = 15; Write(regHL(), Rlc8(Read(regHL()))); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_A>) { cycles = 8; regA(Rlc8(regA(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_B>) { cycles = 8; regB(Rlc8(regB(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_C>) { cycles = 8; regC(Rlc8(regC(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_D>) { cycles = 8; regD(Rlc8(regD(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_E>) { cycles = 8; regE(Rlc8(regE(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_H>) { cycles = 8; regH(Rlc8(regH(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_L>) { cycles = 8; regL(Rlc8(regL(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RL_hl>) { cycles = 15; Write(regHL(), Rlc8(Read(regHL()), true)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_A>) { cycles = 8; regA(Sla8(regA())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_B>) { cycles = 8; regB(Sla8(regB())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_C>) { cycles = 8; regC(Sla8(regC())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_D>) { cycles = 8; regD(Sla8(regD())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_E>) { cycles = 8; regE(Sla8(regE())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_H>) { cycles = 8; regH(Sla8(regH())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_L>) { cycles = 8; regL(Sla8(regL())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLA_hl>) { cycles = 15; Write(regHL(), Sla8(Read(regHL()))); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_A>) { cycles = 8; regA(Sla8(regA())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_B>) { cycles = 8; regB(Sla8(regB())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_C>) { cycles = 8; regC(Sla8(regC())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_D>) { cycles = 8; regD(Sla8(regD())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_E>) { cycles = 8; regE(Sla8(regE())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_H>) { cycles = 8; regH(Sla8(regH())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_L>) { cycles = 8; regL(Sla8(regL())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SLL_hl>) { cycles = 15; Write(regHL(), Sla8(Read(regHL()))); }


  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_A>) { cycles = 8; regA(Rrc8(regA())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_B>) { cycles = 8; regB(Rrc8(regB())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_C>) { cycles = 8; regC(Rrc8(regC())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_D>) { cycles = 8; regD(Rrc8(regD())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_E>) { cycles = 8; regE(Rrc8(regE())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_H>) { cycles = 8; regH(Rrc8(regH())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_L>) { cycles = 8; regL(Rrc8(regL())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RRC_hl>) { cycles = 15; Write(regHL(), Rrc8(Read(regHL()))); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_A>) { cycles = 8; regA(Rrc8(regA(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_B>) { cycles = 8; regB(Rrc8(regB(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_C>) { cycles = 8; regC(Rrc8(regC(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_D>) { cycles = 8; regD(Rrc8(regD(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_E>) { cycles = 8; regE(Rrc8(regE(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_H>) { cycles = 8; regH(Rrc8(regH(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_L>) { cycles = 8; regL(Rrc8(regL(), true)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::RR_hl>) { cycles = 15; Write(regHL(), Rrc8(Read(regHL()), true)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_A>) { cycles = 8; regA(Sra8(regA(), 0x80)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_B>) { cycles = 8; regB(Sra8(regB(), 0x80)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_C>) { cycles = 8; regC(Sra8(regC(), 0x80)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_D>) { cycles = 8; regD(Sra8(regD(), 0x80)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_E>) { cycles = 8; regE(Sra8(regE(), 0x80)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_H>) { cycles = 8; regH(Sra8(regH(), 0x80)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_L>) { cycles = 8; regL(Sra8(regL(), 0x80)); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRA_hl>) { cycles = 15; Write(regHL(), Sra8(Read(regHL()), 0x80)); }

  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_A>) { cycles = 8; regA(Sra8(regA())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_B>) { cycles = 8; regB(Sra8(regB())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_C>) { cycles = 8; regC(Sra8(regC())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_D>) { cycles = 8; regD(Sra8(regD())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_E>) { cycles = 8; regE(Sra8(regE())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_H>) { cycles = 8; regH(Sra8(regH())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_L>) { cycles = 8; regL(Sra8(regL())); }
  inline void Process(Int2Type<Instruction::BIT_INSTR>, Int2Type<BitInstruction::SRL_hl>) { cycles = 15; Write(regHL(), Sra8(Read(regHL()))); }



  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IN_A_c>) { cycles = 12; regA(In8()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IN_B_c>) { cycles = 12; regB(In8()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IN_C_c>) { cycles = 12; regC(In8()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IN_D_c>) { cycles = 12; regD(In8()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IN_E_c>) { cycles = 12; regE(In8()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IN_H_c>) { cycles = 12; regH(In8()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IN_L_c>) { cycles = 12; regL(In8()); }

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUT_c_A>) { cycles = 12; Write(HIGH_SET(Read(), regB()), regA(), false); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUT_c_B>) { cycles = 12; Write(HIGH_SET(Read(), regB()), regB(), false); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUT_c_C>) { cycles = 12; Write(HIGH_SET(Read(), regB()), regC(), false); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUT_c_D>) { cycles = 12; Write(HIGH_SET(Read(), regB()), regD(), false); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUT_c_E>) { cycles = 12; Write(HIGH_SET(Read(), regB()), regE(), false); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUT_c_H>) { cycles = 12; Write(HIGH_SET(Read(), regB()), regH(), false); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUT_c_L>) { cycles = 12; Write(HIGH_SET(Read(), regB()), regL(), false); }


  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::ADC_HL_BC>) { cycles = 15; regHL() = Add16(regHL(), regBC(), true); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::ADC_HL_DE>) { cycles = 15; regHL() = Add16(regHL(), regDE(), true); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::ADC_HL_HL>) { cycles = 15; regHL() = Add16(regHL(), regHL(), true); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::ADC_HL_SP>) { cycles = 15; regHL() = Add16(regHL(), regSP(), true); }

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_BC_nn>) { cycles = 20; regBC() = Word(Word()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_DE_nn>) { cycles = 20; regDE() = Word(Word()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_SP_nn>) { cycles = 20; regSP() = Word(Word()); }

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_nn_DE>) { cycles = 20; Write(Word(), regDE()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_nn_BC>) { cycles = 20; Write(Word(), regBC()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_nn_SP>) { cycles = 20; Write(Word(), regSP()); }

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LDI>) { cycles = 16; Write(regDE(), Read(regHL())); regDE()++; regHL()++; regBC()--; flagH(false)->flagN(false)->flagPV(!regBC()); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LDIR>) {
    Process(Int2Type<Instruction::MISC_INSTR>(), Int2Type<MiscInstruction::LDI>());
    if (regBC() != 0) { cycles = 21; regPC() -= 2; }
  }

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::CPI>) { cycles = 16; auto carry = flagC(); Sub8(regA(), Read(regHL())); regHL()++; regBC()--; flagH(false)->flagN(false)->flagPV(!regBC())->flagC(carry); }
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::CPIR>) { 
    Process(Int2Type<Instruction::MISC_INSTR>(), Int2Type<MiscInstruction::CPI>());
    if (regBC() != 0 && regA() != Read(regHL())) { cycles = 21; regPC() -= 2; }
  }

  // TODO:
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_A_I>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_I_A>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_R_A>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LD_A_R>) {}


  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::SBC_HL_SP>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::SBC_HL_BC>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::SBC_HL_DE>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::SBC_HL_HL>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::NEG>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IM_1>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IM_0>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IM_2>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::INI>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::INIR>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::RETI>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::RETN>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LDD>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::LDDR>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::IND>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::INDR>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::RRD>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::RLD>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUTI>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OTIR>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::CPD>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::CPDR>) {}

  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OUTD>) {}
  inline void Process(Int2Type<Instruction::MISC_INSTR>, Int2Type<MiscInstruction::OTDR>) {}















  // TODO:

private:
  std::string sprintf(std::string str, int32_t byte);

  inline uint8_t Inc8(uint8_t a) {
    uint16_t acc = (uint16_t)a + 1;

    // P/V is set if a was 7Fh before operation; otherwise, it is reset
    flagPV(a == 0x7F)->flagN(false)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagH(((regA() & 0x0F) + 1) & 0xF0);
    return (uint8_t)(acc & 0xFF);
  }

  inline uint8_t Dec8(uint8_t a) {
    uint16_t acc = (uint16_t)a - 1;

    // P/V is set if a was 80h before operation; otherwise, it is reset.
    flagPV(a == 0x80)->flagN(true)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagH(((regA() & 0x0F) - 1) & 0xF0);
    return (uint8_t)(acc & 0xFF);
  }

  inline uint8_t Add8(uint8_t a, uint8_t b, bool carry = false) {
    uint16_t acc = (uint16_t)a + b + (carry && flagC());

    // When adding operands with similar signs and the result contains a different sign,
    // the Overflow Flag is set
    flagPV(SIGN(a) == SIGN(b) && SIGN(a) != SIGN(acc));

    flagN(false)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagC(acc & 0xFF00)->flagH(((a & 0x0F) + (b & 0x0F) + (carry && flagC())) & 0xF0);

    return (uint8_t)(acc & 0xFF);
  }

  inline uint16_t Add16(uint16_t a, uint16_t b, bool carry = false) {
    uint32_t acc = (uint32_t)a + b + (carry && flagC());

    flagN(false)->flagC(acc & 0xFF0000)->flagH(((a & 0x0FFF) + (b & 0x0FFF) + (carry && flagC())) & 0xF000);

    return (uint16_t)(acc & 0xFFFF);
  }

  inline uint8_t Sub8(uint8_t a, uint8_t b, bool carry = false) {
    uint16_t acc = (uint16_t)a + (b ^ 0xFF) + 1 + (carry && flagC()) * 0xFF;

    // When adding operands with similar signs and the result contains a different sign,
    // the Overflow Flag is set
    flagPV(SIGN(a) != SIGN(b) && SIGN(a) != SIGN(acc));

    flagN(true)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagC(!(acc & 0xFF00))->flagH(!(((a & 0x0F) + (((b ^ 0xFF) + 1) & 0x0F) + (carry && flagC()) * 0xFF) & 0xF0));

    return (uint8_t)(acc & 0xFF);
  }

  inline uint8_t Or8(uint8_t a, uint8_t b)  { return BitOperation(a, b, a | b); }
  inline uint8_t Xor8(uint8_t a, uint8_t b) { return BitOperation(a, b, a ^ b); }
  inline uint8_t And8(uint8_t a, uint8_t b) { auto res = BitOperation(a, b, a & b); flagH(true); return res; }
  inline uint8_t In8() { 
   auto acc = Read(HIGH_SET(regC(), regB()), false);

   flagS(SIGN(acc))->flagZ(!acc)->flagH(false)->flagN(false)->flagPV(IsParity(acc));

   return acc;
  }

  inline uint8_t BitOperation(uint8_t a, uint8_t b, uint8_t acc) {
    flagPV(IsParity(acc))->flagN(false)->flagS(SIGN(acc))->flagZ(!acc)->flagC(false)->flagH(false);
    return acc;
  }

  inline uint8_t Set8(uint8_t a, uint8_t offset) { return a | (0x01 << (offset & 0x07)); }
  inline uint8_t Res8(uint8_t a, uint8_t offset) { return a & ((0x01 << (offset & 0x07)) ^ 0xFF); }
  inline void Bit8(uint8_t a, uint8_t offset) { flagN(false)->flagZ(!(a & (0x01 << (offset & 0x07))))->flagH(true); }

  inline uint8_t Rlc8(uint8_t a, bool carry = false) {
    uint8_t acc = (a << 1) | (uint8_t)(carry ? flagC() : !!(a & 0x80));

    flagN(false)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagC(a & 0x80)->flagH(false)->flagPV(IsParity(acc));

    return acc;
  }

  inline uint8_t Sla8(uint8_t a, uint8_t mask = 0x00) {
    uint8_t acc = (a << 1) | (a & mask);

    flagN(false)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagC(a & 0x80)->flagH(false)->flagPV(IsParity(acc));

    return acc;
  }

  inline uint8_t Rrc8(uint8_t a, bool carry = false) {
    uint8_t acc = (a >> 1) | ((uint8_t)(carry ? flagC() : (a & 0x01)) << 7);

    flagN(false)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagC(a & 0x01)->flagH(false)->flagPV(IsParity(acc));

    return acc;
  }

  inline uint8_t Sra8(uint8_t a, uint8_t mask = 0x00) {
    uint8_t acc = (a >> 1) | (a & mask);

    flagN(false)->flagS(SIGN(acc))->flagZ(!(acc & 0xFF))->flagC(a & 0x01)->flagH(false)->flagPV(IsParity(acc));

    return acc;
  }


  inline uint16_t Pop() { regSP() += 2; return (Read(regSP() - 1) << 8) | Read(regSP() - 2); }
  inline void Push(uint16_t word) { Write(regSP() - 1, HIGH(word)); Write(regSP() -= 2, LOW(word)); }
  inline void Ex16(std::atomic<uint16_t>& a, std::atomic<uint16_t>& b) { uint16_t c = a; a = +b; b = c; }

  // inline void Process(Int2Type<Instruction::RET_P>)  { cycles = 5; if (!flagS()) { cycles = 11; regPC() = Pop(); } }
  inline bool Ret(bool flag) { if (flag) regPC() = Pop(); return flag; }
  inline bool Call(bool flag, uint16_t addr) { if (flag) { Push(regPC()); regPC() = addr; } return flag; }
  inline bool Jump(bool flag, uint16_t addr) { if (flag) regPC() = addr; return flag; }

  inline std::atomic<uint16_t>& regALT(REG offset) { return reg[REG::ALTERNATIVE + offset]; }

  inline std::atomic<uint16_t>& regAF() { return reg[REG::AF]; }
  inline std::atomic<uint16_t>& regBC() { return reg[REG::BC]; }
  inline std::atomic<uint16_t>& regDE() { return reg[REG::DE]; }
  inline std::atomic<uint16_t>& regHL() { return reg[REG::HL]; }
  inline std::atomic<uint16_t>& regIX() { return reg[REG::IX]; }
  inline std::atomic<uint16_t>& regIY() { return reg[REG::IY]; }
  inline std::atomic<uint16_t>& regSP() { return reg[REG::SP]; }
  inline std::atomic<uint16_t>& regPC() { return reg[REG::PC]; }

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

  /**
   * Returns true if one's is even
   */
  inline bool IsParity(uint8_t x) { x ^= x >> 4; x ^= x >> 2; x ^= x >> 1; return (~x) & 1; }

  inline std::string Int2Hex(int32_t i, int32_t width = 4) { 
    std::stringstream ss; ss << std::setfill('0') << std::setw(width) << std::hex << std::uppercase << +i;
    return ss.str();
  }

public:
  inline bool IsDebug() { return mode == DEBUG; }
  inline void Debug() { mode = DEBUG; }
  inline void Normal() { mode = NORMAL; sync.second.notify_all(); }

  inline void Step() { if (mode == DEBUG) sync.second.notify_all(); }
  inline uint16_t Addr() { 
    std::unique_lock<std::mutex> lock(callback.first);
    callback.second.wait(lock);

    return regPC();
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(24, 0);

  uint32_t cycles = 0; // Show how many clock cycles are required to run specific command
  std::atomic<ModeT> mode = NORMAL;

  std::pair<std::mutex, std::condition_variable> sync;
  std::pair<std::mutex, std::condition_variable> callback;

  std::atomic<bool> bExec = true;
  std::unique_ptr<std::thread> runtime = nullptr;

  std::array<std::atomic<uint16_t>, 13> reg;

  Bus* bus;
};

};
};


