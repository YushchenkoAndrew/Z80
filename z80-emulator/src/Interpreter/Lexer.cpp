#include "Lexer.h"

namespace Interpreter {
bool Lexer::scan(std::string text) {
  src = text; reset();

  while (!isAtEnd()) {
    nStart = nCurr;

    const char c = advance();
    switch (c) {
      case ',': addToken(TokenT::COMMA); break;
      case ':': addToken(TokenT::COLON); break;
      case '(': addToken(TokenT::LEFT_BRACE); break;
      case ')': addToken(TokenT::RIGHT_BRACE); break;

      case '+': addToken(TokenT::PLUS); break;
      case '-': addToken(TokenT::MINUS); break;
      case '|': addToken(TokenT::BIT_OR); break;
      case '&': addToken(TokenT::BIT_AND); break;
      case '^': addToken(TokenT::BIT_XOR); break;
      case '~': addToken(TokenT::BIT_NOT); break;

      case '.':
       if (advance() != '.') { err = "Unexpected character."; break; }
       addToken(TokenT::CONCATENATE); break;


      case '"': case '\'': string(c); break;

      case ';':
        while (peek() != '\n' && !isAtEnd()) advance();
        addToken(olc::DARK_GREY);
        break;


      case '\n': nLine++; nCol = 1; break;
      case ' ': case '\r': case '\t': addToken(); break; // Ignore whitespace.

      default:
        if (isDigit(c)) number();
        else if (isAlpha(c)) identifier();
        else err = std::string("Unexpected char '") + std::to_string(c) + std::string("'.");

        break;
    }

    if (err.length()) return true;
  }

  nStart = nCurr;
  addToken(TokenT::OP_EOF);
  return err.length();
}
}; 