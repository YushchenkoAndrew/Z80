#pragma once

#include "src/Defs.h"
#include "src/Interpreter/Defs.h"
#include "lib/olcPixelGameEngine.h"
#include <string>

namespace Interpreter {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */

typedef std::pair<const char*, olc::Pixel> MetadataT;
class Token {
public:
  Token(): token(TokenT::NONE), lexeme(""), literal(""), col(-1), line(-1) {}
  Token(TokenT t, std::string le, std::string li, int32_t col, int32_t line):
    token(t), lexeme(le), literal(li), col(col), line(line) {}

  void print() { 
    const auto& [name, _] = GetMetadata(token);

    if (!std::strlen(name)) printf("{ type: %d; lexeme: '%s'; literal: '%s' } Ln %d, Col %d\n", token, lexeme.c_str(), literal.c_str(), line, col);
    else printf("{ type: '%s'; lexeme: '%s'; literal: '%s' } Ln %d, Col %d\n", name, lexeme.c_str(), literal.c_str(), line, col);
  }

  static const TokenT GetToken(std::string lexeme) {
    for (const auto& [token, info] : metadata) {
      if (info.first == lexeme) return token;
    }

    return TokenT::NONE;
  };

  static const MetadataT& GetMetadata(TokenT token) {
    auto it = metadata.find(token);
    if (it != metadata.end()) return metadata.at(token);
    return metadata.at(TokenT::NONE);
  };

public:
  const TokenT token;
  const std::string lexeme;
  const std::string literal;

