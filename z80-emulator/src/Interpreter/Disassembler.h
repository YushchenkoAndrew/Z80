#pragma once

#include "src/Bus/Defs.h"
#include <cstdint>

namespace Interpreter {

class Disassembler {
  typedef std::pair<const char*, uint8_t> MetadataT;

public:
  Disassembler() {}

  // Interpreter& load(std::filesystem::path path) { return (preprocessor.scan(path), *this); }

  // bool save(std::filesystem::path path) {
  //   std::ofstream f(path, std::ios::binary);
  //   if (!f) return false;

  //   f.write(reinterpret_cast<const char*>(memory.data()), static_cast<std::streamsize>(memory.size()));
  //   return f.good();
  // }

  // std::unordered_set<std::filesystem::path>& files() { return preprocessor.files; }

  // std::list<std::string> scan() { return scan(preprocessor.source); }
  // std::list<std::string> scan(Bytes& test) {
  //   const auto& err = layout.scan(src);
  //   if (err.size()) return err;

  //   for (auto& stmt : parser.stmt) evaluate(stmt.get());
  //   return {};
  // }

  void scan() {
    // DisassembleT dasm = std::pair("", std::unordered_map<int32_t, int32_t>());
  
    // for (uint32_t i = 0, line = 0; i < 0xFFFF; line++, i++) {
    //   std::pair<std::string, int32_t> cmd;
    //   dasm.second[i] = line;
    //   std::string unknown = "";

    //   switch (uint32_t instruction = Registry::Bus->Read(i, true)) {
    //     case Z80::MISC_INSTR: {
    //       // FIXME: 
    //       // AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
    //       // std::pair<MiscSTR, int32_t> res = foreach<MiscMInstructions, AnyType<-1, int32_t>>::Key2Value();
    //       // if (res.second == -1) unknown = sprintf("db 0xED, 0x%02x", AnyType<-1, int32_t>::GetValue());

    //       // cmd = std::pair(*res.first, res.second); break;
    //     }
        
    //     case Z80::BIT_INSTR: {
    //       // FIXME: 
    //       // AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
    //       // std::pair<BitSTR, int32_t> res = foreach<BitInstructions, AnyType<-1, int32_t>>::Key2Value();
    //       // if (res.second == -1) unknown = sprintf("db 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

    //       // cmd = std::pair(*res.first, res.second); break;
    //     }

    //     case Z80::IX_INSTR: {
    //       // FIXME: 
    //       // instruction = bus->Read(++i, true);

    //       // if (instruction == Z80::IxBitInstr) {
    //       //   AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
    //       //   std::pair<IxBitSTR, int32_t> res = foreach<IxBitInstructions, AnyType<-1, int32_t>>::Key2Value();
    //       //   if (res.second == -1) unknown = sprintf("db 0xDD, 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

    //       //   cmd = std::pair(*res.first, res.second); break;
    //       // } 

    //       // AnyType<-1, int32_t>::GetValue() = instruction;
    //       // std::pair<IxSTR, int32_t> res = foreach<IxInstructions, AnyType<-1, int32_t>>::Key2Value();
    //       // if (res.second == -1) unknown = sprintf("db 0xDD, 0x%02x", AnyType<-1, int32_t>::GetValue());

    //       // cmd = std::pair(*res.first, res.second); break;
    //     }

    //     case Z80::IY_INSTR: {
    //       // FIXME: 
    //       // instruction = bus->Read(++i, true);

    //       // if (instruction == Z80::IyBitInstr) {
    //       //   AnyType<-1, int32_t>::GetValue() = bus->Read(++i, true);
    //       //   std::pair<IyBitSTR, int32_t> res = foreach<IyBitInstructions, AnyType<-1, int32_t>>::Key2Value();
    //       //   if (res.second == -1) unknown = sprintf("db 0xFD, 0xCB, 0x%02x", AnyType<-1, int32_t>::GetValue());

    //       //   cmd = std::pair(*res.first, res.second); break;
    //       // } 

    //       // AnyType<-1, int32_t>::GetValue() = instruction;
    //       // std::pair<IySTR, int32_t> res = foreach<IyInstructions, AnyType<-1, int32_t>>::Key2Value();
    //       // if (res.second == -1) unknown = sprintf("db 0xFD, 0x%02x", AnyType<-1, int32_t>::GetValue());

    //       // cmd = std::pair(*res.first, res.second); break;
    //     }

    //     default: {
    //       // FIXME: 
    //       // AnyType<-1, int32_t>::GetValue() = instruction;
    //       // std::pair<InstrSTR, int32_t> res = foreach<Instructions, AnyType<-1, int32_t>>::Key2Value();
    //       // if (res.second == -1) unknown = sprintf("db 0x%02x", AnyType<-1, int32_t>::GetValue());

    //       // cmd = std::pair(*res.first, res.second); break;
    //     }
    //   }
      

    //   if (unknown.size()) { dasm.first += unknown + "\n"; continue; }

    //   std::string phrase = cmd.first;
    //   for (int32_t cnt = cmd.second & 0x0F, offset = 1; cnt != 1; offset++) {
    //     const int32_t bytes = (cmd.second >> (4 * offset)) & 0x0F;
    //     int32_t word = 0x00; cnt -= bytes;
    //     for (int32_t j = 0; j < bytes; j++) {
    //       word = word | (Registry::Bus->Read(++i, true) << (j * 8));
    //     }
      
    //     phrase = sprintf(phrase, word);
    //   }

    //   dasm.first += phrase + "\n";
    // }

    // return dasm;
  }

  // std::string CPU::sprintf(std::string str, int32_t byte) {
  //   for (int32_t i = 0; i < str.size(); i++) {
  //     if (str[i] != '%' && i + 1 < str.size()) continue;
  //     std::stringstream ss;

  //     int32_t size = 2; int32_t startAt = i;

  //     if (str[i + 1] != 'd' || str[i + 1] != 'c' || str[i + 1] != 'x') {
  //       ss << std::setfill(str[++i]); size++;

  //       if (i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '9') {
  //         ss << std::setw(str[++i] - '0'); size++;
  //       }
  //     }

  //     switch (str[i + 1]) {
  //       case 'd': ss << byte; break;
  //       case 'c': ss << (char)byte; break;
  //       case 'x': ss << std::hex << std::uppercase << byte; break;
  //       default: return str;
  //     }
      
  //     str.replace(startAt, size, ss.str());
  //     return str;
  //   }

