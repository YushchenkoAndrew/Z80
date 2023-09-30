#pragma once
#include "Token.h"
#include <vector>
#include <array>
#include <sstream>


namespace Interpreter {

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#recognizing-lexemes
 */
class Lexer {
public:
  Lexer(): bFullTokenSupport(false) {}
  Lexer(bool f): bFullTokenSupport(f) {}

  ~Lexer() { reset(); }

  bool scan(std::string src); 


private:
  inline void reset() {
    nStart = 0; nCurr = 0; nCol = 1; nLine = 1;
    tokens.clear(); errors.clear();
  }

  void string(const char &c) {
    while (peek() != c) {
      if (peek() == '\n' || peek() == '\r') break;
      advance();
    }

    if (peek() != c) { addToken(TokenT::NONE); return error("Unclosed string.");  } 
    

    advance(); // closing quote
    addToken(TokenT::STRING, src.substr(nStart + 1, nCurr - nStart - 2));
  }

  void number() {
    int32_t base = 10;

    if (peekPrev() == '0' && match<6>({ 'X', 'x', 'O', 'o', 'B', 'b' })) {
      switch (peekPrev()) {
        case 'X': case 'x': base = 16; while (isHexDigit(peek())) advance(); break;
        case 'O': case 'o': base = 8;  while (isOctDigit(peek())) advance(); break;
        case 'B': case 'b': base = 2;  while (isBinDigit(peek())) advance(); break;
      }
    } else while (isDigit(peek())) advance();

    addToken(TokenT::NUMBER, std::to_string(std::stoul(src.substr(nStart, nCurr - nStart), nullptr, base)));
  }

  void identifier() {
    while (isAlphaNumeric(peek())) advance();

    AnyType<-1, std::string>::GetValue() = src.substr(nStart, nCurr - nStart);
    int32_t type = foreach<KeywordList, AnyType<-1, std::string>>::Value2Key();

    if (type == -1) addToken(TokenT::IDENTIFIER);
    else addToken(static_cast<TokenT>(type));
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

  void addToken(TokenT type, std::string literal = "") {
    const int32_t len = nCurr - nStart;
    tokens.push_back(std::make_shared<Token>(Token(type, src.substr(nStart, len), literal, nCol - len, nLine)));
  }

  void addToken(olc::Pixel c) {
    if (!bFullTokenSupport) return;
    const int32_t len = nCurr - nStart;
    tokens.push_back(std::make_shared<Token>(Token(TokenT::OP_NONE, src.substr(nStart, len), "", nCol - len, nLine, c)));
  }

  inline bool isAtEnd() { return nCurr >= src.length(); }
  inline bool isDigit(const char &c) { return c >= '0' && c <= '9'; }
  inline bool isBinDigit(const char &c) { return c == '0' || c == '1'; }
  inline bool isOctDigit(const char &c) { return c >= '0' && c <= '7'; }
  inline bool isHexDigit(const char &c) { return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
  inline bool isAlpha(const char &c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '\''; }
  inline bool isAlphaNumeric(const char &c) { return isAlpha(c) || isDigit(c); }


  inline void error(std::string message) {
    std::stringstream ss;
    ss << "[Ln " << nLine << " Col " << nCurr << "] Error: " << message << "\n";
    errors.push_back(ss.str());
  }


private:
  std::string src;
  bool bFullTokenSupport;

  int32_t nStart = 0; // index of the src, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the src, which is pointing to the curr char

  int32_t nCol = 1; // tracks current line position
  int32_t nLine = 1; // tracks current line position

public:
  std::vector<std::shared_ptr<Token>> tokens;

  std::vector<std::string> errors;
};

};