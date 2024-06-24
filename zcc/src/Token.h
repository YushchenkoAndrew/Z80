#pragma once
#include "Defs.h"

namespace Zcc {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */
class Token {
public:
  Token(TokenT t, std::string le, std::string li, int32_t col, int32_t line):
    token(t), lexeme(le), literal(li), col(col), line(line) {}

  static void header() { 
    printf("LINE  COL  TOKEN   LEXEME     LITERAL\n");
    printf("---- ----  ------  ---------- -------\n");
  }

  void print() { 
    AnyType<-1, int32_t>::GetValue() = token;
    std::string type = *foreach<TokenList, AnyType<-1, int32_t>>::Key2Value();

    printf("%4d %4d  %-6s  %-10s %s\n", line, col, Utils::Uppercase(type).c_str(), lexeme.c_str(), literal.c_str());
  }

public:
  const TokenT token;
  const std::string lexeme;
  const std::string literal;

  const int32_t col;
  const int32_t line;
};
};