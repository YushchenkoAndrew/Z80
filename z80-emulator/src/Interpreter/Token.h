#pragma once
#include "Defs.h"

namespace Interpreter {
/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#lexemes-and-tokens
 */
struct Token {
  const TokenT token;
  const std::string lexeme;
  const std::string literal;

  const int32_t nPos;
  const int32_t line;

  void print() { 
    AnyType<-1, int32_t>::GetValue() = token;
    std::string type = foreach<KeywordList, AnyType<-1, int32_t>>::Key2Value();

    if (!type.length()) printf("{ type: %d; lexeme: '%s'; literal: '%s' }\n", token, lexeme.c_str(), literal.c_str());
    else printf("{ type: '%s'; lexeme: '%s'; literal: '%s' }\n", type.c_str(), lexeme.c_str(), literal.c_str());
  }
};
};