#pragma once
#include "src/Token.h"

namespace Zazy {

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#recognizing-lexemes
 */
class Lexer {
public:
  Lexer(std::string text): src(text)  {}

  token_t next(); 

private:
  token_t string(const char &c) {
    while (peek() != c) {
      if (peek() == '\n' || peek() == '\r') break;
      advance();
    }

    if (peek() != c) return newToken(TokenT::INVALID); 

    advance(); // closing quote
    return newToken(TokenT::STRING, src.substr(nStart + 1, nCurr - nStart - 2));
  }

  token_t number() {
    auto gettype = [&](unsigned long& num) {
      if (num >= 0 && num <= UINT8_MAX)  return TokenT::CHAR;
      if (num >= 0 && num <= UINT16_MAX) return TokenT::SHORT;
      if (num >= 0 && num <= UINT32_MAX) return TokenT::INT;

      return TokenT::INVALID;
    };

    if (peekPrev() == '0' && match<6>({ 'X', 'x', 'O', 'o', 'B', 'b' })) {
      int32_t base = 10;

      switch (peekPrev()) {
        case 'X': case 'x': base = 16; while (Utils::IsHexDigit(peek())) advance(); break;
        case 'O': case 'o': base = 8;  while (Utils::IsOctDigit(peek())) advance(); break;
        case 'B': case 'b': base = 2;  while (Utils::IsBinDigit(peek())) advance(); break;
      }

      const auto nStart = this->nStart + 2;
      auto num = std::stoul(src.substr(nStart, nCurr - nStart), nullptr, base);
      return newToken(gettype(num), std::to_string(num));
    }
    
    while (Utils::IsDigit(peek())) advance();

    auto num = std::stoul(src.substr(nStart, nCurr - nStart));
    return newToken(gettype(num), std::to_string(num));
  }

  token_t identifier() {
    while (Utils::IsAlpha(peek()) || Utils::IsDigit(peek())) advance();

    AnyType<-1, std::string>::GetValue() = src.substr(nStart, nCurr - nStart);
    int32_t type = foreach<KeywordList, AnyType<-1, std::string>>::Value2Key();

    if (type == -1) return newToken(TokenT::IDENTIFIER);
    return newToken(static_cast<TokenT>(type));
  }

  token_t macros() {
    while (Utils::IsAlpha(peek()) || Utils::IsDigit(peek())) advance();

    AnyType<-1, std::string>::GetValue() = src.substr(nStart, nCurr - nStart);
    int32_t type = foreach<CommandList, AnyType<-1, std::string>>::Value2Key();

    if (type == -1) return newToken(TokenT::INVALID);
    return newToken(static_cast<TokenT>(type));
  }

  inline const char advance() { nCol++; return src[nCurr++]; }
  inline const char peek() { return isAtEnd() ? '\0' : src[nCurr]; }
  inline const char peekPrev() { return !nCurr ? '\0' : src[nCurr - 1]; }
  inline const char peekNext() { return nCurr + 1 >= src.length() ? '\0' : src[nCurr + 1]; }

  template<int32_t T>
  inline bool match(std::array<const char, T> arr) {
    for (auto& el : arr) if (match(el)) return true;
    return false;
  }

  inline bool match(const char expected) {
    if (isAtEnd()) return false;
    if (src[nCurr] != expected) return false;

    nCol++; nCurr++;
    return true;
  }

  token_t newToken(TokenT type, std::string literal = "") {
    const int32_t len = nCurr - nStart;
    return std::make_shared<Token>(Token(type, src.substr(nStart, len), literal, nCol - len, nLine));
  }

  inline bool isAtEnd() { return nCurr >= src.length(); }

private:
  std::string src;

  int32_t nStart = 0; // index of the src, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the src, which is pointing to the curr char

  int32_t nCol = 1; // tracks current line position
  int32_t nLine = 1; // tracks current line position
};
};