  //   return str;
  // }


private:

private:
static inline const std::unordered_map<uint8_t, MetadataT> instructions = {
  { Bus::Z80::NOP,       { "NOP",           0x01 } }, { Bus::Z80::LD_BC_NN,  { "LD BC, 0x%04x", 0x23 } },
  { Bus::Z80::DJNZ_D,    { "DJNZ 0x%02x",   0x12 } }, { Bus::Z80::LD_DE_NN,  { "LD DE, 0x%04x", 0x23 } },
  { Bus::Z80::JR_NZ_D,   { "JR NZ, 0x%02x", 0x12 } }, { Bus::Z80::LD_HL_NN,  { "LD HL, 0x%04x", 0x23 } },
  { Bus::Z80::JR_NC_D,   { "JR NZ, 0x%02x", 0x12 } }, { Bus::Z80::LD_SP_NN,  { "LD SP, 0x%04x", 0x23 } },
  { Bus::Z80::LD_B_B,    { "LD B, B",       0x01 } }, { Bus::Z80::LD_B_C,    { "LD B, C",       0x01 } },

  { Bus::Z80::LD_D_B,    { "LD D, B",         0x01 } }, { Bus::Z80::LD_D_C,    { "LD D, C",         0x01 } },
  { Bus::Z80::LD_H_B,    { "LD H, B",         0x01 } }, { Bus::Z80::LD_H_C,    { "LD H, C",         0x01 } },
  { Bus::Z80::LD_hl_B,   { "LD (HL), B",      0x01 } }, { Bus::Z80::LD_hl_C,   { "LD (HL), C",      0x01 } },
  { Bus::Z80::ADD_A_B,   { "ADD A, B",        0x01 } }, { Bus::Z80::ADD_A_C,   { "ADD A, C",        0x01 } },
  { Bus::Z80::SUB_B,     { "SUB B",           0x01 } }, { Bus::Z80::SUB_C,     { "SUB C",           0x01 } },
  { Bus::Z80::AND_B,     { "AND B",           0x01 } }, { Bus::Z80::AND_C,     { "AND C",           0x01 } },
  { Bus::Z80::OR_B,      { "OR B",            0x01 } }, { Bus::Z80::OR_C,      { "OR C",            0x01 } },
  { Bus::Z80::RET_NZ,    { "RET NZ",          0x01 } }, { Bus::Z80::POP_BC,    { "POP BC",          0x01 } },
  { Bus::Z80::RET_NC,    { "RET NC",          0x01 } }, { Bus::Z80::POP_DE,    { "POP DE",          0x01 } },
  { Bus::Z80::RET_PO,    { "RET PO",          0x01 } }, { Bus::Z80::POP_HL,    { "POP HL",          0x01 } },
  { Bus::Z80::RET_P,     { "RET P",           0x01 } }, { Bus::Z80::POP_AF,    { "POP AF",          0x01 } },
  { Bus::Z80::LD_bc_A,   { "LD (BC), A",      0x01 } }, { Bus::Z80::INC_BC,    { "INC BC",          0x01 } },
  { Bus::Z80::LD_de_A,   { "LD (DE), A",      0x01 } }, { Bus::Z80::INC_DE,    { "INC DE",          0x01 } },
  { Bus::Z80::LD_nn_HL,  { "LD (0x%04x), HL", 0x23 } }, { Bus::Z80::INC_HL,    { "INC HL",          0x01 } },
  { Bus::Z80::LD_nn_A,   { "LD (0x%04x), A",  0x23 } }, { Bus::Z80::INC_SP,    { "INC SP",          0x01 } },
  { Bus::Z80::LD_B_D,    { "LD B, D",         0x01 } }, { Bus::Z80::LD_B_E,    { "LD B, E",         0x01 } },
  { Bus::Z80::LD_D_D,    { "LD D, D",         0x01 } }, { Bus::Z80::LD_D_E,    { "LD D, E",         0x01 } },
  { Bus::Z80::LD_H_D,    { "LD H, D",         0x01 } }, { Bus::Z80::LD_H_E,    { "LD H, E",         0x01 } },
  { Bus::Z80::LD_hl_D,   { "LD (HL), D",      0x01 } }, { Bus::Z80::LD_hl_E,   { "LD (HL), E",      0x01 } },
  { Bus::Z80::ADD_A_D,   { "ADD A, D",        0x01 } }, { Bus::Z80::ADD_A_E,   { "ADD A, E",        0x01 } },
  { Bus::Z80::SUB_D,     { "SUB D",           0x01 } }, { Bus::Z80::SUB_E,     { "SUB E",           0x01 } },
  { Bus::Z80::AND_D,     { "AND D",           0x01 } }, { Bus::Z80::AND_E,     { "AND E",           0x01 } },
  { Bus::Z80::OR_D,      { "OR D",            0x01 } }, { Bus::Z80::OR_E,      { "OR E",            0x01 } },
  { Bus::Z80::JP_NZ_NN,  { "JP NZ, 0x%04x",   0x23 } }, { Bus::Z80::JP_NN,     { "JP 0x%04x",       0x23 } },
  { Bus::Z80::JP_NC_NN,  { "JP NC, 0x%04x",   0x23 } }, { Bus::Z80::OUT_n_A,   { "OUT (0x%02x), A", 0x12 } },
  { Bus::Z80::JP_PO_NN,  { "JP PO, 0x%04x",   0x23 } }, { Bus::Z80::EX_sp_HL,  { "EX (SP), HL",     0x01 } },
  { Bus::Z80::JP_P_NN,   { "JP P, 0x%04x",    0x23 } }, { Bus::Z80::DI,        { "DI",              0x01 } },
  { Bus::Z80::INC_B,     { "INC B",           0x01 } }, { Bus::Z80::DEC_B,     { "DEC B",           0x01 } },
  { Bus::Z80::INC_D,     { "INC D",           0x01 } }, { Bus::Z80::DEC_D,     { "DEC D",           0x01 } },
  { Bus::Z80::INC_H,     { "INC H",           0x01 } }, { Bus::Z80::DEC_H,     { "DEC H",           0x01 } },
  { Bus::Z80::INC_hl,    { "INC (HL)",        0x01 } }, { Bus::Z80::DEC_hl,    { "DEC (HL)",        0x01 } },
  { Bus::Z80::LD_B_H,    { "LD B, H",         0x01 } }, { Bus::Z80::LD_B_L,    { "LD B, L",         0x01 } },
  { Bus::Z80::LD_D_H,    { "LD D, H",         0x01 } }, { Bus::Z80::LD_D_L,    { "LD D, L",         0x01 } },
  { Bus::Z80::LD_H_H,    { "LD H, H",         0x01 } }, { Bus::Z80::LD_H_L,    { "LD H, L",         0x01 } },
  { Bus::Z80::LD_hl_H,   { "LD (HL), H",      0x01 } }, { Bus::Z80::LD_hl_L,   { "LD (HL), L",      0x01 } },
  { Bus::Z80::ADD_A_H,   { "ADD A, H",        0x01 } }, { Bus::Z80::ADD_A_L,   { "ADD A, L",        0x01 } },
  { Bus::Z80::SUB_H,     { "SUB H",           0x01 } }, { Bus::Z80::SUB_L,     { "SUB L",           0x01 } },
  { Bus::Z80::AND_H,     { "AND H",           0x01 } }, { Bus::Z80::AND_L,     { "AND L",           0x01 } },
  { Bus::Z80::OR_H,      { "OR H",            0x01 } }, { Bus::Z80::OR_L,      { "OR L",            0x01 } },
  { Bus::Z80::CALL_NZ_NN,{ "CALL NZ, 0x%04x", 0x23 } }, { Bus::Z80::PUSH_BC,   { "PUSH BC",         0x01 } },
  { Bus::Z80::CALL_NC_NN,{ "CALL NC, 0x%04x", 0x23 } }, { Bus::Z80::PUSH_DE,   { "PUSH DE",         0x01 } },
  { Bus::Z80::CALL_PO_NN,{ "CALL PO, 0x%04x", 0x23 } }, { Bus::Z80::PUSH_HL,   { "PUSH HL",         0x01 } },
  { Bus::Z80::CALL_P_NN, { "CALL P, 0x%04x",  0x23 } }, { Bus::Z80::PUSH_AF,   { "PUSH AF",         0x01 } },
  { Bus::Z80::LD_B_N,    { "LD B, 0x%02x",    0x12 } }, { Bus::Z80::RLCA,      { "RLCA",            0x01 } },
  { Bus::Z80::LD_D_N,    { "LD D, 0x%02x",    0x12 } }, { Bus::Z80::RLA,       { "RLA",             0x01 } },

  { Bus::Z80::LD_H_N,    { "LD H, 0x%02x",      0x12 } }, { Bus::Z80::DAA,       { "DAA",            0x01 } },
  { Bus::Z80::LD_hl_n,   { "LD (HL), 0x%02x",   0x12 } }, { Bus::Z80::SCF,       { "SCF",            0x01 } },
  { Bus::Z80::LD_B_hl,   { "LD B, (HL)",        0x01 } }, { Bus::Z80::LD_B_A,    { "LD B, A",        0x01 } },
  { Bus::Z80::LD_D_hl,   { "LD D, (HL)",        0x01 } }, { Bus::Z80::LD_D_A,    { "LD D, A",        0x01 } },
  { Bus::Z80::LD_H_hl,   { "LD H, (HL)",        0x01 } }, { Bus::Z80::LD_H_A,    { "LD H, A",        0x01 } },
  { Bus::Z80::HALT,      { "HALT",              0x01 } }, { Bus::Z80::LD_hl_A,   { "LD (HL), A",     0x01 } },
  { Bus::Z80::ADD_A_hl,  { "ADD A, (HL)",       0x01 } }, { Bus::Z80::ADD_A_A,   { "ADD A, A",       0x01 } },
  { Bus::Z80::SUB_hl,    { "SUB (HL)",          0x01 } }, { Bus::Z80::SUB_A,     { "SUB A",          0x01 } },
  { Bus::Z80::AND_hl,    { "AND (HL)",          0x01 } }, { Bus::Z80::AND_A,     { "AND A",          0x01 } },
  { Bus::Z80::OR_hl,     { "OR (HL)",           0x01 } }, { Bus::Z80::OR_A,      { "OR A",           0x01 } },
  { Bus::Z80::ADD_A_N,   { "ADD A, 0x%02x",     0x12 } }, { Bus::Z80::RST_00h,   { "RST 0x00",       0x01 } },
  { Bus::Z80::SUB_N,     { "SUB 0x%02x",        0x12 } }, { Bus::Z80::RST_10h,   { "RST 0x10",       0x01 } },
  { Bus::Z80::AND_N,     { "AND 0x%02x",        0x12 } }, { Bus::Z80::RST_20h,   { "RST 0x20",       0x01 } },
  { Bus::Z80::OR_N,      { "OR 0x%02x",         0x12 } }, { Bus::Z80::RST_30h,   { "RST 0x30",       0x01 } },
  { Bus::Z80::EX_AF_AF,  { "EX AF, AF'",        0x01 } }, { Bus::Z80::ADD_HL_BC, { "ADD HL, BC",     0x01 } },
  { Bus::Z80::JR_D,      { "JR 0x%02x",         0x12 } }, { Bus::Z80::ADD_HL_DE, { "ADD HL, DE",     0x01 } },
  { Bus::Z80::JR_Z_D,    { "JR Z, 0x%02x",      0x12 } }, { Bus::Z80::ADD_HL_HL, { "ADD HL, HL",     0x01 } },
  { Bus::Z80::JR_C_D,    { "JR C, 0x%02x",      0x12 } }, { Bus::Z80::ADD_HL_SP, { "ADD HL, SP",     0x01 } },
  { Bus::Z80::LD_C_B,    { "LD C, B",           0x01 } }, { Bus::Z80::LD_C_C,    { "LD C, C",        0x01 } },
  { Bus::Z80::LD_E_B,    { "LD E, B",           0x01 } }, { Bus::Z80::LD_E_C,    { "LD E, C",        0x01 } },
  { Bus::Z80::LD_L_B,    { "LD L, B",           0x01 } }, { Bus::Z80::LD_L_C,    { "LD L, C",        0x01 } },
  { Bus::Z80::LD_A_B,    { "LD A, B",           0x01 } }, { Bus::Z80::LD_A_C,    { "LD A, C",        0x01 } },
  { Bus::Z80::ADC_A_B,   { "ADC A, B",          0x01 } }, { Bus::Z80::ADC_A_C,   { "ADC A, C",       0x01 } },
  { Bus::Z80::SBC_A_B,   { "SBC A, B",          0x01 } }, { Bus::Z80::SBC_A_C,   { "SBC A, C",       0x01 } },
  { Bus::Z80::XOR_B,     { "XOR B",             0x01 } }, { Bus::Z80::XOR_C,     { "XOR C",          0x01 } },
  { Bus::Z80::CP_B,      { "CP B",              0x01 } }, { Bus::Z80::CP_C,      { "CP C",           0x01 } },
  { Bus::Z80::RET_Z,     { "RET Z",             0x01 } }, { Bus::Z80::RET,       { "RET",            0x01 } },
  { Bus::Z80::RET_C,     { "RET C",             0x01 } }, { Bus::Z80::EXX,       { "EXX",            0x01 } },
  { Bus::Z80::RET_PE,    { "RET PE",            0x01 } }, { Bus::Z80::JP_hl,     { "JP (HL)",        0x01 } },
  { Bus::Z80::RET_M,     { "RET M",             0x01 } }, { Bus::Z80::LD_SP_HL,  { "LD SP, HL",      0x01 } },
  { Bus::Z80::LD_A_bc,   { "LD A, (BC)",        0x01 } }, { Bus::Z80::DEC_BC,    { "DEC BC",         0x01 } },
  { Bus::Z80::LD_A_de,   { "LD A, (DE)",        0x01 } }, { Bus::Z80::DEC_DE,    { "DEC DE",         0x01 } },
  { Bus::Z80::LD_HL_nn,  { "LD HL, (0x%04x)",   0x23 } }, { Bus::Z80::DEC_HL,    { "DEC HL",         0x01 } },
  { Bus::Z80::LD_A_nn,   { "LD A, (0x%04x)",    0x23 } }, { Bus::Z80::DEC_SP,    { "DEC SP",         0x01 } },
  { Bus::Z80::LD_C_D,    { "LD C, D",           0x01 } }, { Bus::Z80::LD_C_E,    { "LD C, E",        0x01 } },
  { Bus::Z80::LD_E_D,    { "LD E, D",           0x01 } }, { Bus::Z80::LD_E_E,    { "LD E, E",        0x01 } },
  { Bus::Z80::LD_L_D,    { "LD L, D",           0x01 } }, { Bus::Z80::LD_L_E,    { "LD L, E",        0x01 } },
  { Bus::Z80::LD_A_D,    { "LD A, D",           0x01 } }, { Bus::Z80::LD_A_E,    { "LD A, E",        0x01 } },
  { Bus::Z80::ADC_A_D,   { "ADC A, D",          0x01 } }, { Bus::Z80::ADC_A_E,   { "ADC A, E",       0x01 } },
  { Bus::Z80::SBC_A_D,   { "SBC A, D",          0x01 } }, { Bus::Z80::SBC_A_E,   { "SBC A, E",       0x01 } },
  { Bus::Z80::XOR_D,     { "XOR D",             0x01 } }, { Bus::Z80::XOR_E,     { "XOR E",          0x01 } },
  { Bus::Z80::CP_D,      { "CP D",              0x01 } }, { Bus::Z80::CP_E,      { "CP E",           0x01 } },
  { Bus::Z80::JP_Z_NN,   { "JP Z, 0x%04x",      0x23 } }, { Bus::Z80::BIT_INSTR, { "",               0x01 } },
  { Bus::Z80::JP_C_NN,   { "JP C, 0x%04x",      0x23 } }, { Bus::Z80::IN_A_n,    { "IN A, (0x%02x)", 0x12 } },
  { Bus::Z80::JP_PE_NN,  { "JP PE, 0x%04x",     0x23 } }, { Bus::Z80::EX_DE_HL,  { "EX DE, HL",      0x01 } },
  { Bus::Z80::JP_M_NN,   { "JP M, 0x%04x",      0x23 } }, { Bus::Z80::EI,        { "EI",             0x01 } },
  { Bus::Z80::INC_C,     { "INC C",             0x01 } }, { Bus::Z80::DEC_C,     { "DEC C",          0x01 } },
  { Bus::Z80::INC_E,     { "INC E",             0x01 } }, { Bus::Z80::DEC_E,     { "DEC E",          0x01 } },
  { Bus::Z80::INC_L,     { "INC L",             0x01 } }, { Bus::Z80::DEC_L,     { "DEC L",          0x01 } },
  { Bus::Z80::INC_A,     { "INC A",             0x01 } }, { Bus::Z80::DEC_A,     { "DEC A",          0x01 } },
  { Bus::Z80::LD_C_H,    { "LD C, H",           0x01 } }, { Bus::Z80::LD_C_L,    { "LD C, L",        0x01 } },
  { Bus::Z80::LD_E_H,    { "LD E, H",           0x01 } }, { Bus::Z80::LD_E_L,    { "LD E, L",        0x01 } },
  { Bus::Z80::LD_L_H,    { "LD L, H",           0x01 } }, { Bus::Z80::LD_L_L,    { "LD L, L",        0x01 } },
  { Bus::Z80::LD_A_H,    { "LD A, H",           0x01 } }, { Bus::Z80::LD_A_L,    { "LD A, L",        0x01 } },
  { Bus::Z80::ADC_A_H,   { "ADC A, H",          0x01 } }, { Bus::Z80::ADC_A_L,   { "ADC A, L",       0x01 } },
  { Bus::Z80::SBC_A_H,   { "SBC A, H",          0x01 } }, { Bus::Z80::SBC_A_L,   { "SBC A, L",       0x01 } },
  { Bus::Z80::XOR_H,     { "XOR H",             0x01 } }, { Bus::Z80::XOR_L,     { "XOR L",          0x01 } },
  { Bus::Z80::CP_H,      { "CP H",              0x01 } }, { Bus::Z80::CP_L,      { "CP L",           0x01 } },
  { Bus::Z80::CALL_Z_NN, { "CALL Z, 0x%04x",    0x23 } }, { Bus::Z80::CALL_NN,   { "CALL 0x%04x",    0x23 } },
  { Bus::Z80::CALL_C_NN, { "CALL C, 0x%04x",    0x23 } }, { Bus::Z80::IX_INSTR,  { "",               0x01 } },
  { Bus::Z80::CALL_PE_NN,{ "CALL PE, 0x%04x",   0x23 } }, { Bus::Z80::MISC_INSTR,{ "",               0x01 } },
  { Bus::Z80::CALL_M_NN, { "CALL M, 0x%04x",    0x23 } }, { Bus::Z80::IY_INSTR,  { "",               0x01 } },
  { Bus::Z80::LD_C_N,    { "LD C, 0x%02x",      0x12 } }, { Bus::Z80::RRCA,      { "RRCA",           0x01 } },
  { Bus::Z80::LD_E_N,    { "LD E, 0x%02x",      0x12 } }, { Bus::Z80::RRA,       { "RRA",            0x01 } },
  { Bus::Z80::LD_L_N,    { "LD L, 0x%02x",      0x12 } }, { Bus::Z80::CPL,       { "CPL",            0x01 } },
  { Bus::Z80::LD_A_N,    { "LD A, 0x%02x",      0x12 } }, { Bus::Z80::CCF,       { "CCF",            0x01 } },
  { Bus::Z80::LD_C_hl,   { "LD C, (HL)",        0x01 } }, { Bus::Z80::LD_C_A,    { "LD C, A",        0x01 } },
  { Bus::Z80::LD_E_hl,   { "LD E, (HL)",        0x01 } }, { Bus::Z80::LD_E_A,    { "LD E, A",        0x01 } },
  { Bus::Z80::LD_L_hl,   { "LD L, (HL)",        0x01 } }, { Bus::Z80::LD_L_A,    { "LD L, A",        0x01 } },
  { Bus::Z80::LD_A_hl,   { "LD A, (HL)",        0x01 } }, { Bus::Z80::LD_A_A,    { "LD A, A",        0x01 } },
  { Bus::Z80::ADC_A_hl,  { "ADC A, (HL)",       0x01 } }, { Bus::Z80::ADC_A_A,   { "ADC A, A",       0x01 } },
  { Bus::Z80::SBC_A_hl,  { "SBC A, (HL)",       0x01 } }, { Bus::Z80::SBC_A_A,   { "SBC A, A",       0x01 } },
  { Bus::Z80::XOR_hl,    { "XOR (HL)",          0x01 } }, { Bus::Z80::XOR_A,     { "XOR A",          0x01 } },
  { Bus::Z80::CP_hl,     { "CP (HL)",           0x01 } }, { Bus::Z80::CP_A,      { "CP A",           0x01 } },
  { Bus::Z80::ADC_A_N,   { "ADC A, 0x%02x",     0x12 } }, { Bus::Z80::RST_08h,   { "RST 0x08",       0x01 } },
  { Bus::Z80::SBC_A_N,   { "SBC A, 0x%02x",     0x12 } }, { Bus::Z80::RST_18h,   { "RST 0x18",       0x01 } },
  { Bus::Z80::XOR_N,     { "XOR 0x%02x",        0x12 } }, { Bus::Z80::RST_28h,   { "RST 0x28",       0x01 } },
  { Bus::Z80::CP_N,      { "CP 0x%02x",         0x12 } }, { Bus::Z80::RST_38h,   { "RST 0x38",       0x01 } }
};

