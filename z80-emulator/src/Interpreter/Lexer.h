#pragma once
#include "Token.h"
#include <vector>

class Lexer {
public:
  Lexer(std::string src): src(src) { Defs::Init(); }

  void scan(); 


private:
  void string(const char c) {
    while (!(peek() == c || isAtEnd())) {
      if (peek() == '\n' || peek() == '\r') nLine++;
      advance();
    }

    if (isAtEnd()) { err = "Unclosed string."; return; } 

    advance(); // closing quote
    addToken(TokenT::STRING, src.substr(nStart + 1, nCurr - nStart - 1));
  }

  void number() {
    while (isDigit(peek())) advance();

    int32_t base = 10;
    switch (peek()) {
      case 'X': case 'x': base = 16; break;
      case 'O': case 'o': base = 8; break;
      case 'B': case 'b': base = 2; break;
    }

    if (base != 10 && isDigit(peekNext())) {
      advance(); // Consume 'x' || 'o' || 'b'
      while (isDigit(peek()) || (base == 16 && isHexDigit(peek()))) advance();
    }

    addToken(TokenT::NUMBER, std::to_string(std::stoul(src.substr(nStart, nCurr - nStart), nullptr, base)));
  }

  void identifier() {
    while (isAlphaNumeric(peek())) advance();

    AnyType<-1, std::string>::GetValue() = src.substr(nStart, nCurr - nStart);
    int32_t type = foreach<KeywordList, AnyType<-1, std::string>>::Index();

    if (type == -1) addToken(TokenT::IDENTIFIER);
    else addToken(static_cast<TokenT>(type));
  }

  inline const char advance() { return src[nCurr++]; }
  inline const char peek() { return isAtEnd() ? '\0' : src[nCurr]; }
  inline const char peekPrev() { return !nCurr ? '\0' : src[nCurr - 1]; }
  inline const char peekNext() { return nCurr + 1 >= src.length() ? '\0' : src[nCurr + 1]; }

  inline bool match(const char expected) {
    if (isAtEnd()) return false;
    if (src[nCurr] != expected) return false;

    nCurr++;
    return true;
  }

  void addToken(TokenT type, std::string literal = "") {
    vTokens.push_back(Token { type, src.substr(nStart, nCurr - nStart), literal, nStart, nLine });
  }

  inline bool isAtEnd() { return nCurr >= src.length(); }
  inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
  inline bool isHexDigit(char c) { return (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
  inline bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
  inline bool isAlphaNumeric(char c) { return isAlpha(c) || isDigit(c); }

  
// public:
//   std::string errorMessage() {
//     if (!error) return "";
//     return sprintf(nullptr, "[Ln %d Col %d] Error: %s", nLine, nCurr, error);
//   }


private:
  const std::string src;

  int32_t nStart = 0; // index of the src, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the src, which is pointing to the curr char
  int32_t nLine = 1; // tracks current line position


public:
  std::string err;
  std::vector<Token> vTokens;
};
