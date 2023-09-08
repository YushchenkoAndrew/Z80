#pragma once
#include "Vim.h"

namespace Editor {
class Editor : public Vim {
public:

  // TODO: Change this to more appropriate thing
  void temp(std::string src) { 
    lexer.scan(src); cursor.lines.clear();

    int32_t len = 0;

    for (auto& token : lexer.dst) {
      if (token->line == cursor.lines.size() + 1) { len += token->lexeme.size(); continue; }
      cursor.lines.push_back(len); len = token->lexeme.size();

      while (token->line != cursor.lines.size() + 1) cursor.lines.push_back(1);
    }
  }

  void Draw(olc::PixelGameEngine* GameEngine, olc::vi2d size, olc::vi2d absolute = olc::vi2d(0, 0)) {
    for (auto& token : lexer.dst) {
      olc::vi2d pos = absolute + olc::vi2d(token->col, token->line) * vStep + vOffset;

      // TODO: With X
      if (pos.y > size.y) break;
      GameEngine->DrawString(pos, token->lexeme, token->color);
    }
 
    cursor.Draw(GameEngine, [&](auto pos) { return absolute + pos * vStep + vOffset; });
  }

private:

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  Interpreter::Lexer lexer;
  Interpreter::Interpreter interpreter;
};

};