    // // BitInstructions
    // AnyType<Z80::RLC_B,   Z80::BitSTR>::GetValue() = "RLC B";        AnyType<Z80::RLC_C,   Z80::BitSTR>::GetValue() = "RLC C";
    // AnyType<Z80::RL_B,    Z80::BitSTR>::GetValue() = "RL B";         AnyType<Z80::RL_C,    Z80::BitSTR>::GetValue() = "RL C";
    // AnyType<Z80::SLA_B,   Z80::BitSTR>::GetValue() = "SLA B";        AnyType<Z80::SLA_C,   Z80::BitSTR>::GetValue() = "SLA C";
    // AnyType<Z80::SLL_B,   Z80::BitSTR>::GetValue() = "SLL B";        AnyType<Z80::SLL_C,   Z80::BitSTR>::GetValue() = "SLL C";
    // AnyType<Z80::BIT_0_B, Z80::BitSTR>::GetValue() = "BIT 0, B";     AnyType<Z80::BIT_0_C, Z80::BitSTR>::GetValue() = "BIT 0, C";
    // AnyType<Z80::BIT_2_B, Z80::BitSTR>::GetValue() = "BIT 2, B";     AnyType<Z80::BIT_2_C, Z80::BitSTR>::GetValue() = "BIT 2, C";
    // AnyType<Z80::BIT_4_B, Z80::BitSTR>::GetValue() = "BIT 4, B";     AnyType<Z80::BIT_4_C, Z80::BitSTR>::GetValue() = "BIT 4, C";
    // AnyType<Z80::BIT_6_B, Z80::BitSTR>::GetValue() = "BIT 6, B";     AnyType<Z80::BIT_6_C, Z80::BitSTR>::GetValue() = "BIT 6, C";
    // AnyType<Z80::RES_0_B, Z80::BitSTR>::GetValue() = "RES 0, B";     AnyType<Z80::RES_0_C, Z80::BitSTR>::GetValue() = "RES 0, C";
    // AnyType<Z80::RES_2_B, Z80::BitSTR>::GetValue() = "RES 2, B";     AnyType<Z80::RES_2_C, Z80::BitSTR>::GetValue() = "RES 2, C";
    // AnyType<Z80::RES_4_B, Z80::BitSTR>::GetValue() = "RES 4, B";     AnyType<Z80::RES_4_C, Z80::BitSTR>::GetValue() = "RES 4, C";
    // AnyType<Z80::RES_6_B, Z80::BitSTR>::GetValue() = "RES 6, B";     AnyType<Z80::RES_6_C, Z80::BitSTR>::GetValue() = "RES 6, C";
    // AnyType<Z80::SET_0_B, Z80::BitSTR>::GetValue() = "SET 0, B";     AnyType<Z80::SET_0_C, Z80::BitSTR>::GetValue() = "SET 0, C";
    // AnyType<Z80::SET_2_B, Z80::BitSTR>::GetValue() = "SET 2, B";     AnyType<Z80::SET_2_C, Z80::BitSTR>::GetValue() = "SET 2, C";
    // AnyType<Z80::SET_4_B, Z80::BitSTR>::GetValue() = "SET 4, B";     AnyType<Z80::SET_4_C, Z80::BitSTR>::GetValue() = "SET 4, C";
    // AnyType<Z80::SET_6_B, Z80::BitSTR>::GetValue() = "SET 6, B";     AnyType<Z80::SET_6_C, Z80::BitSTR>::GetValue() = "SET 6, C";


