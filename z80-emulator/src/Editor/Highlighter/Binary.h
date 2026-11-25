#pragma once
#include "lib/olcPixelGameEngine.h"
#include "src/Defs.h"
#include "src/Editor/Highlighter/Defs.h"
#include "src/Editor/Highlighter/Highlighter.h"
#include "src/Utils/Utils.h"

namespace Syntax {
  typedef std::unordered_map<int32_t, std::unordered_map<int32_t, olc::Pixel>> HighlightT;

class Binary : public Highlighter {
public:
  Binary() {}

  std::list<std::string> Scan(std::string text) override {
    src = text; Reset();

    while (!isAtEnd()) {
      prev = curr;

      const char c = advance();
      if (c == '\n') line++, col = 0;
      else if (!prev && !(curr = prev) && match(Header::BINARY)) Mark(Color::RED);
      else if (Utils::IsHexDigit(c)) number();
    }

    prev = curr;
    return {};
  }

private:
  inline void Reset() {
    prev = 0; curr = 0; tokens.clear(); 
    Syntax::Highlighter::Reset();
  }

  void number() {
    while (Utils::IsHexDigit(peek())) advance();
    Mark(curr - prev == 2 ? Color::DARK_GREY : Color::ORANGE);
  }

  inline const char advance() { col++; return src[curr++]; }
  inline const char peek() { return isAtEnd() ? '\0' : src[curr]; }
  inline const char peekPrev() { return !curr ? '\0' : src[curr - 1]; }
  inline const char peekNext() { return curr + 1 >= src.length() ? '\0' : src[curr + 1]; }

  inline bool match(std::string str) {
    auto prev = curr;
    for (auto& c : str) {
      printf("'%c' <> '%c' \n", c, peek());
      if (!match(c)) return curr = prev, false;
    }
    return true;
  }

  inline bool match(const char expected) {
    if (isAtEnd()) return false;
    if (src[curr] != expected) return false;
    return col++, curr++, true;
  }

  inline bool isAtEnd() { return curr >= src.length(); }
};

};