#pragma once
#include "src/Window/Lines.h"

namespace Interpreter {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */
enum TokenT {
  // Single char tokens
  NONE, LEFT_BRACE,  RIGHT_BRACE, COMMA, COLON, PLUS, MINUS, CONCATENATE,
  
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

typedef TypeList<TokenT, std::string> TokenSTR;
typedef TypeList<TokenT, olc::Pixel> TokenCOLOR;

typedef TypeList<
  AnyType<TokenT::OP_ORG,   TokenSTR>, TypeList<
  AnyType<TokenT::OP_DB,    TokenSTR>, TypeList<
  AnyType<TokenT::CMD_ADC,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_ADD,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_AND,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_BIT,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CALL, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CCF,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CP,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CPD,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CPDR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CPI,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CPIR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_CPL,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_DAA,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_DEC,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_DI,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_DJNZ, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_EI,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_EX,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_EXX,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_HALT, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_IM,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_IN,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_INC,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_IND,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_INDR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_INI,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_INIR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_JP,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_JR,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_LD,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_LDD,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_LDDR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_LDI,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_LDIR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_NEG,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_NOP,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_OR,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_OTDR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_OTIR, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_OUT,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_OUTD, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_OUTI, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_POP,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_PUSH, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RES,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RET,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RETI, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RETN, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RL,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RLA,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RLC,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RLCA, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RLD,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RR,   TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RRA,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RRC,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RRCA, TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RRD,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_RST,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_SBC,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_SCF,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_SET,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_SLA,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_SRA,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_SRL,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_SUB,  TokenSTR>, TypeList<
  AnyType<TokenT::CMD_XOR,  TokenSTR>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
   CommandList;


typedef TypeList<
  AnyType<TokenT::OP_EQU,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_AF,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_BC,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_DE,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_HL,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_IX,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_IY,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_SP,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_PC,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_AF$,  TokenSTR>, TypeList<
  AnyType<TokenT::REG_BC$,  TokenSTR>, TypeList<
  AnyType<TokenT::REG_DE$,  TokenSTR>, TypeList<
  AnyType<TokenT::REG_HL$,  TokenSTR>, TypeList<
  AnyType<TokenT::REG_A,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_B,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_C,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_D,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_E,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_H,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_L,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_I,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_R,    TokenSTR>, TypeList<
  AnyType<TokenT::REG_A$,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_B$,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_C$,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_D$,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_E$,   TokenSTR>, TypeList<
  AnyType<TokenT::REG_H$,   TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_C,   TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_M,   TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_NC,  TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_NZ,  TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_P,   TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_PE,  TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_PO,  TokenSTR>, TypeList<
  AnyType<TokenT::FLAG_Z,   TokenSTR>,  CommandList>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    KeywordList;


typedef TypeList<
  AnyType<TokenT::OP_ORG,   TokenCOLOR>, TypeList<
  AnyType<TokenT::OP_DB,    TokenCOLOR>, TypeList<
  AnyType<TokenT::OP_EQU,   TokenCOLOR>, TypeList<

  AnyType<TokenT::CMD_ADC,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_ADD,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_AND,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_BIT,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CALL, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CCF,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CP,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CPD,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CPDR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CPI,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CPIR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_CPL,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_DAA,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_DEC,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_DI,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_DJNZ, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_EI,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_EX,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_EXX,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_HALT, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_IM,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_IN,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_INC,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_IND,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_INDR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_INI,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_INIR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_JP,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_JR,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_LD,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_LDD,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_LDDR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_LDI,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_LDIR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_NEG,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_NOP,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_OR,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_OTDR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_OTIR, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_OUT,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_OUTD, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_OUTI, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_POP,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_PUSH, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RES,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RET,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RETI, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RETN, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RL,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RLA,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RLC,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RLCA, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RLD,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RR,   TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RRA,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RRC,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RRCA, TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RRD,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_RST,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_SBC,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_SCF,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_SET,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_SLA,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_SRA,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_SRL,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_SUB,  TokenCOLOR>, TypeList<
  AnyType<TokenT::CMD_XOR,  TokenCOLOR>, TypeList<

