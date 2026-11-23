#pragma once
#include "lib/olcPixelGameEngine.h"
#include "src/Interpreter/Token.h"

namespace Syntax {

class Highlighter {
public:
  virtual ~Highlighter() = default;
  virtual std::list<std::string> Scan(std::string src) = 0;

  const std::vector<Interpreter::Token>& GetTokens() const { return tokens; }
  const std::unordered_map<int32_t, std::unordered_map<int32_t, olc::Pixel>>& GetHighlights() const { return highlights; }
 
protected:
  void Reset() { highlights.clear(); }
  void Mark(int32_t line, int32_t start, int32_t length, olc::Pixel color) {
    for (int32_t i = start; i < start + length; i++) highlights[line][i] = color;
  }

protected:
  std::vector<Interpreter::Token> tokens;
  std::unordered_map<int32_t, std::unordered_map<int32_t, olc::Pixel>> highlights;
};

};