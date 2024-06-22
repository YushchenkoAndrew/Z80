#include "Preprocessor.h"

namespace Zcc {
void Preprocessor::parse() {
    nStart = nCurr; 

    const char c = advance();
    switch (c) {
      case '"': string(c); break;
      case '/':
        if (peek() == '/') comments();
        break;

      case '\n': nLine++; nCol = 1; break;
      case ' ': case '\r': case '\t': break; // Ignore whitespace.

      case '#':
        if (peek() != ' ') macros();
        else error("Unexpected character."); 
        break;

      default:
        if (isAlpha(c)) identifier();
        // else { addToken(TokenT::NONE); error(std::string("Unexpected char '") + std::string(1, c) + std::string("'.")); }
        break;
    }
}
}; 