  AnyType<TokenT::REG_AF,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_BC,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_DE,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_HL,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_IX,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_IY,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_SP,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_PC,   TokenCOLOR>, TypeList<

  AnyType<TokenT::REG_AF$,  TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_BC$,  TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_DE$,  TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_HL$,  TokenCOLOR>, TypeList<

  AnyType<TokenT::REG_A,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_B,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_C,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_D,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_E,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_H,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_L,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_I,    TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_R,    TokenCOLOR>, TypeList<

  AnyType<TokenT::REG_A$,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_B$,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_C$,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_D$,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_E$,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_H$,   TokenCOLOR>, TypeList<
  AnyType<TokenT::REG_L$,   TokenCOLOR>, TypeList<

  AnyType<TokenT::FLAG_C,   TokenCOLOR>, TypeList<
  AnyType<TokenT::FLAG_M,   TokenCOLOR>, TypeList<
  AnyType<TokenT::FLAG_NC,  TokenCOLOR>, TypeList<
  AnyType<TokenT::FLAG_NZ,  TokenCOLOR>, TypeList<
  AnyType<TokenT::FLAG_P,   TokenCOLOR>, TypeList<
  AnyType<TokenT::FLAG_PE,  TokenCOLOR>, TypeList<
  AnyType<TokenT::FLAG_PO,  TokenCOLOR>, TypeList<
  AnyType<TokenT::FLAG_Z,   TokenCOLOR>, TypeList<
  
