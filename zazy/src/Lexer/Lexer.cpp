#include "Lexer.h"

namespace Zcc {
token_t Lexer::next() {
  while (!isAtEnd()) {
    nStart = nCurr;

    const char c = advance();
    switch (c) {
      case ':': return newToken(TokenT::COLON);
      case ';': return newToken(TokenT::SEMICOLON);
      case ',': return newToken(TokenT::COMMA);
      case '?': return newToken(TokenT::QUESTION_MARK);
      case '.': return newToken(TokenT::OP_DOT);
      case '~': return newToken(TokenT::BIT_NOT);

      case '(': return newToken(TokenT::LEFT_BRACE);
      case ')': return newToken(TokenT::RIGHT_BRACE);
      case '[': return newToken(TokenT::LEFT_SQUARE_BRACE);
      case ']': return newToken(TokenT::RIGHT_SQUARE_BRACE);
      case '{': return newToken(TokenT::LEFT_CURLY_BRACE);
      case '}': return newToken(TokenT::RIGHT_CURLY_BRACE);

      case '*': 
        if (match('=')) return newToken(TokenT::ASSIGN_MUL);
        return newToken(TokenT::STAR);

      case '/': 
        if (match('=')) return newToken(TokenT::ASSIGN_DIV);
        return newToken(TokenT::OP_DIV);

      case '%': 
        if (match('=')) return newToken(TokenT::ASSIGN_MOD);
        return newToken(TokenT::OP_MOD);

      case '!': 
        if (match('=')) return newToken(TokenT::OP_NOT_EQUAL);
        return newToken(TokenT::OP_NOT);

      case '=': 
        if (match('=')) return newToken(TokenT::OP_EQUAL);
        return newToken(TokenT::ASSIGN);

      case '+': 
        if (match('+')) return newToken(TokenT::OP_INC);
        if (match('=')) return newToken(TokenT::ASSIGN_PLUS);
        return newToken(TokenT::OP_PLUS);

      case '-': 
        if (match('-')) return newToken(TokenT::OP_DEC);
        if (match('=')) return newToken(TokenT::ASSIGN_MINUS);
        if (match('>')) return newToken(TokenT::OP_DOT);
        return newToken(TokenT::OP_MINUS);

      case '^': 
        if (match('=')) return newToken(TokenT::ASSIGN_BIT_XOR);
        return newToken(TokenT::BIT_XOR);

      case '|': 
        if (match('|')) return newToken(TokenT::OP_OR);
        if (match('=')) return newToken(TokenT::ASSIGN_BIT_OR);
        return newToken(TokenT::BIT_OR);

      case '&': 
        if (match('&')) return newToken(TokenT::OP_AND);
        if (match('=')) return newToken(TokenT::ASSIGN_BIT_AND);
        return newToken(TokenT::AMPERSAND);

      case '>':
        if (match('=')) return newToken(TokenT::OP_MORE_OR_EQUAL);
        if (match('>')) return newToken(TokenT::BIT_RIGHT_SHIFT);
        return newToken(TokenT::OP_MORE);

      case '<':
        if (match('=')) return newToken(TokenT::OP_LESS_OR_EQUAL); 
        if (match('<')) return newToken(TokenT::BIT_LEFT_SHIFT); 
        return newToken(TokenT::OP_LESS);
      
      case '#':
        if (Utils::IsAlpha(c)) return macros();
        return newToken(TokenT::INVALID);

      case '"': return string(c);

      case '\'': 
        advance();
        if (!match('\'')) return newToken(TokenT::INVALID);
        return newToken(TokenT::CHAR, src.substr(nStart + 1, nCurr - nStart - 2));

      case '\n': nLine++; nCol = 1; break;
      case ' ': case '\r': case '\t': break; // Ignore whitespace.

      default:
        if (Utils::IsDigit(c)) return number();
        if (Utils::IsAlpha(c)) return identifier();
        return newToken(TokenT::INVALID);
    }
  }

  return newToken(TokenT::CMD_EOF);
}
}; 