    // AnyType<Z80::RLC_D,   Z80::BitSTR>::GetValue() = "RLC D";          AnyType<Z80::RLC_E,   Z80::BitSTR>::GetValue() = "RLC E";
    // AnyType<Z80::RL_D,    Z80::BitSTR>::GetValue() = "RL D";           AnyType<Z80::RL_E,    Z80::BitSTR>::GetValue() = "RL E";
    // AnyType<Z80::SLA_D,   Z80::BitSTR>::GetValue() = "SLA D";          AnyType<Z80::SLA_E,   Z80::BitSTR>::GetValue() = "SLA E";
    // AnyType<Z80::SLL_D,   Z80::BitSTR>::GetValue() = "SLL D";          AnyType<Z80::SLL_E,   Z80::BitSTR>::GetValue() = "SLL E";
    // AnyType<Z80::BIT_0_D, Z80::BitSTR>::GetValue() = "BIT 0, D";       AnyType<Z80::BIT_0_E, Z80::BitSTR>::GetValue() = "BIT 0, E";
    // AnyType<Z80::BIT_2_D, Z80::BitSTR>::GetValue() = "BIT 2, D";       AnyType<Z80::BIT_2_E, Z80::BitSTR>::GetValue() = "BIT 2, E";
    // AnyType<Z80::BIT_4_D, Z80::BitSTR>::GetValue() = "BIT 4, D";       AnyType<Z80::BIT_4_E, Z80::BitSTR>::GetValue() = "BIT 4, E";
    // AnyType<Z80::BIT_6_D, Z80::BitSTR>::GetValue() = "BIT 6, D";       AnyType<Z80::BIT_6_E, Z80::BitSTR>::GetValue() = "BIT 6, E";
    // AnyType<Z80::RES_0_D, Z80::BitSTR>::GetValue() = "RES 0, D";       AnyType<Z80::RES_0_E, Z80::BitSTR>::GetValue() = "RES 0, E";
    // AnyType<Z80::RES_2_D, Z80::BitSTR>::GetValue() = "RES 2, D";       AnyType<Z80::RES_2_E, Z80::BitSTR>::GetValue() = "RES 2, E";
    // AnyType<Z80::RES_4_D, Z80::BitSTR>::GetValue() = "RES 4, D";       AnyType<Z80::RES_4_E, Z80::BitSTR>::GetValue() = "RES 4, E";
    // AnyType<Z80::RES_6_D, Z80::BitSTR>::GetValue() = "RES 6, D";       AnyType<Z80::RES_6_E, Z80::BitSTR>::GetValue() = "RES 6, E";
    // AnyType<Z80::SET_0_D, Z80::BitSTR>::GetValue() = "SET 0, D";       AnyType<Z80::SET_0_E, Z80::BitSTR>::GetValue() = "SET 0, E";
    // AnyType<Z80::SET_2_D, Z80::BitSTR>::GetValue() = "SET 2, D";       AnyType<Z80::SET_2_E, Z80::BitSTR>::GetValue() = "SET 2, E";
    // AnyType<Z80::SET_4_D, Z80::BitSTR>::GetValue() = "SET 4, D";       AnyType<Z80::SET_4_E, Z80::BitSTR>::GetValue() = "SET 4, E";
    // AnyType<Z80::SET_6_D, Z80::BitSTR>::GetValue() = "SET 6, D";       AnyType<Z80::SET_6_E, Z80::BitSTR>::GetValue() = "SET 6, E";


    // AnyType<Z80::RLC_H,   Z80::BitSTR>::GetValue() = "RLC H";          AnyType<Z80::RLC_L,   Z80::BitSTR>::GetValue() = "RLC L";
    // AnyType<Z80::RL_H,    Z80::BitSTR>::GetValue() = "RL H";           AnyType<Z80::RL_L,    Z80::BitSTR>::GetValue() = "RL L";
    // AnyType<Z80::SLA_H,   Z80::BitSTR>::GetValue() = "SLA H";          AnyType<Z80::SLA_L,   Z80::BitSTR>::GetValue() = "SLA L";
    // AnyType<Z80::SLL_H,   Z80::BitSTR>::GetValue() = "SLL H";          AnyType<Z80::SLL_L,   Z80::BitSTR>::GetValue() = "SLL L";
    // AnyType<Z80::BIT_0_H, Z80::BitSTR>::GetValue() = "BIT 0, H";       AnyType<Z80::BIT_0_L, Z80::BitSTR>::GetValue() = "BIT 0, L";
    // AnyType<Z80::BIT_2_H, Z80::BitSTR>::GetValue() = "BIT 2, H";       AnyType<Z80::BIT_2_L, Z80::BitSTR>::GetValue() = "BIT 2, L";
    // AnyType<Z80::BIT_4_H, Z80::BitSTR>::GetValue() = "BIT 4, H";       AnyType<Z80::BIT_4_L, Z80::BitSTR>::GetValue() = "BIT 4, L";
    // AnyType<Z80::BIT_6_H, Z80::BitSTR>::GetValue() = "BIT 6, H";       AnyType<Z80::BIT_6_L, Z80::BitSTR>::GetValue() = "BIT 6, L";
    // AnyType<Z80::RES_0_H, Z80::BitSTR>::GetValue() = "RES 0, H";       AnyType<Z80::RES_0_L, Z80::BitSTR>::GetValue() = "RES 0, L";
    // AnyType<Z80::RES_2_H, Z80::BitSTR>::GetValue() = "RES 2, H";       AnyType<Z80::RES_2_L, Z80::BitSTR>::GetValue() = "RES 2, L";
    // AnyType<Z80::RES_4_H, Z80::BitSTR>::GetValue() = "RES 4, H";       AnyType<Z80::RES_4_L, Z80::BitSTR>::GetValue() = "RES 4, L";
    // AnyType<Z80::RES_6_H, Z80::BitSTR>::GetValue() = "RES 6, H";       AnyType<Z80::RES_6_L, Z80::BitSTR>::GetValue() = "RES 6, L";
    // AnyType<Z80::SET_0_H, Z80::BitSTR>::GetValue() = "SET 0, H";       AnyType<Z80::SET_0_L, Z80::BitSTR>::GetValue() = "SET 0, L";
    // AnyType<Z80::SET_2_H, Z80::BitSTR>::GetValue() = "SET 2, H";       AnyType<Z80::SET_2_L, Z80::BitSTR>::GetValue() = "SET 2, L";
    // AnyType<Z80::SET_4_H, Z80::BitSTR>::GetValue() = "SET 4, H";       AnyType<Z80::SET_4_L, Z80::BitSTR>::GetValue() = "SET 4, L";
    // AnyType<Z80::SET_6_H, Z80::BitSTR>::GetValue() = "SET 6, H";       AnyType<Z80::SET_6_L, Z80::BitSTR>::GetValue() = "SET 6, L";


