#pragma once
#include "Token.h"
#include "src/Defs.h"
#include <cstdint>
#include <unordered_map>
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
  Lexer() {}
  ~Lexer() { reset(); }

  bool scan(std::string src); 


private:
  inline void reset() {
    prev = 0; curr = 0; col = 1; line = 1;
    tokens.clear(); errors.clear();
  }

  void string(const char &c) {
    while (peek() != c) {
      if (peek() == '\n' || peek() == '\r') break;
      advance();
    }

    if (peek() != c) { addToken(TokenT::NONE); return error("Unclosed string.");  } 
    

    advance(); // closing quote
    addToken(TokenT::STRING, src.substr(prev + 1, curr - prev - 2));
  }

  void number() {
    if (peekPrev() == '0' && match<6>({ 'X', 'x', 'O', 'o', 'B', 'b' })) {
      int32_t base = 10;

      switch (peekPrev()) {
        case 'X': case 'x': base = 16; while (Utils::IsHexDigit(peek())) advance(); break;
        case 'O': case 'o': base = 8;  while (Utils::IsOctDigit(peek())) advance(); break;
        case 'B': case 'b': base = 2;  while (Utils::IsBinDigit(peek())) advance(); break;
      }

      const auto nStart = this->prev + 2;
      return addToken(TokenT::NUMBER, std::to_string(std::stoul(src.substr(nStart, curr - nStart), nullptr, base)));
    }
    
    while (Utils::IsDigit(peek())) advance();
    return addToken(TokenT::NUMBER, std::to_string(std::stoul(src.substr(prev, curr - prev))));
  }

  void identifier() {
    while (isAlphaNumeric(peek())) advance();

    AnyType<-1, std::string>::GetValue() = src.substr(prev, curr - prev);
    int32_t type = foreach<KeywordList, AnyType<-1, std::string>>::Value2Key();

    if (type == -1) addToken(TokenT::IDENTIFIER);
    else addToken(static_cast<TokenT>(type));
  }

  inline const char advance() { col++; return src[curr++]; }
  inline const char peek() { return isAtEnd() ? '\0' : src[curr]; }
  inline const char peekPrev() { return !curr ? '\0' : src[curr - 1]; }
  inline const char peekNext() { return curr + 1 >= src.length() ? '\0' : src[curr + 1]; }

  template<int32_t T>
  inline bool match(std::array<const char, T> arr) {
    for (auto& el : arr) if (match(el)) return true;
    return false;
  }

  inline bool match(const char expected) {
    if (isAtEnd()) return false;
    if (src[curr] != expected) return false;

    col++; curr++;
    return true;
  }

  void addToken(TokenT type, std::string literal = "") {
    const int32_t len = curr - prev;
    const int32_t start = col - len;

    const auto& [_, color] = Token::GetMetadata(type);
    tokens.push_back({ type, src.substr(prev, len), literal,  start + 1, line + 1 });
    addHighlight(color);
  }

  void addHighlight(olc::Pixel c) {
    const int32_t len = curr - prev;

    // FIX issue with ivalid highlight cased by line & i
    if (!highlights.count(line)) highlights[line] = {};
    for (int32_t i = col - len; i < col; i++) highlights[line][i] = c;
  }

  inline bool isAtEnd() { return curr >= src.length(); }
  inline bool isAlpha(const char &c) { return Utils::IsAlpha(c) || c == '.' || c == '#' || c == '\''; }
  inline bool isAlphaNumeric(const char &c) { return isAlpha(c) || Utils::IsDigit(c); }


  inline void error(std::string message) {
    errors.push_back("[Ln " + std::to_string(line) + " Col " + std::to_string(curr) + "] Error: " + message + "\n");
  }


private:
  std::string src;

  int32_t prev = 0; // index of the src, which is pointing to first char in the lexeme
  int32_t curr = 0; // index of the src, which is pointing to the curr char

  int32_t col = 0; // tracks current line position
  int32_t line = 0; // tracks current line position

public:
  std::vector<Token> tokens;
  std::unordered_map<int32_t, std::unordered_map<int32_t, olc::Pixel>> highlights;

  std::vector<std::string> errors;
};

};