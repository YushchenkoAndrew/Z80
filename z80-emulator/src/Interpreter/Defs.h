#pragma once

#include "include/Typelist.h"
#include "lib/olcPixelGameEngine.h"

namespace Interpreter {

class Token;
typedef std::pair<std::string, std::shared_ptr<Token>> RelativeToken;

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */
enum TokenT {
  // Single char tokens
  NONE, LEFT_BRACE,  RIGHT_BRACE, COMMA, COLON, PLUS, MINUS, CONCATENATE, 
  LEFT_SQUARE_BRACE, RIGHT_SQUARE_BRACE,
  
  // Bit operation in assignment
  BIT_OR, BIT_AND, BIT_XOR, BIT_NOT, LEFT_SHIFT, RIGHT_SHIFT,

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
  OP_ORG, OP_DB, OP_DW, OP_EQU, OP_INCLUDE,

  OP_EOF, OP_NONE
};

typedef TypeList<TokenT, std::string> TokenSTR;
typedef TypeList<TokenT, olc::Pixel> TokenCOLOR;

typedef TypeList<
  Int2Type<TokenT::OP_ORG>,   TypeList<
  Int2Type<TokenT::OP_DB>,    TypeList<
  Int2Type<TokenT::OP_DW>,    TypeList<
  Int2Type<TokenT::CMD_ADC>,  TypeList<
  Int2Type<TokenT::CMD_ADD>,  TypeList<
  Int2Type<TokenT::CMD_AND>,  TypeList<
  Int2Type<TokenT::CMD_BIT>,  TypeList<
  Int2Type<TokenT::CMD_CALL>, TypeList<
  Int2Type<TokenT::CMD_CCF>,  TypeList<
  Int2Type<TokenT::CMD_CP>,   TypeList<
  Int2Type<TokenT::CMD_CPD>,  TypeList<
  Int2Type<TokenT::CMD_CPDR>, TypeList<
  Int2Type<TokenT::CMD_CPI>,  TypeList<
  Int2Type<TokenT::CMD_CPIR>, TypeList<
  Int2Type<TokenT::CMD_CPL>,  TypeList<
  Int2Type<TokenT::CMD_DAA>,  TypeList<
  Int2Type<TokenT::CMD_DEC>,  TypeList<
  Int2Type<TokenT::CMD_DI>,   TypeList<
  Int2Type<TokenT::CMD_DJNZ>, TypeList<
  Int2Type<TokenT::CMD_EI>,   TypeList<
  Int2Type<TokenT::CMD_EX>,   TypeList<
  Int2Type<TokenT::CMD_EXX>,  TypeList<
  Int2Type<TokenT::CMD_HALT>, TypeList<
  Int2Type<TokenT::CMD_IM>,   TypeList<
  Int2Type<TokenT::CMD_IN>,   TypeList<
  Int2Type<TokenT::CMD_INC>,  TypeList<
  Int2Type<TokenT::CMD_IND>,  TypeList<
  Int2Type<TokenT::CMD_INDR>, TypeList<
  Int2Type<TokenT::CMD_INI>,  TypeList<
  Int2Type<TokenT::CMD_INIR>, TypeList<
  Int2Type<TokenT::CMD_JP>,   TypeList<
  Int2Type<TokenT::CMD_JR>,   TypeList<
  Int2Type<TokenT::CMD_LD>,   TypeList<
  Int2Type<TokenT::CMD_LDD>,  TypeList<
  Int2Type<TokenT::CMD_LDDR>, TypeList<
  Int2Type<TokenT::CMD_LDI>,  TypeList<
  Int2Type<TokenT::CMD_LDIR>, TypeList<
  Int2Type<TokenT::CMD_NEG>,  TypeList<
  Int2Type<TokenT::CMD_NOP>,  TypeList<
  Int2Type<TokenT::CMD_OR>,   TypeList<
  Int2Type<TokenT::CMD_OTDR>, TypeList<
  Int2Type<TokenT::CMD_OTIR>, TypeList<
  Int2Type<TokenT::CMD_OUT>,  TypeList<
  Int2Type<TokenT::CMD_OUTD>, TypeList<
  Int2Type<TokenT::CMD_OUTI>, TypeList<
  Int2Type<TokenT::CMD_POP>,  TypeList<
  Int2Type<TokenT::CMD_PUSH>, TypeList<
  Int2Type<TokenT::CMD_RES>,  TypeList<
  Int2Type<TokenT::CMD_RET>,  TypeList<
  Int2Type<TokenT::CMD_RETI>, TypeList<
  Int2Type<TokenT::CMD_RETN>, TypeList<
  Int2Type<TokenT::CMD_RL>,   TypeList<
  Int2Type<TokenT::CMD_RLA>,  TypeList<
  Int2Type<TokenT::CMD_RLC>,  TypeList<
  Int2Type<TokenT::CMD_RLCA>, TypeList<
  Int2Type<TokenT::CMD_RLD>,  TypeList<
  Int2Type<TokenT::CMD_RR>,   TypeList<
  Int2Type<TokenT::CMD_RRA>,  TypeList<
  Int2Type<TokenT::CMD_RRC>,  TypeList<
  Int2Type<TokenT::CMD_RRCA>, TypeList<
  Int2Type<TokenT::CMD_RRD>,  TypeList<
  Int2Type<TokenT::CMD_RST>,  TypeList<
  Int2Type<TokenT::CMD_SBC>,  TypeList<
  Int2Type<TokenT::CMD_SCF>,  TypeList<
  Int2Type<TokenT::CMD_SET>,  TypeList<
  Int2Type<TokenT::CMD_SLA>,  TypeList<
  Int2Type<TokenT::CMD_SRA>,  TypeList<
  Int2Type<TokenT::CMD_SRL>,  TypeList<
  Int2Type<TokenT::CMD_SUB>,  TypeList<
  Int2Type<TokenT::CMD_XOR>,  NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   Commands;

class Defs {

public:
  static void Init() {}
};
}