    // AnyType<Z80::RLC_hl,   Z80::BitSTR>::GetValue() = "RLC (HL)";      AnyType<Z80::RLC_A,   Z80::BitSTR>::GetValue() = "RLC A";
    // AnyType<Z80::RL_hl,    Z80::BitSTR>::GetValue() = "RL (HL)";       AnyType<Z80::RL_A,    Z80::BitSTR>::GetValue() = "RL A";
    // AnyType<Z80::SLA_hl,   Z80::BitSTR>::GetValue() = "SLA (HL)";      AnyType<Z80::SLA_A,   Z80::BitSTR>::GetValue() = "SLA A";
    // AnyType<Z80::SLL_hl,   Z80::BitSTR>::GetValue() = "SLL (HL)";      AnyType<Z80::SLL_A,   Z80::BitSTR>::GetValue() = "SLL A";
    // AnyType<Z80::BIT_0_hl, Z80::BitSTR>::GetValue() = "BIT 0, (HL)";   AnyType<Z80::BIT_0_A, Z80::BitSTR>::GetValue() = "BIT 0, A";
    // AnyType<Z80::BIT_2_hl, Z80::BitSTR>::GetValue() = "BIT 2, (HL)";   AnyType<Z80::BIT_2_A, Z80::BitSTR>::GetValue() = "BIT 2, A";
    // AnyType<Z80::BIT_4_hl, Z80::BitSTR>::GetValue() = "BIT 4, (HL)";   AnyType<Z80::BIT_4_A, Z80::BitSTR>::GetValue() = "BIT 4, A";
    // AnyType<Z80::BIT_6_hl, Z80::BitSTR>::GetValue() = "BIT 6, (HL)";   AnyType<Z80::BIT_6_A, Z80::BitSTR>::GetValue() = "BIT 6, A";
    // AnyType<Z80::RES_0_hl, Z80::BitSTR>::GetValue() = "RES 0, (HL)";   AnyType<Z80::RES_0_A, Z80::BitSTR>::GetValue() = "RES 0, A";
    // AnyType<Z80::RES_2_hl, Z80::BitSTR>::GetValue() = "RES 2, (HL)";   AnyType<Z80::RES_2_A, Z80::BitSTR>::GetValue() = "RES 2, A";
    // AnyType<Z80::RES_4_hl, Z80::BitSTR>::GetValue() = "RES 4, (HL)";   AnyType<Z80::RES_4_A, Z80::BitSTR>::GetValue() = "RES 4, A";
    // AnyType<Z80::RES_6_hl, Z80::BitSTR>::GetValue() = "RES 6, (HL)";   AnyType<Z80::RES_6_A, Z80::BitSTR>::GetValue() = "RES 6, A";
    // AnyType<Z80::SET_0_hl, Z80::BitSTR>::GetValue() = "SET 0, (HL)";   AnyType<Z80::SET_0_A, Z80::BitSTR>::GetValue() = "SET 0, A";
    // AnyType<Z80::SET_2_hl, Z80::BitSTR>::GetValue() = "SET 2, (HL)";   AnyType<Z80::SET_2_A, Z80::BitSTR>::GetValue() = "SET 2, A";
    // AnyType<Z80::SET_4_hl, Z80::BitSTR>::GetValue() = "SET 4, (HL)";   AnyType<Z80::SET_4_A, Z80::BitSTR>::GetValue() = "SET 4, A";
    // AnyType<Z80::SET_6_hl, Z80::BitSTR>::GetValue() = "SET 6, (HL)";   AnyType<Z80::SET_6_A, Z80::BitSTR>::GetValue() = "SET 6, A";


    // AnyType<Z80::RRC_B,   Z80::BitSTR>::GetValue() = "RRC B";          AnyType<Z80::RRC_C,   Z80::BitSTR>::GetValue() = "RRC C";
    // AnyType<Z80::RR_B,    Z80::BitSTR>::GetValue() = "RR B";           AnyType<Z80::RR_C,    Z80::BitSTR>::GetValue() = "RR C";
    // AnyType<Z80::SRA_B,   Z80::BitSTR>::GetValue() = "SRA B";          AnyType<Z80::SRA_C,   Z80::BitSTR>::GetValue() = "SRA C";
    // AnyType<Z80::SRL_B,   Z80::BitSTR>::GetValue() = "SRL B";          AnyType<Z80::SRL_C,   Z80::BitSTR>::GetValue() = "SRL C";
    // AnyType<Z80::BIT_1_B, Z80::BitSTR>::GetValue() = "BIT 1, B";       AnyType<Z80::BIT_1_C, Z80::BitSTR>::GetValue() = "BIT 1, C";
    // AnyType<Z80::BIT_3_B, Z80::BitSTR>::GetValue() = "BIT 3, B";       AnyType<Z80::BIT_3_C, Z80::BitSTR>::GetValue() = "BIT 3, C";
    // AnyType<Z80::BIT_5_B, Z80::BitSTR>::GetValue() = "BIT 5, B";       AnyType<Z80::BIT_5_C, Z80::BitSTR>::GetValue() = "BIT 5, C";
    // AnyType<Z80::BIT_7_B, Z80::BitSTR>::GetValue() = "BIT 7, B";       AnyType<Z80::BIT_7_C, Z80::BitSTR>::GetValue() = "BIT 7, C";
    // AnyType<Z80::RES_1_B, Z80::BitSTR>::GetValue() = "RES 1, B";       AnyType<Z80::RES_1_C, Z80::BitSTR>::GetValue() = "RES 1, C";
    // AnyType<Z80::RES_3_B, Z80::BitSTR>::GetValue() = "RES 3, B";       AnyType<Z80::RES_3_C, Z80::BitSTR>::GetValue() = "RES 3, C";
    // AnyType<Z80::RES_5_B, Z80::BitSTR>::GetValue() = "RES 5, B";       AnyType<Z80::RES_5_C, Z80::BitSTR>::GetValue() = "RES 5, C";
    // AnyType<Z80::RES_7_B, Z80::BitSTR>::GetValue() = "RES 7, B";       AnyType<Z80::RES_7_C, Z80::BitSTR>::GetValue() = "RES 7, C";
    // AnyType<Z80::SET_1_B, Z80::BitSTR>::GetValue() = "SET 1, B";       AnyType<Z80::SET_1_C, Z80::BitSTR>::GetValue() = "SET 1, C";
    // AnyType<Z80::SET_3_B, Z80::BitSTR>::GetValue() = "SET 3, B";       AnyType<Z80::SET_3_C, Z80::BitSTR>::GetValue() = "SET 3, C";
    // AnyType<Z80::SET_5_B, Z80::BitSTR>::GetValue() = "SET 5, B";       AnyType<Z80::SET_5_C, Z80::BitSTR>::GetValue() = "SET 5, C";
    // AnyType<Z80::SET_7_B, Z80::BitSTR>::GetValue() = "SET 7, B";       AnyType<Z80::SET_7_C, Z80::BitSTR>::GetValue() = "SET 7, C";


    // AnyType<Z80::RRC_D,   Z80::BitSTR>::GetValue() = "RRC D";          AnyType<Z80::RRC_E,   Z80::BitSTR>::GetValue() = "RRC E";
    // AnyType<Z80::RR_D,    Z80::BitSTR>::GetValue() = "RR D";           AnyType<Z80::RR_E,    Z80::BitSTR>::GetValue() = "RR E";
    // AnyType<Z80::SRA_D,   Z80::BitSTR>::GetValue() = "SRA D";          AnyType<Z80::SRA_E,   Z80::BitSTR>::GetValue() = "SRA E";
    // AnyType<Z80::SRL_D,   Z80::BitSTR>::GetValue() = "SRL D";          AnyType<Z80::SRL_E,   Z80::BitSTR>::GetValue() = "SRL E";
    // AnyType<Z80::BIT_1_D, Z80::BitSTR>::GetValue() = "BIT 1, D";       AnyType<Z80::BIT_1_E, Z80::BitSTR>::GetValue() = "BIT 1, E";
    // AnyType<Z80::BIT_3_D, Z80::BitSTR>::GetValue() = "BIT 3, D";       AnyType<Z80::BIT_3_E, Z80::BitSTR>::GetValue() = "BIT 3, E";
    // AnyType<Z80::BIT_5_D, Z80::BitSTR>::GetValue() = "BIT 5, D";       AnyType<Z80::BIT_5_E, Z80::BitSTR>::GetValue() = "BIT 5, E";
    // AnyType<Z80::BIT_7_D, Z80::BitSTR>::GetValue() = "BIT 7, D";       AnyType<Z80::BIT_7_E, Z80::BitSTR>::GetValue() = "BIT 7, E";
    // AnyType<Z80::RES_1_D, Z80::BitSTR>::GetValue() = "RES 1, D";       AnyType<Z80::RES_1_E, Z80::BitSTR>::GetValue() = "RES 1, E";
    // AnyType<Z80::RES_3_D, Z80::BitSTR>::GetValue() = "RES 3, D";       AnyType<Z80::RES_3_E, Z80::BitSTR>::GetValue() = "RES 3, E";
    // AnyType<Z80::RES_5_D, Z80::BitSTR>::GetValue() = "RES 5, D";       AnyType<Z80::RES_5_E, Z80::BitSTR>::GetValue() = "RES 5, E";
    // AnyType<Z80::RES_7_D, Z80::BitSTR>::GetValue() = "RES 7, D";       AnyType<Z80::RES_7_E, Z80::BitSTR>::GetValue() = "RES 7, E";
    // AnyType<Z80::SET_1_D, Z80::BitSTR>::GetValue() = "SET 1, D";       AnyType<Z80::SET_1_E, Z80::BitSTR>::GetValue() = "SET 1, E";
    // AnyType<Z80::SET_3_D, Z80::BitSTR>::GetValue() = "SET 3, D";       AnyType<Z80::SET_3_E, Z80::BitSTR>::GetValue() = "SET 3, E";
    // AnyType<Z80::SET_5_D, Z80::BitSTR>::GetValue() = "SET 5, D";       AnyType<Z80::SET_5_E, Z80::BitSTR>::GetValue() = "SET 5, E";
    // AnyType<Z80::SET_7_D, Z80::BitSTR>::GetValue() = "SET 7, D";       AnyType<Z80::SET_7_E, Z80::BitSTR>::GetValue() = "SET 7, E";