  AnyType<TokenT::NUMBER,  TokenCOLOR>, TypeList<
  AnyType<TokenT::STRING,  TokenCOLOR>, TypeList<
  AnyType<TokenT::IDENTIFIER, TokenCOLOR>, NullType>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    TokenColorList;

class Defs {

public:
  static void Init() {
    AnyType<TokenT::OP_ORG,   TokenSTR>::GetValue() = "ORG";   AnyType<TokenT::OP_ORG,   TokenCOLOR>::GetValue() = AnyType<Colors::CYAN,    ColorT>::GetValue();
    AnyType<TokenT::OP_DB,    TokenSTR>::GetValue() = "DB";    AnyType<TokenT::OP_DB,    TokenCOLOR>::GetValue() = AnyType<Colors::CYAN,    ColorT>::GetValue();
    AnyType<TokenT::OP_EQU,   TokenSTR>::GetValue() = "EQU";   AnyType<TokenT::OP_EQU,   TokenCOLOR>::GetValue() = AnyType<Colors::CYAN,    ColorT>::GetValue();

    AnyType<TokenT::CMD_ADC,  TokenSTR>::GetValue() = "ADC";   AnyType<TokenT::CMD_ADC,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_ADD,  TokenSTR>::GetValue() = "ADD";   AnyType<TokenT::CMD_ADD,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_AND,  TokenSTR>::GetValue() = "AND";   AnyType<TokenT::CMD_AND,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_BIT,  TokenSTR>::GetValue() = "BIT";   AnyType<TokenT::CMD_BIT,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CALL, TokenSTR>::GetValue() = "CALL";  AnyType<TokenT::CMD_CALL, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CCF,  TokenSTR>::GetValue() = "CCF";   AnyType<TokenT::CMD_CCF,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CP,   TokenSTR>::GetValue() = "CP";    AnyType<TokenT::CMD_CP,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CPD,  TokenSTR>::GetValue() = "CPD";   AnyType<TokenT::CMD_CPD,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CPDR, TokenSTR>::GetValue() = "CPDR";  AnyType<TokenT::CMD_CPDR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CPI,  TokenSTR>::GetValue() = "CPI";   AnyType<TokenT::CMD_CPI,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CPIR, TokenSTR>::GetValue() = "CPIR";  AnyType<TokenT::CMD_CPIR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_CPL,  TokenSTR>::GetValue() = "CPL";   AnyType<TokenT::CMD_CPL,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_DAA,  TokenSTR>::GetValue() = "DAA";   AnyType<TokenT::CMD_DAA,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_DEC,  TokenSTR>::GetValue() = "DEC";   AnyType<TokenT::CMD_DEC,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_DI,   TokenSTR>::GetValue() = "DI";    AnyType<TokenT::CMD_DI,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_DJNZ, TokenSTR>::GetValue() = "DJNZ";  AnyType<TokenT::CMD_DJNZ, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_EI,   TokenSTR>::GetValue() = "EI";    AnyType<TokenT::CMD_EI,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_EX,   TokenSTR>::GetValue() = "EX";    AnyType<TokenT::CMD_EX,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_EXX,  TokenSTR>::GetValue() = "EXX";   AnyType<TokenT::CMD_EXX,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_HALT, TokenSTR>::GetValue() = "HALT";  AnyType<TokenT::CMD_HALT, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_IM,   TokenSTR>::GetValue() = "IM";    AnyType<TokenT::CMD_IM,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_IN,   TokenSTR>::GetValue() = "IN";    AnyType<TokenT::CMD_IN,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_INC,  TokenSTR>::GetValue() = "INC";   AnyType<TokenT::CMD_INC,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_IND,  TokenSTR>::GetValue() = "IND";   AnyType<TokenT::CMD_IND,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_INDR, TokenSTR>::GetValue() = "INDR";  AnyType<TokenT::CMD_INDR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_INI,  TokenSTR>::GetValue() = "INI";   AnyType<TokenT::CMD_INI,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_INIR, TokenSTR>::GetValue() = "INIR";  AnyType<TokenT::CMD_INIR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_JP,   TokenSTR>::GetValue() = "JP";    AnyType<TokenT::CMD_JP,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_JR,   TokenSTR>::GetValue() = "JR";    AnyType<TokenT::CMD_JR,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_LD,   TokenSTR>::GetValue() = "LD";    AnyType<TokenT::CMD_LD,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_LDD,  TokenSTR>::GetValue() = "LDD";   AnyType<TokenT::CMD_LDD,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_LDDR, TokenSTR>::GetValue() = "LDDR";  AnyType<TokenT::CMD_LDDR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_LDI,  TokenSTR>::GetValue() = "LDI";   AnyType<TokenT::CMD_LDI,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_LDIR, TokenSTR>::GetValue() = "LDIR";  AnyType<TokenT::CMD_LDIR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_NEG,  TokenSTR>::GetValue() = "NEG";   AnyType<TokenT::CMD_NEG,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_NOP,  TokenSTR>::GetValue() = "NOP";   AnyType<TokenT::CMD_NOP,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_OR,   TokenSTR>::GetValue() = "OR";    AnyType<TokenT::CMD_OR,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_OTDR, TokenSTR>::GetValue() = "OTDR";  AnyType<TokenT::CMD_OTDR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_OTIR, TokenSTR>::GetValue() = "OTIR";  AnyType<TokenT::CMD_OTIR, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_OUT,  TokenSTR>::GetValue() = "OUT";   AnyType<TokenT::CMD_OUT,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_OUTD, TokenSTR>::GetValue() = "OUTD";  AnyType<TokenT::CMD_OUTD, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_OUTI, TokenSTR>::GetValue() = "OUTI";  AnyType<TokenT::CMD_OUTI, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_POP,  TokenSTR>::GetValue() = "POP";   AnyType<TokenT::CMD_POP,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_PUSH, TokenSTR>::GetValue() = "PUSH";  AnyType<TokenT::CMD_PUSH, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RES,  TokenSTR>::GetValue() = "RES";   AnyType<TokenT::CMD_RES,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RET,  TokenSTR>::GetValue() = "RET";   AnyType<TokenT::CMD_RET,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RETI, TokenSTR>::GetValue() = "RETI";  AnyType<TokenT::CMD_RETI, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RETN, TokenSTR>::GetValue() = "RETN";  AnyType<TokenT::CMD_RETN, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RL,   TokenSTR>::GetValue() = "RL";    AnyType<TokenT::CMD_RL,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RLA,  TokenSTR>::GetValue() = "RLA";   AnyType<TokenT::CMD_RLA,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RLC,  TokenSTR>::GetValue() = "RLC";   AnyType<TokenT::CMD_RLC,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RLCA, TokenSTR>::GetValue() = "RLCA";  AnyType<TokenT::CMD_RLCA, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RLD,  TokenSTR>::GetValue() = "RLD";   AnyType<TokenT::CMD_RLD,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RR,   TokenSTR>::GetValue() = "RR";    AnyType<TokenT::CMD_RR,   TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RRA,  TokenSTR>::GetValue() = "RRA";   AnyType<TokenT::CMD_RRA,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RRC,  TokenSTR>::GetValue() = "RRC";   AnyType<TokenT::CMD_RRC,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RRCA, TokenSTR>::GetValue() = "RRCA";  AnyType<TokenT::CMD_RRCA, TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RRD,  TokenSTR>::GetValue() = "RRD";   AnyType<TokenT::CMD_RRD,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_RST,  TokenSTR>::GetValue() = "RST";   AnyType<TokenT::CMD_RST,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_SBC,  TokenSTR>::GetValue() = "SBC";   AnyType<TokenT::CMD_SBC,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_SCF,  TokenSTR>::GetValue() = "SCF";   AnyType<TokenT::CMD_SCF,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_SET,  TokenSTR>::GetValue() = "SET";   AnyType<TokenT::CMD_SET,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_SLA,  TokenSTR>::GetValue() = "SLA";   AnyType<TokenT::CMD_SLA,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_SRA,  TokenSTR>::GetValue() = "SRA";   AnyType<TokenT::CMD_SRA,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_SRL,  TokenSTR>::GetValue() = "SRL";   AnyType<TokenT::CMD_SRL,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_SUB,  TokenSTR>::GetValue() = "SUB";   AnyType<TokenT::CMD_SUB,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();
    AnyType<TokenT::CMD_XOR,  TokenSTR>::GetValue() = "XOR";   AnyType<TokenT::CMD_XOR,  TokenCOLOR>::GetValue() = AnyType<Colors::MAGENTA, ColorT>::GetValue();

    // Registers.val

    AnyType<TokenT::REG_AF,   TokenSTR>::GetValue() = "AF";    AnyType<TokenT::REG_AF,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();
    AnyType<TokenT::REG_BC,   TokenSTR>::GetValue() = "BC";    AnyType<TokenT::REG_BC,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();
    AnyType<TokenT::REG_DE,   TokenSTR>::GetValue() = "DE";    AnyType<TokenT::REG_DE,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();
    AnyType<TokenT::REG_HL,   TokenSTR>::GetValue() = "HL";    AnyType<TokenT::REG_HL,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();
    AnyType<TokenT::REG_IX,   TokenSTR>::GetValue() = "IX";    AnyType<TokenT::REG_IX,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();
    AnyType<TokenT::REG_IY,   TokenSTR>::GetValue() = "IY";    AnyType<TokenT::REG_IY,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();
    AnyType<TokenT::REG_SP,   TokenSTR>::GetValue() = "SP";    AnyType<TokenT::REG_SP,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();
    AnyType<TokenT::REG_PC,   TokenSTR>::GetValue() = "PC";    AnyType<TokenT::REG_PC,   TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE,  ColorT>::GetValue();

    AnyType<TokenT::REG_AF$,  TokenSTR>::GetValue() = "AF'";   AnyType<TokenT::REG_AF$,  TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_BC$,  TokenSTR>::GetValue() = "BC'";   AnyType<TokenT::REG_BC$,  TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_DE$,  TokenSTR>::GetValue() = "DE'";   AnyType<TokenT::REG_DE$,  TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_HL$,  TokenSTR>::GetValue() = "HL'";   AnyType<TokenT::REG_HL$,  TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();

    AnyType<TokenT::REG_A,    TokenSTR>::GetValue() = "A";     AnyType<TokenT::REG_A,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_B,    TokenSTR>::GetValue() = "B";     AnyType<TokenT::REG_B,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_C,    TokenSTR>::GetValue() = "C";     AnyType<TokenT::REG_C,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_D,    TokenSTR>::GetValue() = "D";     AnyType<TokenT::REG_D,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_E,    TokenSTR>::GetValue() = "E";     AnyType<TokenT::REG_E,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_H,    TokenSTR>::GetValue() = "H";     AnyType<TokenT::REG_H,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_L,    TokenSTR>::GetValue() = "L";     AnyType<TokenT::REG_L,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_I,    TokenSTR>::GetValue() = "I";     AnyType<TokenT::REG_I,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();
    AnyType<TokenT::REG_R,    TokenSTR>::GetValue() = "R";     AnyType<TokenT::REG_R,    TokenCOLOR>::GetValue() = AnyType<Colors::ORANGE, ColorT>::GetValue();

    AnyType<TokenT::REG_A$,   TokenSTR>::GetValue() = "A'";    AnyType<TokenT::REG_A$,   TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_B$,   TokenSTR>::GetValue() = "B'";    AnyType<TokenT::REG_B$,   TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_C$,   TokenSTR>::GetValue() = "C'";    AnyType<TokenT::REG_C$,   TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_D$,   TokenSTR>::GetValue() = "D'";    AnyType<TokenT::REG_D$,   TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_E$,   TokenSTR>::GetValue() = "E'";    AnyType<TokenT::REG_E$,   TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_H$,   TokenSTR>::GetValue() = "H'";    AnyType<TokenT::REG_H$,   TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();
    AnyType<TokenT::REG_L$,   TokenSTR>::GetValue() = "L'";    AnyType<TokenT::REG_L$,   TokenCOLOR>::GetValue() = AnyType<Colors::DARK_YELLOW, ColorT>::GetValue();

    AnyType<TokenT::FLAG_C,   TokenSTR>::GetValue() = "C";     AnyType<TokenT::FLAG_C,   TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();
    AnyType<TokenT::FLAG_M,   TokenSTR>::GetValue() = "M";     AnyType<TokenT::FLAG_M,   TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();
    AnyType<TokenT::FLAG_NC,  TokenSTR>::GetValue() = "NC";    AnyType<TokenT::FLAG_NC,  TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();
    AnyType<TokenT::FLAG_NZ,  TokenSTR>::GetValue() = "NZ";    AnyType<TokenT::FLAG_NZ,  TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();
    AnyType<TokenT::FLAG_P,   TokenSTR>::GetValue() = "P";     AnyType<TokenT::FLAG_P,   TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();
    AnyType<TokenT::FLAG_PE,  TokenSTR>::GetValue() = "PE";    AnyType<TokenT::FLAG_PE,  TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();
    AnyType<TokenT::FLAG_PO,  TokenSTR>::GetValue() = "PO";    AnyType<TokenT::FLAG_PO,  TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();
    AnyType<TokenT::FLAG_Z,   TokenSTR>::GetValue() = "Z";     AnyType<TokenT::FLAG_Z,   TokenCOLOR>::GetValue() = AnyType<Colors::BLUE,   ColorT>::GetValue();

    // Literals
    AnyType<TokenT::NUMBER,     TokenCOLOR>::GetValue() = AnyType<Colors::RED,    ColorT>::GetValue();
    AnyType<TokenT::STRING,     TokenCOLOR>::GetValue() = AnyType<Colors::YELLOW, ColorT>::GetValue();
    AnyType<TokenT::IDENTIFIER, TokenCOLOR>::GetValue() = AnyType<Colors::WHITE,  ColorT>::GetValue();
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