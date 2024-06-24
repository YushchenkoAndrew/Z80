#pragma once
#include "src/Preprocessor/Preprocessor.h"

namespace Zcc {

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#recognizing-lexemes
 */
class Lexer {
public:
  Lexer(std::string& text): src(text)  {}
  ~Lexer() { reset(); }

  bool scan(); 

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
    addToken(TokenT::LEFT_CURLY_BRACE);

    for (int32_t i = 0; i < nCurr - nStart - 2; i++) {
      if (i) addToken(TokenT::COMMA);
      addToken(TokenT::UNSIGNED_CHAR, std::to_string(src[nStart + i + 1]));
    }

    addToken(TokenT::RIGHT_CURLY_BRACE);
  }

  void number() {
    auto gettype = [&](unsigned long& num) {
      if (num >= 0) {
        if (num <= UINT8_MAX)  return TokenT::UNSIGNED_CHAR;
        if (num <= UINT16_MAX) return TokenT::UNSIGNED_SHORT;
        if (num <= UINT32_MAX) return TokenT::UNSIGNED_INT;
      }

      if (num >= INT8_MIN  && num <= INT8_MAX)  return TokenT::CHAR;
      if (num >= INT16_MIN && num <= INT16_MAX) return TokenT::SHORT;
      if (num >= INT32_MIN && num <= INT32_MAX) return TokenT::INT;

      error("Number is exceeded allowed range.");
      return TokenT::NONE; 
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
      return addToken(gettype(num), std::to_string(num));
    }
    
    while (Utils::IsDigit(peek())) advance();

    auto num = std::stoul(src.substr(nStart, nCurr - nStart));
    return addToken(gettype(num), std::to_string(num));
  }

  void identifier() {
    while (isAlphaNumeric(peek())) advance();

    AnyType<-1, std::string>::GetValue() = src.substr(nStart, nCurr - nStart);
    int32_t type = foreach<KeywordList, AnyType<-1, std::string>>::Value2Key();

    if (type == -1) addToken(TokenT::IDENTIFIER);
    else addToken(static_cast<TokenT>(type));
  }

  void assign(TokenT op) {
    auto identifier = tokens.back();

    addToken(TokenT::ASSIGN); tokens.push_back(identifier); addToken(op);
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

  inline bool isAtEnd() { return nCurr >= src.length(); }
  inline bool isAlpha(const char &c) { return Utils::IsAlpha(c) || c == '.' || c == '\''; }
  inline bool isAlphaNumeric(const char &c) { return isAlpha(c) || Utils::IsDigit(c); }


  inline void error(std::string message) {
    errors.push_back("[Ln " + std::to_string(nLine) + " Col " + std::to_string(nCurr) + "] Error: " + message + "\n");
  }

public:
  void debug() {
    Token::header();
    for (auto& t : tokens) t->print();
  }

private:
  const std::string& src;

  int32_t nStart = 0; // index of the src, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the src, which is pointing to the curr char

  int32_t nCol = 1; // tracks current line position
  int32_t nLine = 1; // tracks current line position

public:
  std::vector<std::shared_ptr<Token>> tokens;

  std::vector<std::string> errors;
};

};