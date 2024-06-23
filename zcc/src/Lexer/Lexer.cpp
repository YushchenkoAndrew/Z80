#include "Lexer.h"

namespace Zcc {
bool Lexer::scan() {
  reset();

  while (!isAtEnd()) {
    nStart = nCurr;

    const char c = advance();
    switch (c) {
      case ':': addToken(TokenT::COLON); break;
      case ';': addToken(TokenT::SEMICOLON); break;
      case ',': addToken(TokenT::COMMA); break;
      case '?': addToken(TokenT::QUESTION_MARK); break;

      case '.': 
        if (peek() != '.') addToken(TokenT::OP_DOT);
        else if (match('.') && match('.')) addToken(TokenT::ELLIPSIS);
        else error("Malformed ellipsis .");
        break;

      case '(': addToken(TokenT::LEFT_BRACE); break;
      case ')': addToken(TokenT::RIGHT_BRACE); break;
      case '[': addToken(TokenT::LEFT_SQUARE_BRACE); break;
      case ']': addToken(TokenT::RIGHT_SQUARE_BRACE); break;
      case '{': addToken(TokenT::LEFT_CURLY_BRACE); break;
      case '}': addToken(TokenT::RIGHT_CURLY_BRACE); break;

      case '~': addToken(TokenT::BIT_NOT); break;
      case '^': 
        if (match('=')) assign(TokenT::BIT_XOR);
        else addToken(TokenT::BIT_XOR);
        break;

      case '*': 
        if (match('=')) assign(TokenT::STAR);
        else addToken(TokenT::STAR);
        break;

      case '/': 
        if (match('=')) assign(TokenT::OP_DIV);
        else addToken(TokenT::OP_DIV);
        break;

      case '%': 
        if (match('=')) assign(TokenT::OP_MOD);
        else addToken(TokenT::OP_MOD);
        break;

      case '!': 
        if (match('=')) addToken(TokenT::OP_NOT_EQUAL);
        else addToken(TokenT::OP_NOT);
        break;

      case '=': 
        if (match('=')) addToken(TokenT::OP_EQUAL);
        else addToken(TokenT::ASSIGN);
        break;

      case '+': 
        if (match('+')) addToken(TokenT::OP_INC);
        else if (match('=')) assign(TokenT::OP_PLUS);
        else addToken(TokenT::OP_PLUS);
        break;

      case '-': 
        if (match('-')) addToken(TokenT::OP_DEC);
        else if (match('=')) assign(TokenT::OP_MINUS);
        else if (match('>')) addToken(TokenT::OP_ARROW);
        else addToken(TokenT::OP_MINUS);
        break;

      case '|': 
        if (match('|')) addToken(TokenT::OP_OR);
        else if (match('=')) assign(TokenT::BIT_OR);
        else addToken(TokenT::BIT_OR);
        break;

      case '&': 
        if (match('&')) addToken(TokenT::OP_AND);
        else if (match('=')) assign(TokenT::AMPERSAND);
        else addToken(TokenT::AMPERSAND);
        break;

      case '>':
        if (match('=')) addToken(TokenT::OP_MORE_OR_EQUAL);
        else if (match('>')) addToken(TokenT::BIT_RIGHT_SHIFT);
        else addToken(TokenT::OP_MORE);
        break;

      case '<':
        if (match('=')) addToken(TokenT::OP_LESS_OR_EQUAL); 
        else if (match('<')) addToken(TokenT::BIT_LEFT_SHIFT); 
        else addToken(TokenT::OP_LESS);
        break;

      case '"': string(c); break;

      case '\'': 
        advance();
        if (!match('\'')) { addToken(TokenT::NONE); error("Unclosed char."); } 
        else addToken(TokenT::CHAR, src.substr(nStart + 1, nCurr - nStart - 2));
        break;

      case '\n': nLine++; nCol = 1; break;
      case ' ': case '\r': case '\t': break; // Ignore whitespace.

      default:
        if (Utils::IsDigit(c)) number();
        else if (isAlpha(c)) identifier();
        else { addToken(TokenT::NONE); error(std::string("Unexpected char '") + std::string(1, c) + std::string("'.")); }

        break;
    }
  }

  nStart = nCurr;
  addToken(TokenT::CMD_EOF);
  return errors.size();
}
}; 