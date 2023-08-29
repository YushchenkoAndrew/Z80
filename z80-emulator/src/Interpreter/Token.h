#pragma once
#include "Defs.h"


struct Token {
  const TokenT token;
  const std::string lexeme;
  const std::string literal;

  const int32_t nPos;
  const int32_t line;

  void print() { printf("{ type: %d; lexeme: '%s'; literal: '%s' }\n", token, lexeme.c_str(), literal.c_str()); }
};

