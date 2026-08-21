#pragma once

#include "lib/olcPixelGameEngine.h"
#include "src/Interpreter/Defs.h"
#include "src/Interpreter/Token.h"
#include "src/Utils/Utils.h"


namespace Interpreter {

/**
 * This code was hardly expired by the book {Creating Interpreters}
 * Check out this link for more info: http://www.craftinginterpreters.com/scanning.html#recognizing-lexemes
 */
class Lexer {
public:
  Lexer() {}
  ~Lexer() { reset(); }

  bool scan(std::string text) {
    src = text; reset();

    while (!isAtEnd()) {
      prev = curr;

      const char c = advance();
      switch (c) {
        case '$': addToken(TokenT::IDENTIFIER); break;
        case ',': addToken(TokenT::COMMA); break;
        case ':': addToken(TokenT::COLON); break;
        case '(': addToken(TokenT::LEFT_BRACE); break;
        case ')': addToken(TokenT::RIGHT_BRACE); break;
        case '[': addToken(TokenT::LEFT_SQUARE_BRACE); break;
        case ']': addToken(TokenT::RIGHT_SQUARE_BRACE); break;

        case '+': addToken(TokenT::PLUS); break;
        case '-': addToken(TokenT::MINUS); break;
        case '|': addToken(TokenT::BIT_OR); break;
        case '&': addToken(TokenT::BIT_AND); break;
        case '^': addToken(TokenT::BIT_XOR); break;
        case '~': addToken(TokenT::BIT_NOT); break;

        case '.':
         if (advance() == '.') { addToken(TokenT::CONCATENATE); break; }
         else if (Utils::IsAlpha(peekPrev())) { identifier(); break; }
         else error("Unexpected character."); break;

        case '>':
         if (advance() != '>') { error("Unexpected character."); break; }
         addToken(TokenT::RIGHT_SHIFT); break;

        case '<':
         if (advance() != '<') { error("Unexpected character."); break; }
         addToken(TokenT::LEFT_SHIFT); break;


        case '"': string(c); break;

        case ';':
          while (peek() != '\n' && !isAtEnd()) advance();
          addHighlight(olc::DARK_GREY);
          break;


        case '\n': line++; col = 0; break;
        case ' ': case '\r': case '\t': break; // Ignore whitespace.
        case '#':
          if (peekNext() != ' ') identifier();
          else error("Unexpected character."); 
          break;

        default:
          if (Utils::IsDigit(c)) number();
          else if (isAlpha(c)) identifier();
          else { addToken(TokenT::NONE); error(std::string("Unexpected char '") + std::string(1, c) + std::string("'.")); }
          break;
      }
    }

    prev = curr;
    addToken(TokenT::OP_EOF);
    return errors.size();
  }


private:
  inline void reset() {
    prev = 0; curr = 0; col = 0; line = 0;
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

    const auto& type = Token::GetToken(src.substr(prev, curr - prev));

    if (type != TokenT::NONE) addToken(type);
    else addToken(TokenT::IDENTIFIER);
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