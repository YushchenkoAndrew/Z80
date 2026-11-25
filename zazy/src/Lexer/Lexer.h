#pragma once
#include "Token.h"

namespace Zazy {

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#recognizing-lexemes
 *
 * Extended for the compiler: escapes in strings / chars, <<= >>= ->, // and block comments,
 * number suffixes and the raw `asm { ... }` block (captured verbatim as one ASM_BLOCK token).
 */
class Lexer {
public:
  Lexer(std::string text): src(text) {}

  token_t next();

  std::vector<std::string> errors;

private:
  token_t string();
  token_t character();
  token_t number();
  token_t identifier();
  token_t asmBlock(int32_t line, int32_t col);

  // returns the decoded char; `ok` is false for a malformed escape
  char escape(bool& ok);

  inline const char advance() { nCol++; return src[nCurr++]; }
  inline const char peek() { return isAtEnd() ? '\0' : src[nCurr]; }
  inline const char peekPrev() { return !nCurr ? '\0' : src[nCurr - 1]; }
  inline const char peekNext() { return nCurr + 1 >= (int32_t)src.length() ? '\0' : src[nCurr + 1]; }

  inline bool match(const char expected) {
    if (isAtEnd() || src[nCurr] != expected) return false;
    nCol++; nCurr++;
    return true;
  }

  token_t newToken(TokenT type, std::string literal = "") {
    const int32_t len = nCurr - nStart;
    return std::make_shared<Token>(Token(type, src.substr(nStart, len), literal, nStartCol, nStartLine));
  }

  token_t error(const std::string& msg) {
    errors.push_back("[Ln " + std::to_string(nStartLine) + " Col " + std::to_string(nStartCol) + "] Error: " + msg + "\n");
    return newToken(TokenT::INVALID);
  }

  inline bool isAtEnd() { return nCurr >= (int32_t)src.length(); }

private:
  std::string src;

  int32_t nStart = 0;      // index of the src, which is pointing to first char in the lexeme
  int32_t nCurr = 0;       // index of the src, which is pointing to the curr char
  int32_t nStartCol = 1;
  int32_t nStartLine = 1;

  int32_t nCol = 1;        // tracks current line position
  int32_t nLine = 1;       // tracks current line position
};
};