  const int32_t col;
  const int32_t line;

private:
static inline const std::unordered_map<TokenT, MetadataT> metadata = {
    { TokenT::NONE,       { "",         Color::WHITE } },
    { TokenT::OP_INCLUDE, { "#include", Color::CYAN } },

    { TokenT::OP_ORG, { "ORG", Color::CYAN } },
    { TokenT::OP_DB,  { "DB",  Color::CYAN } },
    { TokenT::OP_DW,  { "DW",  Color::CYAN } },
    { TokenT::OP_EQU, { "EQU", Color::CYAN } },


    { TokenT::CMD_ADC,  { "ADC",   Color::MAGENTA } },
    { TokenT::CMD_ADD,  { "ADD",   Color::MAGENTA } },
    { TokenT::CMD_AND,  { "AND",   Color::MAGENTA } },
    { TokenT::CMD_BIT,  { "BIT",   Color::MAGENTA } },
    { TokenT::CMD_CALL, { "CALL",  Color::MAGENTA } },
    { TokenT::CMD_CCF,  { "CCF",   Color::MAGENTA } },
    { TokenT::CMD_CP,   { "CP",    Color::MAGENTA } },
    { TokenT::CMD_CPD,  { "CPD",   Color::MAGENTA } },
    { TokenT::CMD_CPDR, { "CPDR",  Color::MAGENTA } },
    { TokenT::CMD_CPI,  { "CPI",   Color::MAGENTA } },
    { TokenT::CMD_CPIR, { "CPIR",  Color::MAGENTA } },
    { TokenT::CMD_CPL,  { "CPL",   Color::MAGENTA } },
    { TokenT::CMD_DAA,  { "DAA",   Color::MAGENTA } },
    { TokenT::CMD_DEC,  { "DEC",   Color::MAGENTA } },
    { TokenT::CMD_DI,   { "DI",    Color::MAGENTA } },
    { TokenT::CMD_DJNZ, { "DJNZ",  Color::MAGENTA } },
    { TokenT::CMD_EI,   { "EI",    Color::MAGENTA } },
    { TokenT::CMD_EX,   { "EX",    Color::MAGENTA } },
    { TokenT::CMD_EXX,  { "EXX",   Color::MAGENTA } },
    { TokenT::CMD_HALT, { "HALT",  Color::MAGENTA } },
    { TokenT::CMD_IM,   { "IM",    Color::MAGENTA } },
    { TokenT::CMD_IN,   { "IN",    Color::MAGENTA } },
    { TokenT::CMD_INC,  { "INC",   Color::MAGENTA } },
    { TokenT::CMD_IND,  { "IND",   Color::MAGENTA } },
    { TokenT::CMD_INDR, { "INDR",  Color::MAGENTA } },
    { TokenT::CMD_INI,  { "INI",   Color::MAGENTA } },
    { TokenT::CMD_INIR, { "INIR",  Color::MAGENTA } },
    { TokenT::CMD_JP,   { "JP",    Color::MAGENTA } },
    { TokenT::CMD_JR,   { "JR",    Color::MAGENTA } },
    { TokenT::CMD_LD,   { "LD",    Color::MAGENTA } },
    { TokenT::CMD_LDD,  { "LDD",   Color::MAGENTA } },
    { TokenT::CMD_LDDR, { "LDDR",  Color::MAGENTA } },
    { TokenT::CMD_LDI,  { "LDI",   Color::MAGENTA } },
    { TokenT::CMD_LDIR, { "LDIR",  Color::MAGENTA } },
    { TokenT::CMD_NEG,  { "NEG",   Color::MAGENTA } },
    { TokenT::CMD_NOP,  { "NOP",   Color::MAGENTA } },
    { TokenT::CMD_OR,   { "OR",    Color::MAGENTA } },
    { TokenT::CMD_OTDR, { "OTDR",  Color::MAGENTA } },
    { TokenT::CMD_OTIR, { "OTIR",  Color::MAGENTA } },
    { TokenT::CMD_OUT,  { "OUT",   Color::MAGENTA } },
    { TokenT::CMD_OUTD, { "OUTD",  Color::MAGENTA } },
    { TokenT::CMD_OUTI, { "OUTI",  Color::MAGENTA } },
    { TokenT::CMD_POP,  { "POP",   Color::MAGENTA } },
    { TokenT::CMD_PUSH, { "PUSH",  Color::MAGENTA } },
    { TokenT::CMD_RES,  { "RES",   Color::MAGENTA } },
    { TokenT::CMD_RET,  { "RET",   Color::MAGENTA } },
    { TokenT::CMD_RETI, { "RETI",  Color::MAGENTA } },
    { TokenT::CMD_RETN, { "RETN",  Color::MAGENTA } },
    { TokenT::CMD_RL,   { "RL",    Color::MAGENTA } },
    { TokenT::CMD_RLA,  { "RLA",   Color::MAGENTA } },
    { TokenT::CMD_RLC,  { "RLC",   Color::MAGENTA } },
    { TokenT::CMD_RLCA, { "RLCA",  Color::MAGENTA } },
    { TokenT::CMD_RLD,  { "RLD",   Color::MAGENTA } },
    { TokenT::CMD_RR,   { "RR",    Color::MAGENTA } },
    { TokenT::CMD_RRA,  { "RRA",   Color::MAGENTA } },
    { TokenT::CMD_RRC,  { "RRC",   Color::MAGENTA } },
    { TokenT::CMD_RRCA, { "RRCA",  Color::MAGENTA } },
    { TokenT::CMD_RRD,  { "RRD",   Color::MAGENTA } },
    { TokenT::CMD_RST,  { "RST",   Color::MAGENTA } },
    { TokenT::CMD_SBC,  { "SBC",   Color::MAGENTA } },
    { TokenT::CMD_SCF,  { "SCF",   Color::MAGENTA } },
    { TokenT::CMD_SET,  { "SET",   Color::MAGENTA } },
    { TokenT::CMD_SLA,  { "SLA",   Color::MAGENTA } },
    { TokenT::CMD_SRA,  { "SRA",   Color::MAGENTA } },
    { TokenT::CMD_SRL,  { "SRL",   Color::MAGENTA } },
    { TokenT::CMD_SUB,  { "SUB",   Color::MAGENTA } },
    { TokenT::CMD_XOR,  { "XOR",   Color::MAGENTA } },

    // Registers.val

    { TokenT::REG_AF, { "AF", Color::ORANGE } },
    { TokenT::REG_BC, { "BC", Color::ORANGE } },
    { TokenT::REG_DE, { "DE", Color::ORANGE } },
    { TokenT::REG_HL, { "HL", Color::ORANGE } },
    { TokenT::REG_IX, { "IX", Color::ORANGE } },
    { TokenT::REG_IY, { "IY", Color::ORANGE } },
    { TokenT::REG_SP, { "SP", Color::ORANGE } },
    { TokenT::REG_PC, { "PC", Color::ORANGE } },

    { TokenT::REG_AF$,{ "AF'",Color::DARK_YELLOW } },
    { TokenT::REG_BC$,{ "BC'",Color::DARK_YELLOW } },
    { TokenT::REG_DE$,{ "DE'",Color::DARK_YELLOW } },
    { TokenT::REG_HL$,{ "HL'",Color::DARK_YELLOW } },

    { TokenT::REG_A, { "A",   Color::ORANGE } },
    { TokenT::REG_B, { "B",   Color::ORANGE } },
    { TokenT::REG_C, { "C",   Color::ORANGE } },
    { TokenT::REG_D, { "D",   Color::ORANGE } },
    { TokenT::REG_E, { "E",   Color::ORANGE } },
    { TokenT::REG_H, { "H",   Color::ORANGE } },
    { TokenT::REG_L, { "L",   Color::ORANGE } },
    { TokenT::REG_I, { "I",   Color::ORANGE } },
    { TokenT::REG_R, { "R",   Color::ORANGE } },

    { TokenT::REG_A$,{ "A'",  Color::DARK_YELLOW } },
    { TokenT::REG_B$,{ "B'",  Color::DARK_YELLOW } },
    { TokenT::REG_C$,{ "C'",  Color::DARK_YELLOW } },
    { TokenT::REG_D$,{ "D'",  Color::DARK_YELLOW } },
    { TokenT::REG_E$,{ "E'",  Color::DARK_YELLOW } },
    { TokenT::REG_H$,{ "H'",  Color::DARK_YELLOW } },
    { TokenT::REG_L$,{ "L'", Color::DARK_YELLOW } },

    { TokenT::FLAG_C,  { "C",  Color::BLUE } },
    { TokenT::FLAG_M,  { "M",  Color::BLUE } },
    { TokenT::FLAG_NC, { "NC", Color::BLUE } },
    { TokenT::FLAG_NZ, { "NZ", Color::BLUE } },
    { TokenT::FLAG_P,  { "P",  Color::BLUE } },
    { TokenT::FLAG_PE, { "PE", Color::BLUE } },
    { TokenT::FLAG_PO, { "PO", Color::BLUE } },
    { TokenT::FLAG_Z,  { "Z",  Color::BLUE } },

    // Literals
    { TokenT::NUMBER,     { "", Color::RED } },
    { TokenT::STRING,     { "", Color::YELLOW } },
    { TokenT::IDENTIFIER, { "", Color::WHITE } },

    // Additional operations
    { TokenT::LEFT_SQUARE_BRACE,  { "", Color::RED } },
    { TokenT::RIGHT_SQUARE_BRACE, { "", Color::RED } },
  };
};
};