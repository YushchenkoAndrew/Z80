#pragma once
#include "src/Defs.h"

namespace Zazy {
class Token;
typedef std::shared_ptr<Token> token_t;

class Token {
public:
  Token(TokenT t, std::string le, std::string li, int32_t col, int32_t line):
    token(t), lexeme(le), literal(li), col(col), line(line) {}

  static void header() {
    printf("LINE  COL  TOKEN   LEXEME     LITERAL\n");
    printf("---- ----  ------  ---------- -------\n");
  }

  void print() {
    printf("%4d %4d  %-6s  %-10s %s\n", line, col, TokenName(token), lexeme.c_str(), literal.c_str());
  }

public:
  const TokenT token;
  const std::string lexeme;
  const std::string literal;   // NUMBER/CHAR: decimal value; STRING: decoded bytes; ASM_BLOCK: raw text

  const int32_t col;
  const int32_t line;          // 1-based line in the preprocessed source
};
};
