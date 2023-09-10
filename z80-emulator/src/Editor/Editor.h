#pragma once
#include "Vim.h"

namespace Editor {
class Editor {
public:

  // TODO: Change this to more appropriate thing
  void temp(std::string src) { 
    lexer.scan(src); vim.Load(lexer.dst);
  }

  void Process(olc::PixelGameEngine* GameEngine) {
    vim.Process(GameEngine);

    if (!vim.bUpdated) return;
    lexer.scan(vim.Text()); vim.Load(lexer.dst);
  }

  void Draw(olc::PixelGameEngine* GameEngine, olc::vi2d size, olc::vi2d absolute = olc::vi2d(0, 0)) {
    for (auto& token : lexer.dst) {
      olc::vi2d pos = absolute + olc::vi2d(token->col, token->line) * vStep + vOffset;

      // TODO: With X
      if (pos.y > size.y) break;
      GameEngine->DrawString(pos, token->lexeme, token->color);
    }
 
    vim.Draw(GameEngine, [&](auto pos) { return absolute + pos * vStep + vOffset; });
  }

private:
  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  Vim vim;

  Interpreter::Lexer lexer;
  Interpreter::Interpreter interpreter;
};

};
