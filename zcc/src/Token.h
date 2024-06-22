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
    token(t), lexeme(le), literal(li), col(col), line(line) {

      AnyType<-1, int32_t>::GetValue() = token;
    }

  void print() { 
    // AnyType<-1, int32_t>::GetValue() = token;
    // std::string type = *foreach<KeywordList, AnyType<-1, int32_t>>::Key2Value();


    // if (!type.length()) printf("{ type: %d; lexeme: '%s'; literal: '%s' } Ln %d, Col %d\n", token, lexeme.c_str(), literal.c_str(), line, col);
    // else printf("{ type: '%s'; lexeme: '%s'; literal: '%s' } Ln %d, Col %d\n", type.c_str(), lexeme.c_str(), literal.c_str(), line, col);
  }

public:
  const TokenT token;
  const std::string lexeme;
  const std::string literal;

  const int32_t col;
  const int32_t line;
};
};