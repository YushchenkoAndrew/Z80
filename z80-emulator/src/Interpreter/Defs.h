#pragma once
#include <stdio.h>
#include <string>
#include <memory>
#include "include/Typelist.h"
#include "include/Foreach.h"
#include "lib/olcPixelGameEngine.h"

namespace Interpreter {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */
enum TokenT {
  // Single char tokens
  LEFT_BRACE = 1, RIGHT_BRACE, COMMA, COLON, PLUS, MINUS,
  
  // Bit operation in assignment
  BIT_OR, BIT_AND, BIT_XOR, BIT_NOT,

  // Literals
  IDENTIFIER, STRING, NUMBER, 

  // Registers
  REG_AF,  REG_BC,  REG_DE,  REG_HL, REG_IX, REG_IY, REG_SP, REG_PC,
  REG_AF$, REG_BC$, REG_DE$, REG_HL$,

  REG_A,  REG_B,  REG_C,  REG_D,  REG_E,  REG_H,  REG_L, REG_I, REG_R, REG_F,
  REG_A$, REG_B$, REG_C$, REG_D$, REG_E$, REG_H$, REG_L$,

  // Flags
  FLAG_C, FLAG_M, FLAG_NC, FLAG_NZ, FLAG_P, FLAG_PE, FLAG_PO, FLAG_Z,

  // Command words
  CMD_ADC,  CMD_ADD,  CMD_AND,  CMD_BIT,  CMD_CALL,  CMD_CCF,  CMD_CP,   CMD_CPD,  CMD_CPDR,  CMD_CPI, 
  CMD_CPIR, CMD_CPL,  CMD_DAA,  CMD_DEC,  CMD_DI,    CMD_DJNZ, CMD_EI,   CMD_EX,   CMD_EXX,   CMD_HALT,
  CMD_IM,   CMD_IN,   CMD_INC,  CMD_IND,  CMD_INDR,  CMD_INI,  CMD_INIR, CMD_JP,   CMD_JR,    CMD_LD,
  CMD_LDD,  CMD_LDDR, CMD_LDI,  CMD_LDIR, CMD_NEG,   CMD_NOP,  CMD_OR,   CMD_OTDR, CMD_OTIR,  CMD_OUT, 
  CMD_OUTD, CMD_OUTI, CMD_POP,  CMD_PUSH, CMD_RES,   CMD_RET,  CMD_RETI, CMD_RETN, CMD_RL,    CMD_RLA, 
  CMD_RLC,  CMD_RLCA, CMD_RLD,  CMD_RR,   CMD_RRA,   CMD_RRC,  CMD_RRCA, CMD_RRD,  CMD_RST,   CMD_SBC, 
  CMD_SCF,  CMD_SET,  CMD_SLA,  CMD_SRA,  CMD_SRL,  CMD_SUB,   CMD_XOR,

  // Operations
  OP_ORG, OP_DB, OP_EQU,

