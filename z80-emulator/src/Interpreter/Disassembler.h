#pragma once

#include "src/Bus/Defs.h"
#include "src/Interpreter/Lexer.h"
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace Interpreter {

class Disassembler {
  typedef std::pair<const char*, uint8_t> MetadataT;

public:
  Disassembler(std::vector<uint8_t>& memory): memory(memory) {}

  std::list<std::string> scan() {
    src = ""; index = 0;

    while (!isAtEnd()) {
      auto cmd = GetInstruction(advance());
      src += (cmd.second ? GetCode(cmd.first, cmd.second) : cmd.first) + "\n";
    }

    return lexer.scan(src);
  }

  std::string GetCode(std::string phrase, uint8_t mask) {
    for (int32_t cnt = mask & 0x0F, offset = 1; cnt != 1; offset++) {
      const int32_t bytes = (mask >> (4 * offset)) & 0x0F;
      int32_t word = 0x00; cnt -= bytes;
      for (int32_t j = 0; j < bytes; j++) {
        word = word | (advance() << (j * 8));
      }
      
      phrase = sprintf(phrase, word);
    }

    return phrase;
  }

  MetadataT GetInstruction(uint8_t opcode) {
    switch (opcode) {
      case Bus::Z80::MISC_INSTR: {
        auto opcode = advance();
        auto it = misc.find(opcode);
        if (it != misc.end()) return misc.at(opcode);
        return { sprintf("db 0xED, 0x%02x", opcode).c_str(), 0 };
      }

      case Bus::Z80::BIT_INSTR: {
        auto opcode = advance();
        auto it = bits.find(opcode);
        if (it != bits.end()) return bits.at(opcode);
        return { sprintf("db 0xCB, 0x%02x", opcode).c_str(), 0 };
      }

      case Bus::Z80::IX_INSTR: {
        auto opcode = advance();
        if (opcode == Bus::Z80::IxBitInstr) {
          auto it = ix_bits.find(advance());
          if (it != ix_bits.end()) return ix_bits.at(opcode);
          return { sprintf("db 0xDD, 0xCB, 0x%02x", opcode).c_str(), 0 };
        }

        auto it = ix.find(advance());
        if (it != ix.end()) return ix.at(opcode);
        return { sprintf("db 0xDD, 0x%02x", opcode).c_str(), 0 };
      }

      case Bus::Z80::IY_INSTR: {
        auto opcode = advance();
        if (opcode == Bus::Z80::IyBitInstr) {
          auto it = iy_bits.find(advance());
          if (it != iy_bits.end()) return iy_bits.at(opcode);
          return { sprintf("db 0xFD, 0xCB, 0x%02x", opcode).c_str(), 0 };
        }

        auto it = iy.find(advance());
        if (it != iy.end()) return iy.at(opcode);
        return { sprintf("db 0xFD, 0x%02x", opcode).c_str(), 0 };
      }
    };

    auto it = instructions.find(opcode);
    if (it != instructions.end()) return instructions.at(opcode);
    return { sprintf("db 0x%02x", opcode).c_str(), 0 };
  }


  std::string sprintf(std::string str, int32_t byte) {
    for (int32_t i = 0; i < str.size(); i++) {
      if (str[i] != '%' && i + 1 < str.size()) continue;
      std::stringstream ss;

      int32_t size = 2; int32_t startAt = i;

      if (str[i + 1] != 'd' || str[i + 1] != 'c' || str[i + 1] != 'x') {
        ss << std::setfill(str[++i]); size++;

        if (i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '9') {
          ss << std::setw(str[++i] - '0'); size++;
        }
      }

      switch (str[i + 1]) {
        case 'd': ss << byte; break;
        case 'c': ss << (char)byte; break;
        case 'x': ss << std::hex << std::uppercase << byte; break;
        default: return str;
      }
      
      str.replace(startAt, size, ss.str());
      return str;
    }

    return str;
  }

private:
  inline uint8_t advance() { return memory[index++]; }
  inline bool isAtEnd() { return index >= memory.size(); }

private:
  Lexer lexer;

  int32_t index = 0; 
  std::string src = "";
  std::vector<uint8_t>& memory;

private:
static inline const std::unordered_map<uint8_t, MetadataT> instructions = {
  { Bus::Z80::NOP,        { "NOP",              0x01 } }, { Bus::Z80::LD_BC_NN,   { "LD BC, 0x%04x",   0x23 } },
  { Bus::Z80::DJNZ_D,     { "DJNZ 0x%02x",      0x12 } }, { Bus::Z80::LD_DE_NN,   { "LD DE, 0x%04x",   0x23 } },
  { Bus::Z80::JR_NZ_D,    { "JR NZ, 0x%02x",    0x12 } }, { Bus::Z80::LD_HL_NN,   { "LD HL, 0x%04x",   0x23 } },
  { Bus::Z80::JR_NC_D,    { "JR NZ, 0x%02x",    0x12 } }, { Bus::Z80::LD_SP_NN,   { "LD SP, 0x%04x",   0x23 } },
  { Bus::Z80::LD_B_B,     { "LD B, B",          0x01 } }, { Bus::Z80::LD_B_C,     { "LD B, C",         0x01 } },
  { Bus::Z80::LD_D_B,     { "LD D, B",          0x01 } }, { Bus::Z80::LD_D_C,     { "LD D, C",         0x01 } },
  { Bus::Z80::LD_H_B,     { "LD H, B",          0x01 } }, { Bus::Z80::LD_H_C,     { "LD H, C",         0x01 } },
  { Bus::Z80::LD_hl_B,    { "LD (HL), B",       0x01 } }, { Bus::Z80::LD_hl_C,    { "LD (HL), C",      0x01 } },
  { Bus::Z80::ADD_A_B,    { "ADD A, B",         0x01 } }, { Bus::Z80::ADD_A_C,    { "ADD A, C",        0x01 } },
  { Bus::Z80::SUB_B,      { "SUB B",            0x01 } }, { Bus::Z80::SUB_C,      { "SUB C",           0x01 } },
  { Bus::Z80::AND_B,      { "AND B",            0x01 } }, { Bus::Z80::AND_C,      { "AND C",           0x01 } },
  { Bus::Z80::OR_B,       { "OR B",             0x01 } }, { Bus::Z80::OR_C,       { "OR C",            0x01 } },
  { Bus::Z80::RET_NZ,     { "RET NZ",           0x01 } }, { Bus::Z80::POP_BC,     { "POP BC",          0x01 } },
  { Bus::Z80::RET_NC,     { "RET NC",           0x01 } }, { Bus::Z80::POP_DE,     { "POP DE",          0x01 } },
  { Bus::Z80::RET_PO,     { "RET PO",           0x01 } }, { Bus::Z80::POP_HL,     { "POP HL",          0x01 } },
  { Bus::Z80::RET_P,      { "RET P",            0x01 } }, { Bus::Z80::POP_AF,     { "POP AF",          0x01 } },
  { Bus::Z80::LD_bc_A,    { "LD (BC), A",       0x01 } }, { Bus::Z80::INC_BC,     { "INC BC",          0x01 } },
  { Bus::Z80::LD_de_A,    { "LD (DE), A",       0x01 } }, { Bus::Z80::INC_DE,     { "INC DE",          0x01 } },
  { Bus::Z80::LD_nn_HL,   { "LD (0x%04x), HL",  0x23 } }, { Bus::Z80::INC_HL,     { "INC HL",          0x01 } },
  { Bus::Z80::LD_nn_A,    { "LD (0x%04x), A",   0x23 } }, { Bus::Z80::INC_SP,     { "INC SP",          0x01 } },
  { Bus::Z80::LD_B_D,     { "LD B, D",          0x01 } }, { Bus::Z80::LD_B_E,     { "LD B, E",         0x01 } },
  { Bus::Z80::LD_D_D,     { "LD D, D",          0x01 } }, { Bus::Z80::LD_D_E,     { "LD D, E",         0x01 } },
  { Bus::Z80::LD_H_D,     { "LD H, D",          0x01 } }, { Bus::Z80::LD_H_E,     { "LD H, E",         0x01 } },
  { Bus::Z80::LD_hl_D,    { "LD (HL), D",       0x01 } }, { Bus::Z80::LD_hl_E,    { "LD (HL), E",      0x01 } },
  { Bus::Z80::ADD_A_D,    { "ADD A, D",         0x01 } }, { Bus::Z80::ADD_A_E,    { "ADD A, E",        0x01 } },
  { Bus::Z80::SUB_D,      { "SUB D",            0x01 } }, { Bus::Z80::SUB_E,      { "SUB E",           0x01 } },
  { Bus::Z80::AND_D,      { "AND D",            0x01 } }, { Bus::Z80::AND_E,      { "AND E",           0x01 } },
  { Bus::Z80::OR_D,       { "OR D",             0x01 } }, { Bus::Z80::OR_E,       { "OR E",            0x01 } },
  { Bus::Z80::JP_NZ_NN,   { "JP NZ, 0x%04x",    0x23 } }, { Bus::Z80::JP_NN,      { "JP 0x%04x",       0x23 } },
  { Bus::Z80::JP_NC_NN,   { "JP NC, 0x%04x",    0x23 } }, { Bus::Z80::OUT_n_A,    { "OUT (0x%02x), A", 0x12 } },
  { Bus::Z80::JP_PO_NN,   { "JP PO, 0x%04x",    0x23 } }, { Bus::Z80::EX_sp_HL,   { "EX (SP), HL",     0x01 } },
  { Bus::Z80::JP_P_NN,    { "JP P, 0x%04x",     0x23 } }, { Bus::Z80::DI,         { "DI",              0x01 } },
  { Bus::Z80::INC_B,      { "INC B",            0x01 } }, { Bus::Z80::DEC_B,      { "DEC B",           0x01 } },
  { Bus::Z80::INC_D,      { "INC D",            0x01 } }, { Bus::Z80::DEC_D,      { "DEC D",           0x01 } },
  { Bus::Z80::INC_H,      { "INC H",            0x01 } }, { Bus::Z80::DEC_H,      { "DEC H",           0x01 } },
  { Bus::Z80::INC_hl,     { "INC (HL)",         0x01 } }, { Bus::Z80::DEC_hl,     { "DEC (HL)",        0x01 } },
  { Bus::Z80::LD_B_H,     { "LD B, H",          0x01 } }, { Bus::Z80::LD_B_L,     { "LD B, L",         0x01 } },
  { Bus::Z80::LD_D_H,     { "LD D, H",          0x01 } }, { Bus::Z80::LD_D_L,     { "LD D, L",         0x01 } },
  { Bus::Z80::LD_H_H,     { "LD H, H",          0x01 } }, { Bus::Z80::LD_H_L,     { "LD H, L",         0x01 } },
  { Bus::Z80::LD_hl_H,    { "LD (HL), H",       0x01 } }, { Bus::Z80::LD_hl_L,    { "LD (HL), L",      0x01 } },
  { Bus::Z80::ADD_A_H,    { "ADD A, H",         0x01 } }, { Bus::Z80::ADD_A_L,    { "ADD A, L",        0x01 } },
  { Bus::Z80::SUB_H,      { "SUB H",            0x01 } }, { Bus::Z80::SUB_L,      { "SUB L",           0x01 } },
  { Bus::Z80::AND_H,      { "AND H",            0x01 } }, { Bus::Z80::AND_L,      { "AND L",           0x01 } },
  { Bus::Z80::OR_H,       { "OR H",             0x01 } }, { Bus::Z80::OR_L,       { "OR L",            0x01 } },
  { Bus::Z80::CALL_NZ_NN, { "CALL NZ, 0x%04x",  0x23 } }, { Bus::Z80::PUSH_BC,    { "PUSH BC",         0x01 } },
  { Bus::Z80::CALL_NC_NN, { "CALL NC, 0x%04x",  0x23 } }, { Bus::Z80::PUSH_DE,    { "PUSH DE",         0x01 } },
  { Bus::Z80::CALL_PO_NN, { "CALL PO, 0x%04x",  0x23 } }, { Bus::Z80::PUSH_HL,    { "PUSH HL",         0x01 } },
  { Bus::Z80::CALL_P_NN,  { "CALL P, 0x%04x",   0x23 } }, { Bus::Z80::PUSH_AF,    { "PUSH AF",         0x01 } },
  { Bus::Z80::LD_B_N,     { "LD B, 0x%02x",     0x12 } }, { Bus::Z80::RLCA,       { "RLCA",            0x01 } },
  { Bus::Z80::LD_D_N,     { "LD D, 0x%02x",     0x12 } }, { Bus::Z80::RLA,        { "RLA",             0x01 } },
  { Bus::Z80::LD_H_N,     { "LD H, 0x%02x",     0x12 } }, { Bus::Z80::DAA,        { "DAA",             0x01 } },
  { Bus::Z80::LD_hl_n,    { "LD (HL), 0x%02x",  0x12 } }, { Bus::Z80::SCF,        { "SCF",             0x01 } },
  { Bus::Z80::LD_B_hl,    { "LD B, (HL)",       0x01 } }, { Bus::Z80::LD_B_A,     { "LD B, A",         0x01 } },
  { Bus::Z80::LD_D_hl,    { "LD D, (HL)",       0x01 } }, { Bus::Z80::LD_D_A,     { "LD D, A",         0x01 } },
  { Bus::Z80::LD_H_hl,    { "LD H, (HL)",       0x01 } }, { Bus::Z80::LD_H_A,     { "LD H, A",         0x01 } },
  { Bus::Z80::HALT,       { "HALT",             0x01 } }, { Bus::Z80::LD_hl_A,    { "LD (HL), A",      0x01 } },
  { Bus::Z80::ADD_A_hl,   { "ADD A, (HL)",      0x01 } }, { Bus::Z80::ADD_A_A,    { "ADD A, A",        0x01 } },
  { Bus::Z80::SUB_hl,     { "SUB (HL)",         0x01 } }, { Bus::Z80::SUB_A,      { "SUB A",           0x01 } },
  { Bus::Z80::AND_hl,     { "AND (HL)",         0x01 } }, { Bus::Z80::AND_A,      { "AND A",           0x01 } },
  { Bus::Z80::OR_hl,      { "OR (HL)",          0x01 } }, { Bus::Z80::OR_A,       { "OR A",            0x01 } },
  { Bus::Z80::ADD_A_N,    { "ADD A, 0x%02x",    0x12 } }, { Bus::Z80::RST_00h,    { "RST 0x00",        0x01 } },
  { Bus::Z80::SUB_N,      { "SUB 0x%02x",       0x12 } }, { Bus::Z80::RST_10h,    { "RST 0x10",        0x01 } },
  { Bus::Z80::AND_N,      { "AND 0x%02x",       0x12 } }, { Bus::Z80::RST_20h,    { "RST 0x20",        0x01 } },
  { Bus::Z80::OR_N,       { "OR 0x%02x",        0x12 } }, { Bus::Z80::RST_30h,    { "RST 0x30",        0x01 } },
  { Bus::Z80::EX_AF_AF,   { "EX AF, AF'",       0x01 } }, { Bus::Z80::ADD_HL_BC,  { "ADD HL, BC",      0x01 } },
  { Bus::Z80::JR_D,       { "JR 0x%02x",        0x12 } }, { Bus::Z80::ADD_HL_DE,  { "ADD HL, DE",      0x01 } },
  { Bus::Z80::JR_Z_D,     { "JR Z, 0x%02x",     0x12 } }, { Bus::Z80::ADD_HL_HL,  { "ADD HL, HL",      0x01 } },
  { Bus::Z80::JR_C_D,     { "JR C, 0x%02x",     0x12 } }, { Bus::Z80::ADD_HL_SP,  { "ADD HL, SP",      0x01 } },
  { Bus::Z80::LD_C_B,     { "LD C, B",          0x01 } }, { Bus::Z80::LD_C_C,     { "LD C, C",         0x01 } },
  { Bus::Z80::LD_E_B,     { "LD E, B",          0x01 } }, { Bus::Z80::LD_E_C,     { "LD E, C",         0x01 } },
  { Bus::Z80::LD_L_B,     { "LD L, B",          0x01 } }, { Bus::Z80::LD_L_C,     { "LD L, C",         0x01 } },
  { Bus::Z80::LD_A_B,     { "LD A, B",          0x01 } }, { Bus::Z80::LD_A_C,     { "LD A, C",         0x01 } },
  { Bus::Z80::ADC_A_B,    { "ADC A, B",         0x01 } }, { Bus::Z80::ADC_A_C,    { "ADC A, C",        0x01 } },
  { Bus::Z80::SBC_A_B,    { "SBC A, B",         0x01 } }, { Bus::Z80::SBC_A_C,    { "SBC A, C",        0x01 } },
  { Bus::Z80::XOR_B,      { "XOR B",            0x01 } }, { Bus::Z80::XOR_C,      { "XOR C",           0x01 } },
  { Bus::Z80::CP_B,       { "CP B",             0x01 } }, { Bus::Z80::CP_C,       { "CP C",            0x01 } },
  { Bus::Z80::RET_Z,      { "RET Z",            0x01 } }, { Bus::Z80::RET,        { "RET",             0x01 } },
  { Bus::Z80::RET_C,      { "RET C",            0x01 } }, { Bus::Z80::EXX,        { "EXX",             0x01 } },
  { Bus::Z80::RET_PE,     { "RET PE",           0x01 } }, { Bus::Z80::JP_hl,      { "JP (HL)",         0x01 } },
  { Bus::Z80::RET_M,      { "RET M",            0x01 } }, { Bus::Z80::LD_SP_HL,   { "LD SP, HL",       0x01 } },
  { Bus::Z80::LD_A_bc,    { "LD A, (BC)",       0x01 } }, { Bus::Z80::DEC_BC,     { "DEC BC",          0x01 } },
  { Bus::Z80::LD_A_de,    { "LD A, (DE)",       0x01 } }, { Bus::Z80::DEC_DE,     { "DEC DE",          0x01 } },
  { Bus::Z80::LD_HL_nn,   { "LD HL, (0x%04x)",  0x23 } }, { Bus::Z80::DEC_HL,     { "DEC HL",          0x01 } },
  { Bus::Z80::LD_A_nn,    { "LD A, (0x%04x)",   0x23 } }, { Bus::Z80::DEC_SP,     { "DEC SP",          0x01 } },
  { Bus::Z80::LD_C_D,     { "LD C, D",          0x01 } }, { Bus::Z80::LD_C_E,     { "LD C, E",         0x01 } },
  { Bus::Z80::LD_E_D,     { "LD E, D",          0x01 } }, { Bus::Z80::LD_E_E,     { "LD E, E",         0x01 } },
  { Bus::Z80::LD_L_D,     { "LD L, D",          0x01 } }, { Bus::Z80::LD_L_E,     { "LD L, E",         0x01 } },
  { Bus::Z80::LD_A_D,     { "LD A, D",          0x01 } }, { Bus::Z80::LD_A_E,     { "LD A, E",         0x01 } },
  { Bus::Z80::ADC_A_D,    { "ADC A, D",         0x01 } }, { Bus::Z80::ADC_A_E,    { "ADC A, E",        0x01 } },
  { Bus::Z80::SBC_A_D,    { "SBC A, D",         0x01 } }, { Bus::Z80::SBC_A_E,    { "SBC A, E",        0x01 } },
  { Bus::Z80::XOR_D,      { "XOR D",            0x01 } }, { Bus::Z80::XOR_E,      { "XOR E",           0x01 } },
  { Bus::Z80::CP_D,       { "CP D",             0x01 } }, { Bus::Z80::CP_E,       { "CP E",            0x01 } },
  { Bus::Z80::JP_Z_NN,    { "JP Z, 0x%04x",     0x23 } }, { Bus::Z80::BIT_INSTR,  { "",                0x01 } },
  { Bus::Z80::JP_C_NN,    { "JP C, 0x%04x",     0x23 } }, { Bus::Z80::IN_A_n,     { "IN A, (0x%02x)",  0x12 } },
  { Bus::Z80::JP_PE_NN,   { "JP PE, 0x%04x",    0x23 } }, { Bus::Z80::EX_DE_HL,   { "EX DE, HL",       0x01 } },
  { Bus::Z80::JP_M_NN,    { "JP M, 0x%04x",     0x23 } }, { Bus::Z80::EI,         { "EI",              0x01 } },
  { Bus::Z80::INC_C,      { "INC C",            0x01 } }, { Bus::Z80::DEC_C,      { "DEC C",           0x01 } },
  { Bus::Z80::INC_E,      { "INC E",            0x01 } }, { Bus::Z80::DEC_E,      { "DEC E",           0x01 } },
  { Bus::Z80::INC_L,      { "INC L",            0x01 } }, { Bus::Z80::DEC_L,      { "DEC L",           0x01 } },
  { Bus::Z80::INC_A,      { "INC A",            0x01 } }, { Bus::Z80::DEC_A,      { "DEC A",           0x01 } },
  { Bus::Z80::LD_C_H,     { "LD C, H",          0x01 } }, { Bus::Z80::LD_C_L,     { "LD C, L",         0x01 } },
  { Bus::Z80::LD_E_H,     { "LD E, H",          0x01 } }, { Bus::Z80::LD_E_L,     { "LD E, L",         0x01 } },
  { Bus::Z80::LD_L_H,     { "LD L, H",          0x01 } }, { Bus::Z80::LD_L_L,     { "LD L, L",         0x01 } },
  { Bus::Z80::LD_A_H,     { "LD A, H",          0x01 } }, { Bus::Z80::LD_A_L,     { "LD A, L",         0x01 } },
  { Bus::Z80::ADC_A_H,    { "ADC A, H",         0x01 } }, { Bus::Z80::ADC_A_L,    { "ADC A, L",        0x01 } },
  { Bus::Z80::SBC_A_H,    { "SBC A, H",         0x01 } }, { Bus::Z80::SBC_A_L,    { "SBC A, L",        0x01 } },
  { Bus::Z80::XOR_H,      { "XOR H",            0x01 } }, { Bus::Z80::XOR_L,      { "XOR L",           0x01 } },
  { Bus::Z80::CP_H,       { "CP H",             0x01 } }, { Bus::Z80::CP_L,       { "CP L",            0x01 } },
  { Bus::Z80::CALL_Z_NN,  { "CALL Z, 0x%04x",   0x23 } }, { Bus::Z80::CALL_NN,    { "CALL 0x%04x",     0x23 } },
  { Bus::Z80::CALL_C_NN,  { "CALL C, 0x%04x",   0x23 } }, { Bus::Z80::IX_INSTR,   { "",                0x01 } },
  { Bus::Z80::CALL_PE_NN, { "CALL PE, 0x%04x",  0x23 } }, { Bus::Z80::MISC_INSTR, { "",                0x01 } },
  { Bus::Z80::CALL_M_NN,  { "CALL M, 0x%04x",   0x23 } }, { Bus::Z80::IY_INSTR,   { "",                0x01 } },
  { Bus::Z80::LD_C_N,     { "LD C, 0x%02x",     0x12 } }, { Bus::Z80::RRCA,       { "RRCA",            0x01 } },
  { Bus::Z80::LD_E_N,     { "LD E, 0x%02x",     0x12 } }, { Bus::Z80::RRA,        { "RRA",             0x01 } },
  { Bus::Z80::LD_L_N,     { "LD L, 0x%02x",     0x12 } }, { Bus::Z80::CPL,        { "CPL",             0x01 } },
  { Bus::Z80::LD_A_N,     { "LD A, 0x%02x",     0x12 } }, { Bus::Z80::CCF,        { "CCF",             0x01 } },
  { Bus::Z80::LD_C_hl,    { "LD C, (HL)",       0x01 } }, { Bus::Z80::LD_C_A,     { "LD C, A",         0x01 } },
  { Bus::Z80::LD_E_hl,    { "LD E, (HL)",       0x01 } }, { Bus::Z80::LD_E_A,     { "LD E, A",         0x01 } },
  { Bus::Z80::LD_L_hl,    { "LD L, (HL)",       0x01 } }, { Bus::Z80::LD_L_A,     { "LD L, A",         0x01 } },
  { Bus::Z80::LD_A_hl,    { "LD A, (HL)",       0x01 } }, { Bus::Z80::LD_A_A,     { "LD A, A",         0x01 } },
  { Bus::Z80::ADC_A_hl,   { "ADC A, (HL)",      0x01 } }, { Bus::Z80::ADC_A_A,    { "ADC A, A",        0x01 } },
  { Bus::Z80::SBC_A_hl,   { "SBC A, (HL)",      0x01 } }, { Bus::Z80::SBC_A_A,    { "SBC A, A",        0x01 } },
  { Bus::Z80::XOR_hl,     { "XOR (HL)",         0x01 } }, { Bus::Z80::XOR_A,      { "XOR A",           0x01 } },
  { Bus::Z80::CP_hl,      { "CP (HL)",          0x01 } }, { Bus::Z80::CP_A,       { "CP A",            0x01 } },
  { Bus::Z80::ADC_A_N,    { "ADC A, 0x%02x",    0x12 } }, { Bus::Z80::RST_08h,    { "RST 0x08",        0x01 } },
  { Bus::Z80::SBC_A_N,    { "SBC A, 0x%02x",    0x12 } }, { Bus::Z80::RST_18h,    { "RST 0x18",        0x01 } },
  { Bus::Z80::XOR_N,      { "XOR 0x%02x",       0x12 } }, { Bus::Z80::RST_28h,    { "RST 0x28",        0x01 } },
  { Bus::Z80::CP_N,       { "CP 0x%02x",        0x12 } }, { Bus::Z80::RST_38h,    { "RST 0x38",        0x01 } },
};

static inline const std::unordered_map<uint8_t, MetadataT> bits = {
  { Bus::Z80::RLC_B,    { "RLC B",       0x01 } }, { Bus::Z80::RLC_C,   { "RLC C",    0x01 } },
  { Bus::Z80::RL_B,     { "RL B",        0x01 } }, { Bus::Z80::RL_C,    { "RL C",     0x01 } },
  { Bus::Z80::SLA_B,    { "SLA B",       0x01 } }, { Bus::Z80::SLA_C,   { "SLA C",    0x01 } },
  { Bus::Z80::SLL_B,    { "SLL B",       0x01 } }, { Bus::Z80::SLL_C,   { "SLL C",    0x01 } },
  { Bus::Z80::BIT_0_B,  { "BIT 0, B",    0x01 } }, { Bus::Z80::BIT_0_C, { "BIT 0, C", 0x01 } },
  { Bus::Z80::BIT_2_B,  { "BIT 2, B",    0x01 } }, { Bus::Z80::BIT_2_C, { "BIT 2, C", 0x01 } },
  { Bus::Z80::BIT_4_B,  { "BIT 4, B",    0x01 } }, { Bus::Z80::BIT_4_C, { "BIT 4, C", 0x01 } },
  { Bus::Z80::BIT_6_B,  { "BIT 6, B",    0x01 } }, { Bus::Z80::BIT_6_C, { "BIT 6, C", 0x01 } },
  { Bus::Z80::RES_0_B,  { "RES 0, B",    0x01 } }, { Bus::Z80::RES_0_C, { "RES 0, C", 0x01 } },
  { Bus::Z80::RES_2_B,  { "RES 2, B",    0x01 } }, { Bus::Z80::RES_2_C, { "RES 2, C", 0x01 } },
  { Bus::Z80::RES_4_B,  { "RES 4, B",    0x01 } }, { Bus::Z80::RES_4_C, { "RES 4, C", 0x01 } },
  { Bus::Z80::RES_6_B,  { "RES 6, B",    0x01 } }, { Bus::Z80::RES_6_C, { "RES 6, C", 0x01 } },
  { Bus::Z80::SET_0_B,  { "SET 0, B",    0x01 } }, { Bus::Z80::SET_0_C, { "SET 0, C", 0x01 } },
  { Bus::Z80::SET_2_B,  { "SET 2, B",    0x01 } }, { Bus::Z80::SET_2_C, { "SET 2, C", 0x01 } },
  { Bus::Z80::SET_4_B,  { "SET 4, B",    0x01 } }, { Bus::Z80::SET_4_C, { "SET 4, C", 0x01 } },
  { Bus::Z80::SET_6_B,  { "SET 6, B",    0x01 } }, { Bus::Z80::SET_6_C, { "SET 6, C", 0x01 } },
  { Bus::Z80::RLC_D,    { "RLC D",       0x01 } }, { Bus::Z80::RLC_E,   { "RLC E",    0x01 } },
  { Bus::Z80::RL_D,     { "RL D",        0x01 } }, { Bus::Z80::RL_E,    { "RL E",     0x01 } },
  { Bus::Z80::SLA_D,    { "SLA D",       0x01 } }, { Bus::Z80::SLA_E,   { "SLA E",    0x01 } },
  { Bus::Z80::SLL_D,    { "SLL D",       0x01 } }, { Bus::Z80::SLL_E,   { "SLL E",    0x01 } },
  { Bus::Z80::BIT_0_D,  { "BIT 0, D",    0x01 } }, { Bus::Z80::BIT_0_E, { "BIT 0, E", 0x01 } },
  { Bus::Z80::BIT_2_D,  { "BIT 2, D",    0x01 } }, { Bus::Z80::BIT_2_E, { "BIT 2, E", 0x01 } },
  { Bus::Z80::BIT_4_D,  { "BIT 4, D",    0x01 } }, { Bus::Z80::BIT_4_E, { "BIT 4, E", 0x01 } },
  { Bus::Z80::BIT_6_D,  { "BIT 6, D",    0x01 } }, { Bus::Z80::BIT_6_E, { "BIT 6, E", 0x01 } },
  { Bus::Z80::RES_0_D,  { "RES 0, D",    0x01 } }, { Bus::Z80::RES_0_E, { "RES 0, E", 0x01 } },
  { Bus::Z80::RES_2_D,  { "RES 2, D",    0x01 } }, { Bus::Z80::RES_2_E, { "RES 2, E", 0x01 } },
  { Bus::Z80::RES_4_D,  { "RES 4, D",    0x01 } }, { Bus::Z80::RES_4_E, { "RES 4, E", 0x01 } },
  { Bus::Z80::RES_6_D,  { "RES 6, D",    0x01 } }, { Bus::Z80::RES_6_E, { "RES 6, E", 0x01 } },
  { Bus::Z80::SET_0_D,  { "SET 0, D",    0x01 } }, { Bus::Z80::SET_0_E, { "SET 0, E", 0x01 } },
  { Bus::Z80::SET_2_D,  { "SET 2, D",    0x01 } }, { Bus::Z80::SET_2_E, { "SET 2, E", 0x01 } },
  { Bus::Z80::SET_4_D,  { "SET 4, D",    0x01 } }, { Bus::Z80::SET_4_E, { "SET 4, E", 0x01 } },
  { Bus::Z80::SET_6_D,  { "SET 6, D",    0x01 } }, { Bus::Z80::SET_6_E, { "SET 6, E", 0x01 } },
  { Bus::Z80::RLC_H,    { "RLC H",       0x01 } }, { Bus::Z80::RLC_L,   { "RLC L",    0x01 } },
  { Bus::Z80::RL_H,     { "RL H",        0x01 } }, { Bus::Z80::RL_L,    { "RL L",     0x01 } },
  { Bus::Z80::SLA_H,    { "SLA H",       0x01 } }, { Bus::Z80::SLA_L,   { "SLA L",    0x01 } },
  { Bus::Z80::SLL_H,    { "SLL H",       0x01 } }, { Bus::Z80::SLL_L,   { "SLL L",    0x01 } },
  { Bus::Z80::BIT_0_H,  { "BIT 0, H",    0x01 } }, { Bus::Z80::BIT_0_L, { "BIT 0, L", 0x01 } },
  { Bus::Z80::BIT_2_H,  { "BIT 2, H",    0x01 } }, { Bus::Z80::BIT_2_L, { "BIT 2, L", 0x01 } },
  { Bus::Z80::BIT_4_H,  { "BIT 4, H",    0x01 } }, { Bus::Z80::BIT_4_L, { "BIT 4, L", 0x01 } },
  { Bus::Z80::BIT_6_H,  { "BIT 6, H",    0x01 } }, { Bus::Z80::BIT_6_L, { "BIT 6, L", 0x01 } },
  { Bus::Z80::RES_0_H,  { "RES 0, H",    0x01 } }, { Bus::Z80::RES_0_L, { "RES 0, L", 0x01 } },
  { Bus::Z80::RES_2_H,  { "RES 2, H",    0x01 } }, { Bus::Z80::RES_2_L, { "RES 2, L", 0x01 } },
  { Bus::Z80::RES_4_H,  { "RES 4, H",    0x01 } }, { Bus::Z80::RES_4_L, { "RES 4, L", 0x01 } },
  { Bus::Z80::RES_6_H,  { "RES 6, H",    0x01 } }, { Bus::Z80::RES_6_L, { "RES 6, L", 0x01 } },
  { Bus::Z80::SET_0_H,  { "SET 0, H",    0x01 } }, { Bus::Z80::SET_0_L, { "SET 0, L", 0x01 } },
  { Bus::Z80::SET_2_H,  { "SET 2, H",    0x01 } }, { Bus::Z80::SET_2_L, { "SET 2, L", 0x01 } },
  { Bus::Z80::SET_4_H,  { "SET 4, H",    0x01 } }, { Bus::Z80::SET_4_L, { "SET 4, L", 0x01 } },
  { Bus::Z80::SET_6_H,  { "SET 6, H",    0x01 } }, { Bus::Z80::SET_6_L, { "SET 6, L", 0x01 } },
  { Bus::Z80::RLC_hl,   { "RLC (HL)",    0x01 } }, { Bus::Z80::RLC_A,   { "RLC A",    0x01 } },
  { Bus::Z80::RL_hl,    { "RL (HL)",     0x01 } }, { Bus::Z80::RL_A,    { "RL A",     0x01 } },
  { Bus::Z80::SLA_hl,   { "SLA (HL)",    0x01 } }, { Bus::Z80::SLA_A,   { "SLA A",    0x01 } },
  { Bus::Z80::SLL_hl,   { "SLL (HL)",    0x01 } }, { Bus::Z80::SLL_A,   { "SLL A",    0x01 } },
  { Bus::Z80::BIT_0_hl, { "BIT 0, (HL)", 0x01 } }, { Bus::Z80::BIT_0_A, { "BIT 0, A", 0x01 } },
  { Bus::Z80::BIT_2_hl, { "BIT 2, (HL)", 0x01 } }, { Bus::Z80::BIT_2_A, { "BIT 2, A", 0x01 } },
  { Bus::Z80::BIT_4_hl, { "BIT 4, (HL)", 0x01 } }, { Bus::Z80::BIT_4_A, { "BIT 4, A", 0x01 } },
  { Bus::Z80::BIT_6_hl, { "BIT 6, (HL)", 0x01 } }, { Bus::Z80::BIT_6_A, { "BIT 6, A", 0x01 } },
  { Bus::Z80::RES_0_hl, { "RES 0, (HL)", 0x01 } }, { Bus::Z80::RES_0_A, { "RES 0, A", 0x01 } },
  { Bus::Z80::RES_2_hl, { "RES 2, (HL)", 0x01 } }, { Bus::Z80::RES_2_A, { "RES 2, A", 0x01 } },
  { Bus::Z80::RES_4_hl, { "RES 4, (HL)", 0x01 } }, { Bus::Z80::RES_4_A, { "RES 4, A", 0x01 } },
  { Bus::Z80::RES_6_hl, { "RES 6, (HL)", 0x01 } }, { Bus::Z80::RES_6_A, { "RES 6, A", 0x01 } },
  { Bus::Z80::SET_0_hl, { "SET 0, (HL)", 0x01 } }, { Bus::Z80::SET_0_A, { "SET 0, A", 0x01 } },
  { Bus::Z80::SET_2_hl, { "SET 2, (HL)", 0x01 } }, { Bus::Z80::SET_2_A, { "SET 2, A", 0x01 } },
  { Bus::Z80::SET_4_hl, { "SET 4, (HL)", 0x01 } }, { Bus::Z80::SET_4_A, { "SET 4, A", 0x01 } },
  { Bus::Z80::SET_6_hl, { "SET 6, (HL)", 0x01 } }, { Bus::Z80::SET_6_A, { "SET 6, A", 0x01 } },
  { Bus::Z80::RRC_B,    { "RRC B",       0x01 } }, { Bus::Z80::RRC_C,   { "RRC C",    0x01 } },
  { Bus::Z80::RR_B,     { "RR B",        0x01 } }, { Bus::Z80::RR_C,    { "RR C",     0x01 } },
  { Bus::Z80::SRA_B,    { "SRA B",       0x01 } }, { Bus::Z80::SRA_C,   { "SRA C",    0x01 } },
  { Bus::Z80::SRL_B,    { "SRL B",       0x01 } }, { Bus::Z80::SRL_C,   { "SRL C",    0x01 } },
  { Bus::Z80::BIT_1_B,  { "BIT 1, B",    0x01 } }, { Bus::Z80::BIT_1_C, { "BIT 1, C", 0x01 } },
  { Bus::Z80::BIT_3_B,  { "BIT 3, B",    0x01 } }, { Bus::Z80::BIT_3_C, { "BIT 3, C", 0x01 } },
  { Bus::Z80::BIT_5_B,  { "BIT 5, B",    0x01 } }, { Bus::Z80::BIT_5_C, { "BIT 5, C", 0x01 } },
  { Bus::Z80::BIT_7_B,  { "BIT 7, B",    0x01 } }, { Bus::Z80::BIT_7_C, { "BIT 7, C", 0x01 } },
  { Bus::Z80::RES_1_B,  { "RES 1, B",    0x01 } }, { Bus::Z80::RES_1_C, { "RES 1, C", 0x01 } },
  { Bus::Z80::RES_3_B,  { "RES 3, B",    0x01 } }, { Bus::Z80::RES_3_C, { "RES 3, C", 0x01 } },
  { Bus::Z80::RES_5_B,  { "RES 5, B",    0x01 } }, { Bus::Z80::RES_5_C, { "RES 5, C", 0x01 } },
  { Bus::Z80::RES_7_B,  { "RES 7, B",    0x01 } }, { Bus::Z80::RES_7_C, { "RES 7, C", 0x01 } },
  { Bus::Z80::SET_1_B,  { "SET 1, B",    0x01 } }, { Bus::Z80::SET_1_C, { "SET 1, C", 0x01 } },
  { Bus::Z80::SET_3_B,  { "SET 3, B",    0x01 } }, { Bus::Z80::SET_3_C, { "SET 3, C", 0x01 } },
  { Bus::Z80::SET_5_B,  { "SET 5, B",    0x01 } }, { Bus::Z80::SET_5_C, { "SET 5, C", 0x01 } },
  { Bus::Z80::SET_7_B,  { "SET 7, B",    0x01 } }, { Bus::Z80::SET_7_C, { "SET 7, C", 0x01 } },
  { Bus::Z80::RRC_D,    { "RRC D",       0x01 } }, { Bus::Z80::RRC_E,   { "RRC E",    0x01 } },
  { Bus::Z80::RR_D,     { "RR D",        0x01 } }, { Bus::Z80::RR_E,    { "RR E",     0x01 } },
  { Bus::Z80::SRA_D,    { "SRA D",       0x01 } }, { Bus::Z80::SRA_E,   { "SRA E",    0x01 } },
  { Bus::Z80::SRL_D,    { "SRL D",       0x01 } }, { Bus::Z80::SRL_E,   { "SRL E",    0x01 } },
  { Bus::Z80::BIT_1_D,  { "BIT 1, D",    0x01 } }, { Bus::Z80::BIT_1_E, { "BIT 1, E", 0x01 } },
  { Bus::Z80::BIT_3_D,  { "BIT 3, D",    0x01 } }, { Bus::Z80::BIT_3_E, { "BIT 3, E", 0x01 } },
  { Bus::Z80::BIT_5_D,  { "BIT 5, D",    0x01 } }, { Bus::Z80::BIT_5_E, { "BIT 5, E", 0x01 } },
  { Bus::Z80::BIT_7_D,  { "BIT 7, D",    0x01 } }, { Bus::Z80::BIT_7_E, { "BIT 7, E", 0x01 } },
  { Bus::Z80::RES_1_D,  { "RES 1, D",    0x01 } }, { Bus::Z80::RES_1_E, { "RES 1, E", 0x01 } },
  { Bus::Z80::RES_3_D,  { "RES 3, D",    0x01 } }, { Bus::Z80::RES_3_E, { "RES 3, E", 0x01 } },
  { Bus::Z80::RES_5_D,  { "RES 5, D",    0x01 } }, { Bus::Z80::RES_5_E, { "RES 5, E", 0x01 } },
  { Bus::Z80::RES_7_D,  { "RES 7, D",    0x01 } }, { Bus::Z80::RES_7_E, { "RES 7, E", 0x01 } },
  { Bus::Z80::SET_1_D,  { "SET 1, D",    0x01 } }, { Bus::Z80::SET_1_E, { "SET 1, E", 0x01 } },
  { Bus::Z80::SET_3_D,  { "SET 3, D",    0x01 } }, { Bus::Z80::SET_3_E, { "SET 3, E", 0x01 } },
  { Bus::Z80::SET_5_D,  { "SET 5, D",    0x01 } }, { Bus::Z80::SET_5_E, { "SET 5, E", 0x01 } },
  { Bus::Z80::SET_7_D,  { "SET 7, D",    0x01 } }, { Bus::Z80::SET_7_E, { "SET 7, E", 0x01 } },
  { Bus::Z80::RRC_H,    { "RRC H",       0x01 } }, { Bus::Z80::RRC_L,   { "RRC L",    0x01 } },
  { Bus::Z80::RR_H,     { "RR H",        0x01 } }, { Bus::Z80::RR_L,    { "RR L",     0x01 } },
  { Bus::Z80::SRA_H,    { "SRA H",       0x01 } }, { Bus::Z80::SRA_L,   { "SRA L",    0x01 } },
  { Bus::Z80::SRL_H,    { "SRL H",       0x01 } }, { Bus::Z80::SRL_L,   { "SRL L",    0x01 } },
  { Bus::Z80::BIT_1_H,  { "BIT 1, H",    0x01 } }, { Bus::Z80::BIT_1_L, { "BIT 1, L", 0x01 } },
  { Bus::Z80::BIT_3_H,  { "BIT 3, H",    0x01 } }, { Bus::Z80::BIT_3_L, { "BIT 3, L", 0x01 } },
  { Bus::Z80::BIT_5_H,  { "BIT 5, H",    0x01 } }, { Bus::Z80::BIT_5_L, { "BIT 5, L", 0x01 } },
  { Bus::Z80::BIT_7_H,  { "BIT 7, H",    0x01 } }, { Bus::Z80::BIT_7_L, { "BIT 7, L", 0x01 } },
  { Bus::Z80::RES_1_H,  { "RES 1, H",    0x01 } }, { Bus::Z80::RES_1_L, { "RES 1, L", 0x01 } },
  { Bus::Z80::RES_3_H,  { "RES 3, H",    0x01 } }, { Bus::Z80::RES_3_L, { "RES 3, L", 0x01 } },
  { Bus::Z80::RES_5_H,  { "RES 5, H",    0x01 } }, { Bus::Z80::RES_5_L, { "RES 5, L", 0x01 } },
  { Bus::Z80::RES_7_H,  { "RES 7, H",    0x01 } }, { Bus::Z80::RES_7_L, { "RES 7, L", 0x01 } },
  { Bus::Z80::SET_1_H,  { "SET 1, H",    0x01 } }, { Bus::Z80::SET_1_L, { "SET 1, L", 0x01 } },
  { Bus::Z80::SET_3_H,  { "SET 3, H",    0x01 } }, { Bus::Z80::SET_3_L, { "SET 3, L", 0x01 } },
  { Bus::Z80::SET_5_H,  { "SET 5, H",    0x01 } }, { Bus::Z80::SET_5_L, { "SET 5, L", 0x01 } },
  { Bus::Z80::SET_7_H,  { "SET 7, H",    0x01 } }, { Bus::Z80::SET_7_L, { "SET 7, L", 0x01 } },
  { Bus::Z80::RRC_hl,   { "RRC (HL)",    0x01 } }, { Bus::Z80::RRC_A,   { "RRC A",    0x01 } },
  { Bus::Z80::RR_hl,    { "RR (HL)",     0x01 } }, { Bus::Z80::RR_A,    { "RR A",     0x01 } },
  { Bus::Z80::SRA_hl,   { "SRA (HL)",    0x01 } }, { Bus::Z80::SRA_A,   { "SRA A",    0x01 } },
  { Bus::Z80::SRL_hl,   { "SRL (HL)",    0x01 } }, { Bus::Z80::SRL_A,   { "SRL A",    0x01 } },
  { Bus::Z80::BIT_1_hl, { "BIT 1, (HL)", 0x01 } }, { Bus::Z80::BIT_1_A, { "BIT 1, A", 0x01 } },
  { Bus::Z80::BIT_3_hl, { "BIT 3, (HL)", 0x01 } }, { Bus::Z80::BIT_3_A, { "BIT 3, A", 0x01 } },
  { Bus::Z80::BIT_5_hl, { "BIT 5, (HL)", 0x01 } }, { Bus::Z80::BIT_5_A, { "BIT 5, A", 0x01 } },
  { Bus::Z80::BIT_7_hl, { "BIT 7, (HL)", 0x01 } }, { Bus::Z80::BIT_7_A, { "BIT 7, A", 0x01 } },
  { Bus::Z80::RES_1_hl, { "RES 1, (HL)", 0x01 } }, { Bus::Z80::RES_1_A, { "RES 1, A", 0x01 } },
  { Bus::Z80::RES_3_hl, { "RES 3, (HL)", 0x01 } }, { Bus::Z80::RES_3_A, { "RES 3, A", 0x01 } },
  { Bus::Z80::RES_5_hl, { "RES 5, (HL)", 0x01 } }, { Bus::Z80::RES_5_A, { "RES 5, A", 0x01 } },
  { Bus::Z80::RES_7_hl, { "RES 7, (HL)", 0x01 } }, { Bus::Z80::RES_7_A, { "RES 7, A", 0x01 } },
  { Bus::Z80::SET_1_hl, { "SET 1, (HL)", 0x01 } }, { Bus::Z80::SET_1_A, { "SET 1, A", 0x01 } },
  { Bus::Z80::SET_3_hl, { "SET 3, (HL)", 0x01 } }, { Bus::Z80::SET_3_A, { "SET 3, A", 0x01 } },
  { Bus::Z80::SET_5_hl, { "SET 5, (HL)", 0x01 } }, { Bus::Z80::SET_5_A, { "SET 5, A", 0x01 } },
  { Bus::Z80::SET_7_hl, { "SET 7, (HL)", 0x01 } }, { Bus::Z80::SET_7_A, { "SET 7, A", 0x01 } },
};

static inline const std::unordered_map<uint8_t, MetadataT> ix = {
  { Bus::Z80::ADD_IX_BC, { "ADD IX, BC",             0x001 } }, { Bus::Z80::ADD_IX_DE,  { "ADD IX, DE",       0x001 } },
  { Bus::Z80::INC_ixd,   { "INC (IX+0x%02x)",        0x012 } }, { Bus::Z80::DEC_ixd,    { "DEC (IX+%02x)",    0x012 } },
  { Bus::Z80::LD_ixd_B,  { "LD (IX+0x%02x), B",      0x012 } }, { Bus::Z80::LD_ixd_C,   { "LD (IX+%02x), C",  0x012 } },
  { Bus::Z80::ADD_A_ixd, { "ADD A, (IX+0x%02x)",     0x012 } }, { Bus::Z80::ADC_A_ixd,  { "ADC A, (IX+%02x)", 0x001 } },
  { Bus::Z80::POP_IX,    { "POP IX",                 0x001 } }, { Bus::Z80::EX_sp_IX,   { "EX (SP), IX",      0x001 } },
  { Bus::Z80::LD_IX_NN,  { "LD IX, 0x%04x",          0x023 } }, { Bus::Z80::LD_nn_IX,   { "LD (0x%04x), IX",  0x023 } },
  { Bus::Z80::LD_ixd_N,  { "LD (IX+0x%02x), 0x%04x", 0x113 } }, { Bus::Z80::ADD_IX_SP,  { "ADD IX, SP",       0x001 } },
  { Bus::Z80::LD_ixd_D,  { "LD (IX+0x%02x), D",      0x012 } }, { Bus::Z80::LD_ixd_E,   { "LD (IX+%02x), E",  0x012 } },
  { Bus::Z80::SUB_ixd,   { "SUB (IX+0x%02x)",        0x012 } }, { Bus::Z80::SBC_A_ixd,  { "SBC A, (IX+%02x)", 0x001 } },
  { Bus::Z80::PUSH_IX,   { "PUSH IX",                0x001 } }, { Bus::Z80::JP_ix,      { "JP (IX)",          0x001 } },
  { Bus::Z80::INC_IX,    { "INC IX",                 0x001 } }, { Bus::Z80::ADD_IX_IX,  { "ADD IX, IX",       0x001 } },
  { Bus::Z80::LD_B_ixd,  { "LD B, (IX+0x%02x)",      0x012 } }, { Bus::Z80::LD_C_ixd,   { "LD C, (IX+%02x)",  0x001 } },
  { Bus::Z80::LD_ixd_H,  { "LD (IX+0x%02x), H",      0x012 } }, { Bus::Z80::LD_ixd_L,   { "LD (IX+%02x), L",  0x012 } },
  { Bus::Z80::AND_ixd,   { "AND (IX+0x%02x)",        0x012 } }, { Bus::Z80::XOR_ixd,    { "XOR (IX+%02x)",    0x001 } },
  { Bus::Z80::LD_IX_nn,  { "LD IX, 0x%04x",          0x023 } }, { Bus::Z80::DEC_IX,     { "DEC IX",           0x001 } },
  { Bus::Z80::LD_D_ixd,  { "LD D, (IX+0x%02x)",      0x012 } }, { Bus::Z80::LD_E_ixd,   { "LD E, (IX+%02x)",  0x001 } },
  { Bus::Z80::LD_H_ixd,  { "LD H, (IX+0x%02x)",      0x012 } }, { Bus::Z80::LD_L_ixd,   { "LD L, (IX+%02x)",  0x001 } },
  { Bus::Z80::LD_ixd_A,  { "LD (IX+0x%02x), A",      0x012 } }, { Bus::Z80::LD_A_ixd,   { "LD A, (IX+%02x)",  0x001 } },
  { Bus::Z80::OR_ixd,    { "OR (IX+0x%02x)",         0x012 } }, { Bus::Z80::CP_ixd,     { "CP (IX+%02x)",     0x001 } },
  { Bus::Z80::LD_SP_IX,  { "LD SP, IX",              0x001 } }, { Bus::Z80::IxBitInstr, { "",                 0x001 } },
};

static inline const std::unordered_map<uint8_t, MetadataT> ix_bits = {
  { Bus::Z80::RLC_ixd,   { "RLC (IX+0x%02x)",    0x12 } }, { Bus::Z80::RRC_ixd,   { "RRC (IX+%02x)",    0x12 } },
  { Bus::Z80::RL_ixd,    { "RL (IX+0x%02x)",     0x12 } }, { Bus::Z80::RR_ixd,    { "RR (IX+%02x)",     0x12 } },
  { Bus::Z80::SLA_ixd,   { "SLA (IX+0x%02x)",    0x12 } }, { Bus::Z80::SRA_ixd,   { "SRA (IX+%02x)",    0x12 } },
  { Bus::Z80::SLL_ixd,   { "SLL (IX+0x%02x)",    0x12 } }, { Bus::Z80::SRL_ixd,   { "SRL (IX+%02x)",    0x12 } },
  { Bus::Z80::BIT_0_ixd, { "BIT 0, (IX+0x%02x)", 0x12 } }, { Bus::Z80::BIT_1_ixd, { "BIT 1, (IX+%02x)", 0x12 } },
  { Bus::Z80::RES_0_ixd, { "RES 0, (IX+0x%02x)", 0x12 } }, { Bus::Z80::RES_1_ixd, { "RES 1, (IX+%02x)", 0x12 } },
  { Bus::Z80::SET_0_ixd, { "SET 0, (IX+0x%02x)", 0x12 } }, { Bus::Z80::SET_1_ixd, { "SET 1, (IX+%02x)", 0x12 } },
  { Bus::Z80::BIT_2_ixd, { "BIT 2, (IX+0x%02x)", 0x12 } }, { Bus::Z80::BIT_3_ixd, { "BIT 3, (IX+%02x)", 0x12 } },
  { Bus::Z80::RES_2_ixd, { "RES 2, (IX+0x%02x)", 0x12 } }, { Bus::Z80::RES_3_ixd, { "RES 3, (IX+%02x)", 0x12 } },
  { Bus::Z80::SET_2_ixd, { "SET 2, (IX+0x%02x)", 0x12 } }, { Bus::Z80::SET_3_ixd, { "SET 3, (IX+%02x)", 0x12 } },
  { Bus::Z80::BIT_4_ixd, { "BIT 4, (IX+0x%02x)", 0x12 } }, { Bus::Z80::BIT_5_ixd, { "BIT 5, (IX+%02x)", 0x12 } },
  { Bus::Z80::RES_4_ixd, { "RES 4, (IX+0x%02x)", 0x12 } }, { Bus::Z80::RES_5_ixd, { "RES 5, (IX+%02x)", 0x12 } },
  { Bus::Z80::SET_4_ixd, { "SET 4, (IX+0x%02x)", 0x12 } }, { Bus::Z80::SET_5_ixd, { "SET 5, (IX+%02x)", 0x12 } },
  { Bus::Z80::BIT_6_ixd, { "BIT 6, (IX+0x%02x)", 0x12 } }, { Bus::Z80::BIT_7_ixd, { "BIT 7, (IX+%02x)", 0x12 } },
  { Bus::Z80::RES_6_ixd, { "RES 6, (IX+0x%02x)", 0x12 } }, { Bus::Z80::RES_7_ixd, { "RES 7, (IX+%02x)", 0x12 } },
  { Bus::Z80::SET_6_ixd, { "SET 6, (IX+0x%02x)", 0x12 } }, { Bus::Z80::SET_7_ixd, { "SET 7, (IX+%02x)", 0x12 } },
};

static inline const std::unordered_map<uint8_t, MetadataT> iy = {
  { Bus::Z80::ADD_IY_BC, { "ADD IY, BC",             0x001 } }, { Bus::Z80::ADD_IY_DE,  { "ADD IY, DE",       0x001 } },
  { Bus::Z80::INC_iyd,   { "INC (IY+0x%02x)",        0x012 } }, { Bus::Z80::DEC_iyd,    { "DEC (IY+%02x)",    0x012 } },
  { Bus::Z80::LD_iyd_B,  { "LD (IY+0x%02x), B",      0x012 } }, { Bus::Z80::LD_iyd_C,   { "LD (IY+%02x), C",  0x012 } },
  { Bus::Z80::ADD_A_iyd, { "ADD A, (IY+0x%02x)",     0x012 } }, { Bus::Z80::ADC_A_iyd,  { "ADC A, (IY+%02x)", 0x001 } },
  { Bus::Z80::POP_IY,    { "POP IY",                 0x001 } }, { Bus::Z80::EX_sp_IY,   { "EX (SP), IY",      0x001 } },
  { Bus::Z80::LD_IY_NN,  { "LD IY, 0x%04x",          0x023 } }, { Bus::Z80::LD_nn_IY,   { "LD (0x%04x), IY",  0x023 } },
  { Bus::Z80::LD_iyd_N,  { "LD (IY+0x%02x), 0x%04x", 0x113 } }, { Bus::Z80::ADD_IY_SP,  { "ADD IY, SP",       0x001 } },
  { Bus::Z80::LD_iyd_D,  { "LD (IY+0x%02x), D",      0x012 } }, { Bus::Z80::LD_iyd_E,   { "LD (IY+%02x), E",  0x012 } },
  { Bus::Z80::SUB_iyd,   { "SUB (IY+0x%02x)",        0x012 } }, { Bus::Z80::SBC_A_iyd,  { "SBC A, (IY+%02x)", 0x001 } },
  { Bus::Z80::PUSH_IY,   { "PUSH IY",                0x001 } }, { Bus::Z80::JP_iy,      { "JP (IY)",          0x001 } },
  { Bus::Z80::INC_IY,    { "INC IY",                 0x001 } }, { Bus::Z80::ADD_IY_IY,  { "ADD IY, IY",       0x001 } },
  { Bus::Z80::LD_B_iyd,  { "LD B, (IY+0x%02x)",      0x012 } }, { Bus::Z80::LD_C_iyd,   { "LD C, (IY+%02x)",  0x001 } },
  { Bus::Z80::LD_iyd_H,  { "LD (IY+0x%02x), H",      0x012 } }, { Bus::Z80::LD_iyd_L,   { "LD (IY+%02x), L",  0x012 } },
  { Bus::Z80::AND_iyd,   { "AND (IY+0x%02x)",        0x012 } }, { Bus::Z80::XOR_iyd,    { "XOR (IY+%02x)",    0x001 } },
  { Bus::Z80::LD_IY_nn,  { "LD IY, 0x%04x",          0x023 } }, { Bus::Z80::DEC_IY,     { "DEC IY",           0x001 } },
  { Bus::Z80::LD_D_iyd,  { "LD D, (IY+0x%02x)",      0x012 } }, { Bus::Z80::LD_E_iyd,   { "LD E, (IY+%02x)",  0x001 } },
  { Bus::Z80::LD_H_iyd,  { "LD H, (IY+0x%02x)",      0x012 } }, { Bus::Z80::LD_L_iyd,   { "LD L, (IY+%02x)",  0x001 } },
  { Bus::Z80::LD_iyd_A,  { "LD (IY+0x%02x), A",      0x012 } }, { Bus::Z80::LD_A_iyd,   { "LD A, (IY+%02x)",  0x001 } },
  { Bus::Z80::OR_iyd,    { "OR (IY+0x%02x)",         0x012 } }, { Bus::Z80::CP_iyd,     { "CP (IY+%02x)",     0x001 } },
  { Bus::Z80::LD_SP_IY,  { "LD SP, IY",              0x001 } }, { Bus::Z80::IyBitInstr, { "",                 0x001 } },
};

static inline const std::unordered_map<uint8_t, MetadataT> iy_bits = {
  { Bus::Z80::RLC_iyd,   { "RLC (IY+0x%02x)",    0x12 } }, { Bus::Z80::RRC_iyd,   { "RRC (IY+%02x)",    0x12 } },
  { Bus::Z80::RL_iyd,    { "RL (IY+0x%02x)",     0x12 } }, { Bus::Z80::RR_iyd,    { "RR (IY+%02x)",     0x12 } },
  { Bus::Z80::SLA_iyd,   { "SLA (IY+0x%02x)",    0x12 } }, { Bus::Z80::SRA_iyd,   { "SRA (IY+%02x)",    0x12 } },
  { Bus::Z80::SLL_iyd,   { "SLL (IY+0x%02x)",    0x12 } }, { Bus::Z80::SRL_iyd,   { "SRL (IY+%02x)",    0x12 } },
  { Bus::Z80::BIT_0_iyd, { "BIT 0, (IY+0x%02x)", 0x12 } }, { Bus::Z80::BIT_1_iyd, { "BIT 1, (IY+%02x)", 0x12 } },
  { Bus::Z80::RES_0_iyd, { "RES 0, (IY+0x%02x)", 0x12 } }, { Bus::Z80::RES_1_iyd, { "RES 1, (IY+%02x)", 0x12 } },
  { Bus::Z80::SET_0_iyd, { "SET 0, (IY+0x%02x)", 0x12 } }, { Bus::Z80::SET_1_iyd, { "SET 1, (IY+%02x)", 0x12 } },
  { Bus::Z80::BIT_2_iyd, { "BIT 2, (IY+0x%02x)", 0x12 } }, { Bus::Z80::BIT_3_iyd, { "BIT 3, (IY+%02x)", 0x12 } },
  { Bus::Z80::RES_2_iyd, { "RES 2, (IY+0x%02x)", 0x12 } }, { Bus::Z80::RES_3_iyd, { "RES 3, (IY+%02x)", 0x12 } },
  { Bus::Z80::SET_2_iyd, { "SET 2, (IY+0x%02x)", 0x12 } }, { Bus::Z80::SET_3_iyd, { "SET 3, (IY+%02x)", 0x12 } },
  { Bus::Z80::BIT_4_iyd, { "BIT 4, (IY+0x%02x)", 0x12 } }, { Bus::Z80::BIT_5_iyd, { "BIT 5, (IY+%02x)", 0x12 } },
  { Bus::Z80::RES_4_iyd, { "RES 4, (IY+0x%02x)", 0x12 } }, { Bus::Z80::RES_5_iyd, { "RES 5, (IY+%02x)", 0x12 } },
  { Bus::Z80::SET_4_iyd, { "SET 4, (IY+0x%02x)", 0x12 } }, { Bus::Z80::SET_5_iyd, { "SET 5, (IY+%02x)", 0x12 } },
  { Bus::Z80::BIT_6_iyd, { "BIT 6, (IY+0x%02x)", 0x12 } }, { Bus::Z80::BIT_7_iyd, { "BIT 7, (IY+%02x)", 0x12 } },
  { Bus::Z80::RES_6_iyd, { "RES 6, (IY+0x%02x)", 0x12 } }, { Bus::Z80::RES_7_iyd, { "RES 7, (IY+%02x)", 0x12 } },
  { Bus::Z80::SET_6_iyd, { "SET 6, (IY+0x%02x)", 0x12 } }, { Bus::Z80::SET_7_iyd, { "SET 7, (IY+%02x)", 0x12 } },
};


static inline const std::unordered_map<uint8_t, MetadataT> misc = {
  { Bus::Z80::IN_B_c,    { "IN B, (C)",  0x01 } }, { Bus::Z80::OUT_c_B,  { "OUT (C), B",      0x01 } },
  { Bus::Z80::IN_D_c,    { "IN D, (C)",  0x01 } }, { Bus::Z80::OUT_c_D,  { "OUT (C), D",      0x01 } },
  { Bus::Z80::IN_H_c,    { "IN H, (C)",  0x01 } }, { Bus::Z80::OUT_c_H,  { "OUT (C), H",      0x01 } },
  { Bus::Z80::SBC_HL_SP, { "SBC HL, SP", 0x01 } }, { Bus::Z80::LD_nn_DE, { "LD (0x%04x), DE", 0x23 } },
  { Bus::Z80::SBC_HL_BC, { "SBC HL, BC", 0x01 } }, { Bus::Z80::LD_nn_BC, { "LD (0x%04x), BC", 0x23 } },
  { Bus::Z80::SBC_HL_DE, { "SBC HL, DE", 0x01 } }, { Bus::Z80::LD_nn_SP, { "LD (0x%04x), SP", 0x23 } },
  { Bus::Z80::SBC_HL_HL, { "SBC HL, HL", 0x01 } }, { Bus::Z80::RRD,      { "RRD",             0x01 } },
  { Bus::Z80::IN_A_c,    { "IN A, (C)",  0x01 } }, { Bus::Z80::OUT_c_A,  { "OUT (C), A",      0x01 } },
  { Bus::Z80::NEG,       { "NEG",        0x01 } }, { Bus::Z80::RETN,     { "RETN",            0x01 } },
  { Bus::Z80::IM_1,      { "IM 1",       0x01 } }, { Bus::Z80::LD_A_I,   { "LD A, I",         0x01 } },
  { Bus::Z80::IN_L_c,    { "IN L, (C)",  0x01 } }, { Bus::Z80::OUT_c_L,  { "OUT (C), L",      0x01 } },
  { Bus::Z80::ADC_HL_SP, { "ADC HL, SP", 0x01 } }, { Bus::Z80::LD_SP_nn, { "LD SP, (0x%04x)", 0x23 } },
  { Bus::Z80::IM_0,      { "IM 0",       0x01 } }, { Bus::Z80::LD_I_A,   { "LD I, A",         0x01 } },
  { Bus::Z80::IN_E_c,    { "IN E, (C)",  0x01 } }, { Bus::Z80::OUT_c_E,  { "OUT (C), E",      0x01 } },
  { Bus::Z80::ADC_HL_HL, { "ADC HL, HL", 0x01 } }, { Bus::Z80::RLD,      { "RLD",             0x01 } },
  { Bus::Z80::IN_C_c,    { "IN C, (C)",  0x01 } }, { Bus::Z80::OUT_c_C,  { "OUT (C), C",      0x01 } },
  { Bus::Z80::ADC_HL_BC, { "ADC HL, BC", 0x01 } }, { Bus::Z80::LD_BC_nn, { "LD BC, (0x%04x)", 0x23 } },
  { Bus::Z80::RETI,      { "RETI",       0x01 } }, { Bus::Z80::LD_R_A,   { "LD R, A",         0x01 } },
  { Bus::Z80::ADC_HL_DE, { "ADC HL, DE", 0x01 } }, { Bus::Z80::LD_DE_nn, { "LD DE, (0x%04x)", 0x23 } },
  { Bus::Z80::IM_2,      { "IM 2",       0x01 } }, { Bus::Z80::LD_A_R,   { "LD A, R",         0x01 } },
  { Bus::Z80::LDI,       { "LDI",        0x01 } }, { Bus::Z80::CPI,      { "CPI",             0x01 } },
  { Bus::Z80::LDIR,      { "LDIR",       0x01 } }, { Bus::Z80::CPIR,     { "CPIR",            0x01 } },
  { Bus::Z80::INI,       { "INI",        0x01 } }, { Bus::Z80::OUTI,     { "OUTI",            0x01 } },
  { Bus::Z80::INIR,      { "INIR",       0x01 } }, { Bus::Z80::OTIR,     { "OTIR",            0x01 } },
  { Bus::Z80::LDD,       { "LDD",        0x01 } }, { Bus::Z80::CPD,      { "CPD",             0x01 } },
  { Bus::Z80::LDDR,      { "LDDR",       0x01 } }, { Bus::Z80::CPDR,     { "CPDR",            0x01 } },
  { Bus::Z80::IND,       { "IND",        0x01 } }, { Bus::Z80::OUTD,     { "OUTD",            0x01 } },
  { Bus::Z80::INDR,      { "INDR",       0x01 } }, { Bus::Z80::OTDR,     { "OTDR",            0x01 } },
};
};

};

