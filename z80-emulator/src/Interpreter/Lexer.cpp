#include "Lexer.h"

namespace Interpreter {
bool Lexer::scan(std::string text) {
  src = text; reset();

  while (!isAtEnd()) {
    nStart = nCurr;

    const char c = advance();
    switch (c) {
      case '#': addToken(TokenT::HASH); break;
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
       if (advance() != '.') { error("Unexpected character."); break; }
       addToken(TokenT::CONCATENATE); break;


      case '"': string(c); break;

      case ';':
        while (peek() != '\n' && !isAtEnd()) advance();
        addToken(~AnyType<Colors::DARK_GREY, ColorT>::GetValue());
        break;


      case '\n': nLine++; nCol = 1; break;
      case ' ': case '\r': case '\t': addToken(olc::BLANK); break; // Ignore whitespace.

      default:
        if (isDigit(c)) number();
        else if (isAlpha(c)) identifier();
        else { addToken(TokenT::NONE); error(std::string("Unexpected char '") + std::string(1, c) + std::string("'.")); }

        break;
    }
  }

  nStart = nCurr;
  addToken(TokenT::OP_EOF);
  return errors.size();
}
}; 