    // AnyType<Z80::RRC_H,   Z80::BitSTR>::GetValue() = "RRC H";          AnyType<Z80::RRC_L,   Z80::BitSTR>::GetValue() = "RRC L";
    // AnyType<Z80::RR_H,    Z80::BitSTR>::GetValue() = "RR H";           AnyType<Z80::RR_L,    Z80::BitSTR>::GetValue() = "RR L";
    // AnyType<Z80::SRA_H,   Z80::BitSTR>::GetValue() = "SRA H";          AnyType<Z80::SRA_L,   Z80::BitSTR>::GetValue() = "SRA L";
    // AnyType<Z80::SRL_H,   Z80::BitSTR>::GetValue() = "SRL H";          AnyType<Z80::SRL_L,   Z80::BitSTR>::GetValue() = "SRL L";
    // AnyType<Z80::BIT_1_H, Z80::BitSTR>::GetValue() = "BIT 1, H";       AnyType<Z80::BIT_1_L, Z80::BitSTR>::GetValue() = "BIT 1, L";
    // AnyType<Z80::BIT_3_H, Z80::BitSTR>::GetValue() = "BIT 3, H";       AnyType<Z80::BIT_3_L, Z80::BitSTR>::GetValue() = "BIT 3, L";
    // AnyType<Z80::BIT_5_H, Z80::BitSTR>::GetValue() = "BIT 5, H";       AnyType<Z80::BIT_5_L, Z80::BitSTR>::GetValue() = "BIT 5, L";
    // AnyType<Z80::BIT_7_H, Z80::BitSTR>::GetValue() = "BIT 7, H";       AnyType<Z80::BIT_7_L, Z80::BitSTR>::GetValue() = "BIT 7, L";
    // AnyType<Z80::RES_1_H, Z80::BitSTR>::GetValue() = "RES 1, H";       AnyType<Z80::RES_1_L, Z80::BitSTR>::GetValue() = "RES 1, L";
    // AnyType<Z80::RES_3_H, Z80::BitSTR>::GetValue() = "RES 3, H";       AnyType<Z80::RES_3_L, Z80::BitSTR>::GetValue() = "RES 3, L";
    // AnyType<Z80::RES_5_H, Z80::BitSTR>::GetValue() = "RES 5, H";       AnyType<Z80::RES_5_L, Z80::BitSTR>::GetValue() = "RES 5, L";
    // AnyType<Z80::RES_7_H, Z80::BitSTR>::GetValue() = "RES 7, H";       AnyType<Z80::RES_7_L, Z80::BitSTR>::GetValue() = "RES 7, L";
    // AnyType<Z80::SET_1_H, Z80::BitSTR>::GetValue() = "SET 1, H";       AnyType<Z80::SET_1_L, Z80::BitSTR>::GetValue() = "SET 1, L";
    // AnyType<Z80::SET_3_H, Z80::BitSTR>::GetValue() = "SET 3, H";       AnyType<Z80::SET_3_L, Z80::BitSTR>::GetValue() = "SET 3, L";
    // AnyType<Z80::SET_5_H, Z80::BitSTR>::GetValue() = "SET 5, H";       AnyType<Z80::SET_5_L, Z80::BitSTR>::GetValue() = "SET 5, L";
    // AnyType<Z80::SET_7_H, Z80::BitSTR>::GetValue() = "SET 7, H";       AnyType<Z80::SET_7_L, Z80::BitSTR>::GetValue() = "SET 7, L";


    // AnyType<Z80::RRC_hl,   Z80::BitSTR>::GetValue() = "RRC (HL)";      AnyType<Z80::RRC_A,   Z80::BitSTR>::GetValue() = "RRC A";
    // AnyType<Z80::RR_hl,    Z80::BitSTR>::GetValue() = "RR (HL)";       AnyType<Z80::RR_A,    Z80::BitSTR>::GetValue() = "RR A";
    // AnyType<Z80::SRA_hl,   Z80::BitSTR>::GetValue() = "SRA (HL)";      AnyType<Z80::SRA_A,   Z80::BitSTR>::GetValue() = "SRA A";
    // AnyType<Z80::SRL_hl,   Z80::BitSTR>::GetValue() = "SRL (HL)";      AnyType<Z80::SRL_A,   Z80::BitSTR>::GetValue() = "SRL A";
    // AnyType<Z80::BIT_1_hl, Z80::BitSTR>::GetValue() = "BIT 1, (HL)";   AnyType<Z80::BIT_1_A, Z80::BitSTR>::GetValue() = "BIT 1, A";
    // AnyType<Z80::BIT_3_hl, Z80::BitSTR>::GetValue() = "BIT 3, (HL)";   AnyType<Z80::BIT_3_A, Z80::BitSTR>::GetValue() = "BIT 3, A";
    // AnyType<Z80::BIT_5_hl, Z80::BitSTR>::GetValue() = "BIT 5, (HL)";   AnyType<Z80::BIT_5_A, Z80::BitSTR>::GetValue() = "BIT 5, A";
    // AnyType<Z80::BIT_7_hl, Z80::BitSTR>::GetValue() = "BIT 7, (HL)";   AnyType<Z80::BIT_7_A, Z80::BitSTR>::GetValue() = "BIT 7, A";
    // AnyType<Z80::RES_1_hl, Z80::BitSTR>::GetValue() = "RES 1, (HL)";   AnyType<Z80::RES_1_A, Z80::BitSTR>::GetValue() = "RES 1, A";
    // AnyType<Z80::RES_3_hl, Z80::BitSTR>::GetValue() = "RES 3, (HL)";   AnyType<Z80::RES_3_A, Z80::BitSTR>::GetValue() = "RES 3, A";
    // AnyType<Z80::RES_5_hl, Z80::BitSTR>::GetValue() = "RES 5, (HL)";   AnyType<Z80::RES_5_A, Z80::BitSTR>::GetValue() = "RES 5, A";
    // AnyType<Z80::RES_7_hl, Z80::BitSTR>::GetValue() = "RES 7, (HL)";   AnyType<Z80::RES_7_A, Z80::BitSTR>::GetValue() = "RES 7, A";
    // AnyType<Z80::SET_1_hl, Z80::BitSTR>::GetValue() = "SET 1, (HL)";   AnyType<Z80::SET_1_A, Z80::BitSTR>::GetValue() = "SET 1, A";
    // AnyType<Z80::SET_3_hl, Z80::BitSTR>::GetValue() = "SET 3, (HL)";   AnyType<Z80::SET_3_A, Z80::BitSTR>::GetValue() = "SET 3, A";
    // AnyType<Z80::SET_5_hl, Z80::BitSTR>::GetValue() = "SET 5, (HL)";   AnyType<Z80::SET_5_A, Z80::BitSTR>::GetValue() = "SET 5, A";
    // AnyType<Z80::SET_7_hl, Z80::BitSTR>::GetValue() = "SET 7, (HL)";   AnyType<Z80::SET_7_A, Z80::BitSTR>::GetValue() = "SET 7, A";


    // // IxInstructions
    // AnyType<Z80::ADD_IX_BC, Z80::IxSTR>::GetValue() = "ADD IX, BC";             AnyType<Z80::ADD_IX_DE, Z80::IxSTR>::GetValue() = "ADD IX, DE";
    // AnyType<Z80::INC_ixd,   Z80::IxSTR>::GetValue() = "INC (IX+0x%02x)";        AnyType<Z80::DEC_ixd,   Z80::IxSTR>::GetValue() = "DEC (IX+%02x)";
    // AnyType<Z80::LD_ixd_B,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), B";      AnyType<Z80::LD_ixd_C,  Z80::IxSTR>::GetValue() = "LD (IX+%02x), C";
    // AnyType<Z80::ADD_A_ixd, Z80::IxSTR>::GetValue() = "ADD A, (IX+0x%02x)";     AnyType<Z80::ADC_A_ixd, Z80::IxSTR>::GetValue() = "ADC A, (IX+%02x)";
    // AnyType<Z80::POP_IX,    Z80::IxSTR>::GetValue() = "POP IX";                 AnyType<Z80::EX_sp_IX,  Z80::IxSTR>::GetValue() = "EX (SP), IX";
    // AnyType<Z80::LD_IX_NN,  Z80::IxSTR>::GetValue() = "LD IX, 0x%04x";          AnyType<Z80::LD_nn_IX,  Z80::IxSTR>::GetValue() = "LD (0x%04x), IX";
    // AnyType<Z80::LD_ixd_N,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), 0x%04x"; AnyType<Z80::ADD_IX_SP, Z80::IxSTR>::GetValue() = "ADD IX, SP";
    // AnyType<Z80::LD_ixd_D,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), D";      AnyType<Z80::LD_ixd_E,  Z80::IxSTR>::GetValue() = "LD (IX+%02x), E";
    // AnyType<Z80::SUB_ixd,   Z80::IxSTR>::GetValue() = "SUB (IX+0x%02x)";        AnyType<Z80::SBC_A_ixd, Z80::IxSTR>::GetValue() = "SBC A, (IX+%02x)";
    // AnyType<Z80::PUSH_IX,   Z80::IxSTR>::GetValue() = "PUSH IX";                AnyType<Z80::JP_ix,     Z80::IxSTR>::GetValue() = "JP (IX)";
    // AnyType<Z80::INC_IX,    Z80::IxSTR>::GetValue() = "INC IX";                 AnyType<Z80::ADD_IX_IX, Z80::IxSTR>::GetValue() = "ADD IX, IX";
    // AnyType<Z80::LD_B_ixd,  Z80::IxSTR>::GetValue() = "LD B, (IX+0x%02x)";      AnyType<Z80::LD_C_ixd,  Z80::IxSTR>::GetValue() = "LD C, (IX+%02x)";
    // AnyType<Z80::LD_ixd_H,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), H";      AnyType<Z80::LD_ixd_L,  Z80::IxSTR>::GetValue() = "LD (IX+%02x), L";
    // AnyType<Z80::AND_ixd,   Z80::IxSTR>::GetValue() = "AND (IX+0x%02x)";        AnyType<Z80::XOR_ixd,   Z80::IxSTR>::GetValue() = "XOR (IX+%02x)";
    // AnyType<Z80::LD_IX_nn,  Z80::IxSTR>::GetValue() = "LD IX, 0x%04x";          AnyType<Z80::DEC_IX,    Z80::IxSTR>::GetValue() = "DEC IX";
    // AnyType<Z80::LD_D_ixd,  Z80::IxSTR>::GetValue() = "LD D, (IX+0x%02x)";      AnyType<Z80::LD_E_ixd,  Z80::IxSTR>::GetValue() = "LD E, (IX+%02x)";
    // AnyType<Z80::LD_H_ixd,  Z80::IxSTR>::GetValue() = "LD H, (IX+0x%02x)";      AnyType<Z80::LD_L_ixd,  Z80::IxSTR>::GetValue() = "LD L, (IX+%02x)";
    // AnyType<Z80::LD_ixd_A,  Z80::IxSTR>::GetValue() = "LD (IX+0x%02x), A";      AnyType<Z80::LD_A_ixd,  Z80::IxSTR>::GetValue() = "LD A, (IX+%02x)";
    // AnyType<Z80::OR_ixd,    Z80::IxSTR>::GetValue() = "OR (IX+0x%02x)";         AnyType<Z80::CP_ixd,    Z80::IxSTR>::GetValue() = "CP (IX+%02x)";
    // AnyType<Z80::LD_SP_IX,  Z80::IxSTR>::GetValue() = "LD SP, IX";              AnyType<Z80::IxBitInstr,Z80::IxSTR>::GetValue() = "";


