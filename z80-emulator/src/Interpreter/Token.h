#pragma once
#include "Defs.h"
#include "src/Defs.h"
#include <utility>

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
    AnyType<-1, int32_t>::GetValue() = token;
    std::string type = *foreach<KeywordList, AnyType<-1, int32_t>>::Key2Value();


    if (!type.length()) printf("{ type: %d; lexeme: '%s'; literal: '%s' } Ln %d, Col %d\n", token, lexeme.c_str(), literal.c_str(), line, col);
    else printf("{ type: '%s'; lexeme: '%s'; literal: '%s' } Ln %d, Col %d\n", type.c_str(), lexeme.c_str(), literal.c_str(), line, col);
  }

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
    { TokenT::NONE,       { "",         olc::RED } },
    { TokenT::OP_INCLUDE, { "#include", olc::CYAN } },

    { TokenT::OP_ORG, { "ORG", olc::CYAN } },
    { TokenT::OP_DB,  { "DB",  olc::CYAN } },
    { TokenT::OP_DW,  { "DW",  olc::CYAN } },
    { TokenT::OP_EQU, { "EQU", olc::CYAN } },


    { TokenT::CMD_ADC,  { "ADC",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_ADD,  { "ADD",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_AND,  { "AND",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_BIT,  { "BIT",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CALL, { "CALL",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CCF,  { "CCF",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CP,   { "CP",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CPD,  { "CPD",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CPDR, { "CPDR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CPI,  { "CPI",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CPIR, { "CPIR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_CPL,  { "CPL",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_DAA,  { "DAA",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_DEC,  { "DEC",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_DI,   { "DI",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_DJNZ, { "DJNZ",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_EI,   { "EI",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_EX,   { "EX",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_EXX,  { "EXX",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_HALT, { "HALT",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_IM,   { "IM",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_IN,   { "IN",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_INC,  { "INC",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_IND,  { "IND",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_INDR, { "INDR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_INI,  { "INI",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_INIR, { "INIR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_JP,   { "JP",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_JR,   { "JR",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_LD,   { "LD",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_LDD,  { "LDD",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_LDDR, { "LDDR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_LDI,  { "LDI",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_LDIR, { "LDIR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_NEG,  { "NEG",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_NOP,  { "NOP",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_OR,   { "OR",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_OTDR, { "OTDR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_OTIR, { "OTIR",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_OUT,  { "OUT",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_OUTD, { "OUTD",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_OUTI, { "OUTI",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_POP,  { "POP",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_PUSH, { "PUSH",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RES,  { "RES",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RET,  { "RET",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RETI, { "RETI",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RETN, { "RETN",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RL,   { "RL",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RLA,  { "RLA",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RLC,  { "RLC",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RLCA, { "RLCA",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RLD,  { "RLD",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RR,   { "RR",    olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RRA,  { "RRA",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RRC,  { "RRC",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RRCA, { "RRCA",  olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RRD,  { "RRD",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_RST,  { "RST",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_SBC,  { "SBC",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_SCF,  { "SCF",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_SET,  { "SET",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_SLA,  { "SLA",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_SRA,  { "SRA",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_SRL,  { "SRL",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_SUB,  { "SUB",   olc::Pixel(0xCC, 0x75, 0xEC) } },
    { TokenT::CMD_XOR,  { "XOR",   olc::Pixel(0xCC, 0x75, 0xEC) } },

    // Registers.val

    { TokenT::REG_AF, { "AF", olc::Pixel(0xE0, 0x9C, 0x5F)} },
    { TokenT::REG_BC, { "BC", olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_DE, { "DE", olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_HL, { "HL", olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_IX, { "IX", olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_IY, { "IY", olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_SP, { "SP", olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_PC, { "PC", olc::Pixel(0xE0, 0x9C, 0x5F) } },

    { TokenT::REG_AF$,{ "AF'",olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_BC$,{ "BC'",olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_DE$,{ "DE'",olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_HL$,{ "HL'",olc::Pixel(0xD7, 0xAD, 0x5D) } },

    { TokenT::REG_A, { "A",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_B, { "B",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_C, { "C",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_D, { "D",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_E, { "E",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_H, { "H",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_L, { "L",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_I, { "I",   olc::Pixel(0xE0, 0x9C, 0x5F) } },
    { TokenT::REG_R, { "R",   olc::Pixel(0xE0, 0x9C, 0x5F) } },

    { TokenT::REG_A$,{ "A'",  olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_B$,{ "B'",  olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_C$,{ "C'",  olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_D$,{ "D'",  olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_E$,{ "E'",  olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_H$,{ "H'",  olc::Pixel(0xD7, 0xAD, 0x5D) } },
    { TokenT::REG_L$,{ "L'",  olc::Pixel(0xD7, 0xAD, 0x5D) } },

    { TokenT::FLAG_C,  { "C",  olc::Pixel(0x4C, 0xB8, 0xFE) } },
    { TokenT::FLAG_M,  { "M",  olc::Pixel(0x4C, 0xB8, 0xFE) } },
    { TokenT::FLAG_NC, { "NC", olc::Pixel(0x4C, 0xB8, 0xFE) } },
    { TokenT::FLAG_NZ, { "NZ", olc::Pixel(0x4C, 0xB8, 0xFE) } },
    { TokenT::FLAG_P,  { "P",  olc::Pixel(0x4C, 0xB8, 0xFE) } },
    { TokenT::FLAG_PE, { "PE", olc::Pixel(0x4C, 0xB8, 0xFE) } },
    { TokenT::FLAG_PO, { "PO", olc::Pixel(0x4C, 0xB8, 0xFE) } },
    { TokenT::FLAG_Z,  { "Z",  olc::Pixel(0x4C, 0xB8, 0xFE) } },

    // Literals
    { TokenT::NUMBER,     { "", olc::Pixel(0XFF, 0x86, 0x63) } },
    { TokenT::STRING,     { "", olc::Pixel(0xEC, 0xCE, 0x8A) } },
    { TokenT::IDENTIFIER, { "", olc::Pixel(0xDB, 0xE5, 0xD5) } },

    // Additional operations
    { TokenT::LEFT_SQUARE_BRACE,  { "", olc::Pixel(0XFF, 0x86, 0x63) } },
    { TokenT::RIGHT_SQUARE_BRACE, { "", olc::Pixel(0XFF, 0x86, 0x63) } },
  };
};
};