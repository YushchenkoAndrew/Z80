#pragma once
#include "Lexer.h"

void Lexer::scan() {
  while (!isAtEnd()) {
    nStart = nCurr;

    const char c = advance();
    switch (c) {
      case ',': addToken(TokenT::COMMA); break;
      case ':': addToken(TokenT::COLON); break;
      case '(': addToken(TokenT::LEFT_BRACE); break;
      case ')': addToken(TokenT::RIGHT_BRACE); break;


      case '"': case '\'': string(c); break;

      case ';':
        while (peek() != '\n' && !isAtEnd()) advance();
        break;


      case '\n': nLine++; break;
      case ' ': case '\r': case '\t': break; // Ignore whitespace.

      default:
        if (isDigit(c)) number();
        else if (isAlpha(c)) identifier();
        else err = std::string("Unexpected char '") + std::to_string(c) + std::string("'.");

        break;
    }

    if (err.length()) break;
  }

  nStart = nCurr;
  addToken(TokenT::OP_EOF);
}
