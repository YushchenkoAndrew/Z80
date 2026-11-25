#pragma once
#include "lib/olcPixelGameEngine.h"
#include "src/Interpreter/Token.h"

namespace Syntax {
  typedef std::unordered_map<int32_t, std::unordered_map<int32_t, olc::Pixel>> HighlightT;

class Highlighter {
public:
  virtual ~Highlighter() = default;
  virtual std::list<std::string> Scan(std::string src) = 0;

  const HighlightT& GetHighlights() const { return highlights; }
  const std::vector<Interpreter::Token>& GetTokens() const { return tokens; }
 
protected:
  void Reset() { highlights.clear(); }
  void Mark(olc::Pixel color) {
    const int32_t len = curr - prev;

    if (!highlights.count(line)) highlights[line] = {};
    for (int32_t i = col - len; i < col; i++) highlights[line][i] = color;
  }

protected:
  std::string src;

  int32_t prev = 0; // index of the src, which is pointing to first char in the lexeme
  int32_t curr = 0; // index of the src, which is pointing to the curr char

  int32_t col = 0; // tracks current line position
  int32_t line = 0; // tracks current line position

  HighlightT highlights;
  std::vector<Interpreter::Token> tokens;
};

};