  OP_EOF, OP_NONE
};


typedef TypeList<
  AnyType<TokenT::OP_ORG,   std::string>, TypeList<
  AnyType<TokenT::OP_DB,    std::string>, TypeList<
  AnyType<TokenT::OP_EQU,   std::string>, TypeList<
  AnyType<TokenT::CMD_ADC,  std::string>, TypeList<
  AnyType<TokenT::CMD_ADD,  std::string>, TypeList<
  AnyType<TokenT::CMD_AND,  std::string>, TypeList<
  AnyType<TokenT::CMD_BIT,  std::string>, TypeList<
  AnyType<TokenT::CMD_CALL, std::string>, TypeList<
  AnyType<TokenT::CMD_CCF,  std::string>, TypeList<
  AnyType<TokenT::CMD_CP,   std::string>, TypeList<
  AnyType<TokenT::CMD_CPD,  std::string>, TypeList<
  AnyType<TokenT::CMD_CPDR, std::string>, TypeList<
  AnyType<TokenT::CMD_CPI,  std::string>, TypeList<
  AnyType<TokenT::CMD_CPIR, std::string>, TypeList<
  AnyType<TokenT::CMD_CPL,  std::string>, TypeList<
  AnyType<TokenT::CMD_DAA,  std::string>, TypeList<
  AnyType<TokenT::CMD_DEC,  std::string>, TypeList<
  AnyType<TokenT::CMD_DI,   std::string>, TypeList<
  AnyType<TokenT::CMD_DJNZ, std::string>, TypeList<
  AnyType<TokenT::CMD_EI,   std::string>, TypeList<
  AnyType<TokenT::CMD_EX,   std::string>, TypeList<
  AnyType<TokenT::CMD_EXX,  std::string>, TypeList<
  AnyType<TokenT::CMD_HALT, std::string>, TypeList<
  AnyType<TokenT::CMD_IM,   std::string>, TypeList<
  AnyType<TokenT::CMD_IN,   std::string>, TypeList<
  AnyType<TokenT::CMD_INC,  std::string>, TypeList<
  AnyType<TokenT::CMD_IND,  std::string>, TypeList<
  AnyType<TokenT::CMD_INDR, std::string>, TypeList<
  AnyType<TokenT::CMD_INI,  std::string>, TypeList<
  AnyType<TokenT::CMD_INIR, std::string>, TypeList<
  AnyType<TokenT::CMD_JP,   std::string>, TypeList<
  AnyType<TokenT::CMD_JR,   std::string>, TypeList<
  AnyType<TokenT::CMD_LD,   std::string>, TypeList<
  AnyType<TokenT::CMD_LDD,  std::string>, TypeList<
  AnyType<TokenT::CMD_LDDR, std::string>, TypeList<
  AnyType<TokenT::CMD_LDI,  std::string>, TypeList<
  AnyType<TokenT::CMD_LDIR, std::string>, TypeList<
  AnyType<TokenT::CMD_NEG,  std::string>, TypeList<
  AnyType<TokenT::CMD_NOP,  std::string>, TypeList<
  AnyType<TokenT::CMD_OR,   std::string>, TypeList<
  AnyType<TokenT::CMD_OTDR, std::string>, TypeList<
  AnyType<TokenT::CMD_OTIR, std::string>, TypeList<
  AnyType<TokenT::CMD_OUT,  std::string>, TypeList<
  AnyType<TokenT::CMD_OUTD, std::string>, TypeList<
  AnyType<TokenT::CMD_OUTI, std::string>, TypeList<
  AnyType<TokenT::CMD_POP,  std::string>, TypeList<
  AnyType<TokenT::CMD_PUSH, std::string>, TypeList<
  AnyType<TokenT::CMD_RES,  std::string>, TypeList<
  AnyType<TokenT::CMD_RET,  std::string>, TypeList<
  AnyType<TokenT::CMD_RETI, std::string>, TypeList<
  AnyType<TokenT::CMD_RETN, std::string>, TypeList<
  AnyType<TokenT::CMD_RL,   std::string>, TypeList<
  AnyType<TokenT::CMD_RLA,  std::string>, TypeList<
  AnyType<TokenT::CMD_RLC,  std::string>, TypeList<
  AnyType<TokenT::CMD_RLCA, std::string>, TypeList<
  AnyType<TokenT::CMD_RLD,  std::string>, TypeList<
  AnyType<TokenT::CMD_RR,   std::string>, TypeList<
  AnyType<TokenT::CMD_RRA,  std::string>, TypeList<
  AnyType<TokenT::CMD_RRC,  std::string>, TypeList<
  AnyType<TokenT::CMD_RRCA, std::string>, TypeList<
  AnyType<TokenT::CMD_RRD,  std::string>, TypeList<
  AnyType<TokenT::CMD_RST,  std::string>, TypeList<
  AnyType<TokenT::CMD_SBC,  std::string>, TypeList<
  AnyType<TokenT::CMD_SCF,  std::string>, TypeList<
  AnyType<TokenT::CMD_SET,  std::string>, TypeList<
  AnyType<TokenT::CMD_SLA,  std::string>, TypeList<
  AnyType<TokenT::CMD_SRA,  std::string>, TypeList<
  AnyType<TokenT::CMD_SRL,  std::string>, TypeList<
  AnyType<TokenT::CMD_SUB,  std::string>, TypeList<
  AnyType<TokenT::CMD_XOR,  std::string>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   CommandList;


typedef TypeList<
  AnyType<TokenT::OP_ORG,   std::string>, TypeList<
  AnyType<TokenT::OP_DB,    std::string>, TypeList<
  AnyType<TokenT::OP_EQU,   std::string>, TypeList<
  AnyType<TokenT::REG_AF,   std::string>, TypeList<
  AnyType<TokenT::REG_BC,   std::string>, TypeList<
  AnyType<TokenT::REG_DE,   std::string>, TypeList<
  AnyType<TokenT::REG_HL,   std::string>, TypeList<
  AnyType<TokenT::REG_IX,   std::string>, TypeList<
  AnyType<TokenT::REG_IY,   std::string>, TypeList<
  AnyType<TokenT::REG_SP,   std::string>, TypeList<
  AnyType<TokenT::REG_PC,   std::string>, TypeList<
  AnyType<TokenT::REG_AF$,  std::string>, TypeList<
  AnyType<TokenT::REG_BC$,  std::string>, TypeList<
  AnyType<TokenT::REG_DE$,  std::string>, TypeList<
  AnyType<TokenT::REG_HL$,  std::string>, TypeList<
  AnyType<TokenT::REG_A,    std::string>, TypeList<
  AnyType<TokenT::REG_B,    std::string>, TypeList<
  AnyType<TokenT::REG_C,    std::string>, TypeList<
  AnyType<TokenT::REG_D,    std::string>, TypeList<
  AnyType<TokenT::REG_E,    std::string>, TypeList<
  AnyType<TokenT::REG_H,    std::string>, TypeList<
  AnyType<TokenT::REG_L,    std::string>, TypeList<
  AnyType<TokenT::REG_I,    std::string>, TypeList<
  AnyType<TokenT::REG_R,    std::string>, TypeList<
  AnyType<TokenT::REG_A$,   std::string>, TypeList<
  AnyType<TokenT::REG_B$,   std::string>, TypeList<
  AnyType<TokenT::REG_C$,   std::string>, TypeList<
  AnyType<TokenT::REG_D$,   std::string>, TypeList<
  AnyType<TokenT::REG_E$,   std::string>, TypeList<
  AnyType<TokenT::REG_H$,   std::string>, TypeList<
  AnyType<TokenT::FLAG_C,   std::string>, TypeList<
  AnyType<TokenT::FLAG_M,   std::string>, TypeList<
  AnyType<TokenT::FLAG_NC,  std::string>, TypeList<
  AnyType<TokenT::FLAG_NZ,  std::string>, TypeList<
  AnyType<TokenT::FLAG_P,   std::string>, TypeList<
  AnyType<TokenT::FLAG_PE,  std::string>, TypeList<
  AnyType<TokenT::FLAG_PO,  std::string>, TypeList<
  AnyType<TokenT::FLAG_Z,   std::string>,  CommandList>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    KeywordList;


class Defs {

public:
  static void Init() {
    AnyType<TokenT::OP_ORG,  std::string>::GetValue() = "ORG";
    AnyType<TokenT::OP_DB,   std::string>::GetValue() = "DB";
    AnyType<TokenT::OP_EQU,  std::string>::GetValue() = "EQU";

    AnyType<TokenT::CMD_ADC,  std::string>::GetValue() = "ADC";
    AnyType<TokenT::CMD_ADD,  std::string>::GetValue() = "ADD";
    AnyType<TokenT::CMD_AND,  std::string>::GetValue() = "AND";
    AnyType<TokenT::CMD_BIT,  std::string>::GetValue() = "BIT";
    AnyType<TokenT::CMD_CALL, std::string>::GetValue() = "CALL";
    AnyType<TokenT::CMD_CCF,  std::string>::GetValue() = "CCF";
    AnyType<TokenT::CMD_CP,   std::string>::GetValue() = "CP";
    AnyType<TokenT::CMD_CPD,  std::string>::GetValue() = "CPD";
    AnyType<TokenT::CMD_CPDR, std::string>::GetValue() = "CPDR";
    AnyType<TokenT::CMD_CPI,  std::string>::GetValue() = "CPI";
    AnyType<TokenT::CMD_CPIR, std::string>::GetValue() = "CPIR";
    AnyType<TokenT::CMD_CPL,  std::string>::GetValue() = "CPL";
    AnyType<TokenT::CMD_DAA,  std::string>::GetValue() = "DAA";
    AnyType<TokenT::CMD_DEC,  std::string>::GetValue() = "DEC";
    AnyType<TokenT::CMD_DI,   std::string>::GetValue() = "DI";
    AnyType<TokenT::CMD_DJNZ, std::string>::GetValue() = "DJNZ";
    AnyType<TokenT::CMD_EI,   std::string>::GetValue() = "EI";
    AnyType<TokenT::CMD_EX,   std::string>::GetValue() = "EX";
    AnyType<TokenT::CMD_EXX,  std::string>::GetValue() = "EXX";
    AnyType<TokenT::CMD_HALT, std::string>::GetValue() = "HALT";
    AnyType<TokenT::CMD_IM,   std::string>::GetValue() = "IM";
    AnyType<TokenT::CMD_IN,   std::string>::GetValue() = "IN";
    AnyType<TokenT::CMD_INC,  std::string>::GetValue() = "INC";
    AnyType<TokenT::CMD_IND,  std::string>::GetValue() = "IND";
    AnyType<TokenT::CMD_INDR, std::string>::GetValue() = "INDR";
    AnyType<TokenT::CMD_INI,  std::string>::GetValue() = "INI";
    AnyType<TokenT::CMD_INIR, std::string>::GetValue() = "INIR";
    AnyType<TokenT::CMD_JP,   std::string>::GetValue() = "JP";
    AnyType<TokenT::CMD_JR,   std::string>::GetValue() = "JR";
    AnyType<TokenT::CMD_LD,   std::string>::GetValue() = "LD";
    AnyType<TokenT::CMD_LDD,  std::string>::GetValue() = "LDD";
    AnyType<TokenT::CMD_LDDR, std::string>::GetValue() = "LDDR";
    AnyType<TokenT::CMD_LDI,  std::string>::GetValue() = "LDI";
    AnyType<TokenT::CMD_LDIR, std::string>::GetValue() = "LDIR";
    AnyType<TokenT::CMD_NEG,  std::string>::GetValue() = "NEG";
    AnyType<TokenT::CMD_NOP,  std::string>::GetValue() = "NOP";
    AnyType<TokenT::CMD_OR,   std::string>::GetValue() = "OR";
    AnyType<TokenT::CMD_OTDR, std::string>::GetValue() = "OTDR";
    AnyType<TokenT::CMD_OTIR, std::string>::GetValue() = "OTIR";
    AnyType<TokenT::CMD_OUT,  std::string>::GetValue() = "OUT";
    AnyType<TokenT::CMD_OUTD, std::string>::GetValue() = "OUTD";
    AnyType<TokenT::CMD_OUTI, std::string>::GetValue() = "OUTI";
    AnyType<TokenT::CMD_POP,  std::string>::GetValue() = "POP";
    AnyType<TokenT::CMD_PUSH, std::string>::GetValue() = "PUSH";
    AnyType<TokenT::CMD_RES,  std::string>::GetValue() = "RES";
    AnyType<TokenT::CMD_RET,  std::string>::GetValue() = "RET";
    AnyType<TokenT::CMD_RETI, std::string>::GetValue() = "RETI";
    AnyType<TokenT::CMD_RETN, std::string>::GetValue() = "RETN";
    AnyType<TokenT::CMD_RL,   std::string>::GetValue() = "RL";
    AnyType<TokenT::CMD_RLA,  std::string>::GetValue() = "RLA";
    AnyType<TokenT::CMD_RLC,  std::string>::GetValue() = "RLC";
    AnyType<TokenT::CMD_RLCA, std::string>::GetValue() = "RLCA";
    AnyType<TokenT::CMD_RLD,  std::string>::GetValue() = "RLD";
    AnyType<TokenT::CMD_RR,   std::string>::GetValue() = "RR";
    AnyType<TokenT::CMD_RRA,  std::string>::GetValue() = "RRA";
    AnyType<TokenT::CMD_RRC,  std::string>::GetValue() = "RRC";
    AnyType<TokenT::CMD_RRCA, std::string>::GetValue() = "RRCA";
    AnyType<TokenT::CMD_RRD,  std::string>::GetValue() = "RRD";
    AnyType<TokenT::CMD_RST,  std::string>::GetValue() = "RST";
    AnyType<TokenT::CMD_SBC,  std::string>::GetValue() = "SBC";
    AnyType<TokenT::CMD_SCF,  std::string>::GetValue() = "SCF";
    AnyType<TokenT::CMD_SET,  std::string>::GetValue() = "SET";
    AnyType<TokenT::CMD_SLA,  std::string>::GetValue() = "SLA";
    AnyType<TokenT::CMD_SRA,  std::string>::GetValue() = "SRA";
    AnyType<TokenT::CMD_SRL,  std::string>::GetValue() = "SRL";
    AnyType<TokenT::CMD_SUB,  std::string>::GetValue() = "SUB";
    AnyType<TokenT::CMD_XOR,  std::string>::GetValue() = "XOR";

    // NOTE: Registers

    AnyType<TokenT::REG_AF,   std::string>::GetValue() = "AF";
    AnyType<TokenT::REG_BC,   std::string>::GetValue() = "BC";
    AnyType<TokenT::REG_DE,   std::string>::GetValue() = "DE";
    AnyType<TokenT::REG_HL,   std::string>::GetValue() = "HL";
    AnyType<TokenT::REG_IX,   std::string>::GetValue() = "IX";
    AnyType<TokenT::REG_IY,   std::string>::GetValue() = "IY";
    AnyType<TokenT::REG_SP,   std::string>::GetValue() = "SP";
    AnyType<TokenT::REG_PC,   std::string>::GetValue() = "PC";

    AnyType<TokenT::REG_AF$,  std::string>::GetValue() = "AF'";
    AnyType<TokenT::REG_BC$,  std::string>::GetValue() = "BC'";
    AnyType<TokenT::REG_DE$,  std::string>::GetValue() = "DE'";
    AnyType<TokenT::REG_HL$,  std::string>::GetValue() = "HL'";

    AnyType<TokenT::REG_A,   std::string>::GetValue() = "A";
    AnyType<TokenT::REG_B,   std::string>::GetValue() = "B";
    AnyType<TokenT::REG_C,   std::string>::GetValue() = "C";
    AnyType<TokenT::REG_D,   std::string>::GetValue() = "D";
    AnyType<TokenT::REG_E,   std::string>::GetValue() = "E";
    AnyType<TokenT::REG_H,   std::string>::GetValue() = "H";
    AnyType<TokenT::REG_L,   std::string>::GetValue() = "L";
    AnyType<TokenT::REG_I,   std::string>::GetValue() = "I";
    AnyType<TokenT::REG_R,   std::string>::GetValue() = "R";

    AnyType<TokenT::REG_A$,  std::string>::GetValue() = "A'";
    AnyType<TokenT::REG_B$,  std::string>::GetValue() = "B'";
    AnyType<TokenT::REG_C$,  std::string>::GetValue() = "C'";
    AnyType<TokenT::REG_D$,  std::string>::GetValue() = "D'";
    AnyType<TokenT::REG_E$,  std::string>::GetValue() = "E'";
    AnyType<TokenT::REG_H$,  std::string>::GetValue() = "H'";
    AnyType<TokenT::REG_L$,  std::string>::GetValue() = "L'";

    AnyType<TokenT::FLAG_C,  std::string>::GetValue() = "C";
    AnyType<TokenT::FLAG_M,  std::string>::GetValue() = "M";
    AnyType<TokenT::FLAG_NC, std::string>::GetValue() = "NC";
    AnyType<TokenT::FLAG_NZ, std::string>::GetValue() = "NZ";
    AnyType<TokenT::FLAG_P,  std::string>::GetValue() = "P";
    AnyType<TokenT::FLAG_PE, std::string>::GetValue() = "PE";
    AnyType<TokenT::FLAG_PO, std::string>::GetValue() = "PO";
    AnyType<TokenT::FLAG_Z,  std::string>::GetValue() = "Z";
  }

  static inline uint32_t Int2Mask(int32_t size) {
    if (size == 0) return 0x00;
    return (Int2Mask(size - 1) << 8) | 0xFF;
  }

  static inline uint32_t Reg2Mask(TokenT reg) {
    switch (reg) {
      case TokenT::REG_A: return 0b111;
      case TokenT::REG_B: return 0b000;
      case TokenT::REG_C: return 0b001;
      case TokenT::REG_D: return 0b010;
      case TokenT::REG_E: return 0b011;
      case TokenT::REG_H: return 0b100;
      case TokenT::REG_L: return 0b101;
      default: return 0b000;
    }
  }
};
}