    // AnyType<Z80::RLC_ixd,   Z80::IxBitSTR>::GetValue() = "RLC (IX+0x%02x)";     AnyType<Z80::RRC_ixd,   Z80::IxBitSTR>::GetValue() = "RRC (IX+%02x)";
    // AnyType<Z80::RL_ixd,    Z80::IxBitSTR>::GetValue() = "RL (IX+0x%02x)";      AnyType<Z80::RR_ixd,    Z80::IxBitSTR>::GetValue() = "RR (IX+%02x)";
    // AnyType<Z80::SLA_ixd,   Z80::IxBitSTR>::GetValue() = "SLA (IX+0x%02x)";     AnyType<Z80::SRA_ixd,   Z80::IxBitSTR>::GetValue() = "SRA (IX+%02x)";
    // AnyType<Z80::SLL_ixd,   Z80::IxBitSTR>::GetValue() = "SLL (IX+0x%02x)";     AnyType<Z80::SRL_ixd,   Z80::IxBitSTR>::GetValue() = "SRL (IX+%02x)";
    // AnyType<Z80::BIT_0_ixd, Z80::IxBitSTR>::GetValue() = "BIT 0, (IX+0x%02x)";  AnyType<Z80::BIT_1_ixd, Z80::IxBitSTR>::GetValue() = "BIT 1, (IX+%02x)";
    // AnyType<Z80::RES_0_ixd, Z80::IxBitSTR>::GetValue() = "RES 0, (IX+0x%02x)";  AnyType<Z80::RES_1_ixd, Z80::IxBitSTR>::GetValue() = "RES 1, (IX+%02x)";
    // AnyType<Z80::SET_0_ixd, Z80::IxBitSTR>::GetValue() = "SET 0, (IX+0x%02x)";  AnyType<Z80::SET_1_ixd, Z80::IxBitSTR>::GetValue() = "SET 1, (IX+%02x)";
    // AnyType<Z80::BIT_2_ixd, Z80::IxBitSTR>::GetValue() = "BIT 2, (IX+0x%02x)";  AnyType<Z80::BIT_3_ixd, Z80::IxBitSTR>::GetValue() = "BIT 3, (IX+%02x)";
    // AnyType<Z80::RES_2_ixd, Z80::IxBitSTR>::GetValue() = "RES 2, (IX+0x%02x)";  AnyType<Z80::RES_3_ixd, Z80::IxBitSTR>::GetValue() = "RES 3, (IX+%02x)";
    // AnyType<Z80::SET_2_ixd, Z80::IxBitSTR>::GetValue() = "SET 2, (IX+0x%02x)";  AnyType<Z80::SET_3_ixd, Z80::IxBitSTR>::GetValue() = "SET 3, (IX+%02x)";
    // AnyType<Z80::BIT_4_ixd, Z80::IxBitSTR>::GetValue() = "BIT 4, (IX+0x%02x)";  AnyType<Z80::BIT_5_ixd, Z80::IxBitSTR>::GetValue() = "BIT 5, (IX+%02x)";
    // AnyType<Z80::RES_4_ixd, Z80::IxBitSTR>::GetValue() = "RES 4, (IX+0x%02x)";  AnyType<Z80::RES_5_ixd, Z80::IxBitSTR>::GetValue() = "RES 5, (IX+%02x)";
    // AnyType<Z80::SET_4_ixd, Z80::IxBitSTR>::GetValue() = "SET 4, (IX+0x%02x)";  AnyType<Z80::SET_5_ixd, Z80::IxBitSTR>::GetValue() = "SET 5, (IX+%02x)";
    // AnyType<Z80::BIT_6_ixd, Z80::IxBitSTR>::GetValue() = "BIT 6, (IX+0x%02x)";  AnyType<Z80::BIT_7_ixd, Z80::IxBitSTR>::GetValue() = "BIT 7, (IX+%02x)";
    // AnyType<Z80::RES_6_ixd, Z80::IxBitSTR>::GetValue() = "RES 6, (IX+0x%02x)";  AnyType<Z80::RES_7_ixd, Z80::IxBitSTR>::GetValue() = "RES 7, (IX+%02x)";
    // AnyType<Z80::SET_6_ixd, Z80::IxBitSTR>::GetValue() = "SET 6, (IX+0x%02x)";  AnyType<Z80::SET_7_ixd, Z80::IxBitSTR>::GetValue() = "SET 7, (IX+%02x)";


    // // IyInstructions
    // AnyType<Z80::ADD_IY_BC, Z80::IySTR>::GetValue() = "ADD IY, BC";             AnyType<Z80::ADD_IY_DE, Z80::IySTR>::GetValue() = "ADD IY, DE";
    // AnyType<Z80::INC_iyd,   Z80::IySTR>::GetValue() = "INC (IY+0x%02x)";        AnyType<Z80::DEC_iyd,   Z80::IySTR>::GetValue() = "DEC (IY+%02x)";
    // AnyType<Z80::LD_iyd_B,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), B";      AnyType<Z80::LD_iyd_C,  Z80::IySTR>::GetValue() = "LD (IY+%02x), C";
    // AnyType<Z80::ADD_A_iyd, Z80::IySTR>::GetValue() = "ADD A, (IY+0x%02x)";     AnyType<Z80::ADC_A_iyd, Z80::IySTR>::GetValue() = "ADC A, (IY+%02x)";
    // AnyType<Z80::POP_IY,    Z80::IySTR>::GetValue() = "POP IY";                 AnyType<Z80::EX_sp_IY,  Z80::IySTR>::GetValue() = "EX (SP), IY";
    // AnyType<Z80::LD_IY_NN,  Z80::IySTR>::GetValue() = "LD IY, 0x%04x";          AnyType<Z80::LD_nn_IY,  Z80::IySTR>::GetValue() = "LD (0x%04x), IY";
    // AnyType<Z80::LD_iyd_N,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), 0x%04x"; AnyType<Z80::ADD_IY_SP, Z80::IySTR>::GetValue() = "ADD IY, SP";
    // AnyType<Z80::LD_iyd_D,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), D";      AnyType<Z80::LD_iyd_E,  Z80::IySTR>::GetValue() = "LD (IY+%02x), E";
    // AnyType<Z80::SUB_iyd,   Z80::IySTR>::GetValue() = "SUB (IY+0x%02x)";        AnyType<Z80::SBC_A_iyd, Z80::IySTR>::GetValue() = "SBC A, (IY+%02x)";
    // AnyType<Z80::PUSH_IY,   Z80::IySTR>::GetValue() = "PUSH IY";                AnyType<Z80::JP_iy,     Z80::IySTR>::GetValue() = "JP (IY)";
    // AnyType<Z80::INC_IY,    Z80::IySTR>::GetValue() = "INC IY";                 AnyType<Z80::ADD_IY_IY, Z80::IySTR>::GetValue() = "ADD IY, IY";
    // AnyType<Z80::LD_B_iyd,  Z80::IySTR>::GetValue() = "LD B, (IY+0x%02x)";      AnyType<Z80::LD_C_iyd,  Z80::IySTR>::GetValue() = "LD C, (IY+%02x)";
    // AnyType<Z80::LD_iyd_H,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), H";      AnyType<Z80::LD_iyd_L,  Z80::IySTR>::GetValue() = "LD (IY+%02x), L";
    // AnyType<Z80::AND_iyd,   Z80::IySTR>::GetValue() = "AND (IY+0x%02x)";        AnyType<Z80::XOR_iyd,   Z80::IySTR>::GetValue() = "XOR (IY+%02x)";
    // AnyType<Z80::LD_IY_nn,  Z80::IySTR>::GetValue() = "LD IY, 0x%04x";          AnyType<Z80::DEC_IY,    Z80::IySTR>::GetValue() = "DEC IY";
    // AnyType<Z80::LD_D_iyd,  Z80::IySTR>::GetValue() = "LD D, (IY+0x%02x)";      AnyType<Z80::LD_E_iyd,  Z80::IySTR>::GetValue() = "LD E, (IY+%02x)";
    // AnyType<Z80::LD_H_iyd,  Z80::IySTR>::GetValue() = "LD H, (IY+0x%02x)";      AnyType<Z80::LD_L_iyd,  Z80::IySTR>::GetValue() = "LD L, (IY+%02x)";
    // AnyType<Z80::LD_iyd_A,  Z80::IySTR>::GetValue() = "LD (IY+0x%02x), A";      AnyType<Z80::LD_A_iyd,  Z80::IySTR>::GetValue() = "LD A, (IY+%02x)";
    // AnyType<Z80::OR_iyd,    Z80::IySTR>::GetValue() = "OR (IY+0x%02x)";         AnyType<Z80::CP_iyd,    Z80::IySTR>::GetValue() = "CP (IY+%02x)";
    // AnyType<Z80::LD_SP_IY,  Z80::IySTR>::GetValue() = "LD SP, IY";              AnyType<Z80::IyBitInstr,Z80::IySTR>::GetValue() = "";

    // AnyType<Z80::RLC_iyd,   Z80::IyBitSTR>::GetValue() = "RLC (IY+0x%02x)";     AnyType<Z80::RRC_iyd,   Z80::IyBitSTR>::GetValue() = "RRC (IY+%02x)";
    // AnyType<Z80::RL_iyd,    Z80::IyBitSTR>::GetValue() = "RL (IY+0x%02x)";      AnyType<Z80::RR_iyd,    Z80::IyBitSTR>::GetValue() = "RR (IY+%02x)";
    // AnyType<Z80::SLA_iyd,   Z80::IyBitSTR>::GetValue() = "SLA (IY+0x%02x)";     AnyType<Z80::SRA_iyd,   Z80::IyBitSTR>::GetValue() = "SRA (IY+%02x)";
    // AnyType<Z80::SLL_iyd,   Z80::IyBitSTR>::GetValue() = "SLL (IY+0x%02x)";     AnyType<Z80::SRL_iyd,   Z80::IyBitSTR>::GetValue() = "SRL (IY+%02x)";
    // AnyType<Z80::BIT_0_iyd, Z80::IyBitSTR>::GetValue() = "BIT 0, (IY+0x%02x)";  AnyType<Z80::BIT_1_iyd, Z80::IyBitSTR>::GetValue() = "BIT 1, (IY+%02x)";
    // AnyType<Z80::RES_0_iyd, Z80::IyBitSTR>::GetValue() = "RES 0, (IY+0x%02x)";  AnyType<Z80::RES_1_iyd, Z80::IyBitSTR>::GetValue() = "RES 1, (IY+%02x)";
    // AnyType<Z80::SET_0_iyd, Z80::IyBitSTR>::GetValue() = "SET 0, (IY+0x%02x)";  AnyType<Z80::SET_1_iyd, Z80::IyBitSTR>::GetValue() = "SET 1, (IY+%02x)";
    // AnyType<Z80::BIT_2_iyd, Z80::IyBitSTR>::GetValue() = "BIT 2, (IY+0x%02x)";  AnyType<Z80::BIT_3_iyd, Z80::IyBitSTR>::GetValue() = "BIT 3, (IY+%02x)";
    // AnyType<Z80::RES_2_iyd, Z80::IyBitSTR>::GetValue() = "RES 2, (IY+0x%02x)";  AnyType<Z80::RES_3_iyd, Z80::IyBitSTR>::GetValue() = "RES 3, (IY+%02x)";
    // AnyType<Z80::SET_2_iyd, Z80::IyBitSTR>::GetValue() = "SET 2, (IY+0x%02x)";  AnyType<Z80::SET_3_iyd, Z80::IyBitSTR>::GetValue() = "SET 3, (IY+%02x)";
    // AnyType<Z80::BIT_4_iyd, Z80::IyBitSTR>::GetValue() = "BIT 4, (IY+0x%02x)";  AnyType<Z80::BIT_5_iyd, Z80::IyBitSTR>::GetValue() = "BIT 5, (IY+%02x)";
    // AnyType<Z80::RES_4_iyd, Z80::IyBitSTR>::GetValue() = "RES 4, (IY+0x%02x)";  AnyType<Z80::RES_5_iyd, Z80::IyBitSTR>::GetValue() = "RES 5, (IY+%02x)";
    // AnyType<Z80::SET_4_iyd, Z80::IyBitSTR>::GetValue() = "SET 4, (IY+0x%02x)";  AnyType<Z80::SET_5_iyd, Z80::IyBitSTR>::GetValue() = "SET 5, (IY+%02x)";
    // AnyType<Z80::BIT_6_iyd, Z80::IyBitSTR>::GetValue() = "BIT 6, (IY+0x%02x)";  AnyType<Z80::BIT_7_iyd, Z80::IyBitSTR>::GetValue() = "BIT 7, (IY+%02x)";
    // AnyType<Z80::RES_6_iyd, Z80::IyBitSTR>::GetValue() = "RES 6, (IY+0x%02x)";  AnyType<Z80::RES_7_iyd, Z80::IyBitSTR>::GetValue() = "RES 7, (IY+%02x)";
    // AnyType<Z80::SET_6_iyd, Z80::IyBitSTR>::GetValue() = "SET 6, (IY+0x%02x)";  AnyType<Z80::SET_7_iyd, Z80::IyBitSTR>::GetValue() = "SET 7, (IY+%02x)";

    

    // // Misc Instruction
    // AnyType<Z80::IN_B_c,    Z80::MiscSTR>::GetValue() = "IN B, (C)";  AnyType<Z80::OUT_c_B,  Z80::MiscSTR>::GetValue() = "OUT (C), B";
    // AnyType<Z80::IN_D_c,    Z80::MiscSTR>::GetValue() = "IN D, (C)";  AnyType<Z80::OUT_c_D,  Z80::MiscSTR>::GetValue() = "OUT (C), D";
    // AnyType<Z80::IN_H_c,    Z80::MiscSTR>::GetValue() = "IN H, (C)";  AnyType<Z80::OUT_c_H,  Z80::MiscSTR>::GetValue() = "OUT (C), H";
    // AnyType<Z80::SBC_HL_SP, Z80::MiscSTR>::GetValue() = "SBC HL, SP"; AnyType<Z80::LD_nn_DE, Z80::MiscSTR>::GetValue() = "LD (0x%04x), DE";
    // AnyType<Z80::SBC_HL_BC, Z80::MiscSTR>::GetValue() = "SBC HL, BC"; AnyType<Z80::LD_nn_BC, Z80::MiscSTR>::GetValue() = "LD (0x%04x), BC";
    // AnyType<Z80::SBC_HL_DE, Z80::MiscSTR>::GetValue() = "SBC HL, DE"; AnyType<Z80::LD_nn_SP, Z80::MiscSTR>::GetValue() = "LD (0x%04x), SP";
    // AnyType<Z80::SBC_HL_HL, Z80::MiscSTR>::GetValue() = "SBC HL, HL"; AnyType<Z80::RRD,      Z80::MiscSTR>::GetValue() = "RRD";
    // AnyType<Z80::IN_A_c,    Z80::MiscSTR>::GetValue() = "IN A, (C)";  AnyType<Z80::OUT_c_A,  Z80::MiscSTR>::GetValue() = "OUT (C), A";
    // AnyType<Z80::NEG,       Z80::MiscSTR>::GetValue() = "NEG";        AnyType<Z80::RETN,     Z80::MiscSTR>::GetValue() = "RETN";
    // AnyType<Z80::IM_1,      Z80::MiscSTR>::GetValue() = "IM 1";       AnyType<Z80::LD_A_I,   Z80::MiscSTR>::GetValue() = "LD A, I";
    // AnyType<Z80::IN_L_c,    Z80::MiscSTR>::GetValue() = "IN L, (C)";  AnyType<Z80::OUT_c_L,  Z80::MiscSTR>::GetValue() = "OUT (C), L";
    // AnyType<Z80::ADC_HL_SP, Z80::MiscSTR>::GetValue() = "ADC HL, SP"; AnyType<Z80::LD_SP_nn, Z80::MiscSTR>::GetValue() = "LD SP, (0x%04x)";
    // AnyType<Z80::IM_0,      Z80::MiscSTR>::GetValue() = "IM 0";       AnyType<Z80::LD_I_A,   Z80::MiscSTR>::GetValue() = "LD I, A";
    // AnyType<Z80::IN_E_c,    Z80::MiscSTR>::GetValue() = "IN E, (C)";  AnyType<Z80::OUT_c_E,  Z80::MiscSTR>::GetValue() = "OUT (C), E";
    // AnyType<Z80::ADC_HL_HL, Z80::MiscSTR>::GetValue() = "ADC HL, HL"; AnyType<Z80::RLD,      Z80::MiscSTR>::GetValue() = "RLD";
    // AnyType<Z80::IN_C_c,    Z80::MiscSTR>::GetValue() = "IN C, (C)";  AnyType<Z80::OUT_c_C,  Z80::MiscSTR>::GetValue() = "OUT (C), C";
    // AnyType<Z80::ADC_HL_BC, Z80::MiscSTR>::GetValue() = "ADC HL, BC"; AnyType<Z80::LD_BC_nn, Z80::MiscSTR>::GetValue() = "LD BC, (0x%04x)";
    // AnyType<Z80::RETI,      Z80::MiscSTR>::GetValue() = "RETI";       AnyType<Z80::LD_R_A,   Z80::MiscSTR>::GetValue() = "LD R, A";
    // AnyType<Z80::ADC_HL_DE, Z80::MiscSTR>::GetValue() = "ADC HL, DE"; AnyType<Z80::LD_DE_nn, Z80::MiscSTR>::GetValue() = "LD DE, (0x%04x)";
    // AnyType<Z80::IM_2,      Z80::MiscSTR>::GetValue() = "IM 2";       AnyType<Z80::LD_A_R,   Z80::MiscSTR>::GetValue() = "LD A, R";
    // AnyType<Z80::LDI,       Z80::MiscSTR>::GetValue() = "LDI";        AnyType<Z80::CPI,      Z80::MiscSTR>::GetValue() = "CPI";
    // AnyType<Z80::LDIR,      Z80::MiscSTR>::GetValue() = "LDIR";       AnyType<Z80::CPIR,     Z80::MiscSTR>::GetValue() = "CPIR";
    // AnyType<Z80::INI,       Z80::MiscSTR>::GetValue() = "INI";        AnyType<Z80::OUTI,     Z80::MiscSTR>::GetValue() = "OUTI";
    // AnyType<Z80::INIR,      Z80::MiscSTR>::GetValue() = "INIR";       AnyType<Z80::OTIR,     Z80::MiscSTR>::GetValue() = "OTIR";
    // AnyType<Z80::LDD,       Z80::MiscSTR>::GetValue() = "LDD";        AnyType<Z80::CPD,      Z80::MiscSTR>::GetValue() = "CPD";
    // AnyType<Z80::LDDR,      Z80::MiscSTR>::GetValue() = "LDDR";       AnyType<Z80::CPDR,     Z80::MiscSTR>::GetValue() = "CPDR";
    // AnyType<Z80::IND,       Z80::MiscSTR>::GetValue() = "IND";        AnyType<Z80::OUTD,     Z80::MiscSTR>::GetValue() = "OUTD";
    // AnyType<Z80::INDR,      Z80::MiscSTR>::GetValue() = "INDR";       AnyType<Z80::OTDR,     Z80::MiscSTR>::GetValue() = "OTDR";

};

};

