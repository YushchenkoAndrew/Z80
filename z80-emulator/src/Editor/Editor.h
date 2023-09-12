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
    auto nWheel = GameEngine->GetMouseWheel();
    if (nWheel > 0) vim.MoveTo({ 0, -1 });
    else if (nWheel < 0) vim.MoveTo({ 0, 1 });

    auto vMax = size / vStep;
    auto cursor = vim.GetPos();

    auto mouse = GameEngine->GetMousePos();
    if (GameEngine->GetMouse(0).bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x && mouse.y < absolute.y + size.y) {
      vim.MoveTo((mouse - absolute) / vStep + vStartAt - vim.GetPos() - olc::vi2d(1 , 1));
    }

    if (cursor.y - vStartAt.y + 3 > vMax.y && cursor.y < vim.GetLinesSize()) vStartAt.y++;
    if ((cursor.y - vStartAt.y - 1 < 0 && cursor.y) || (!cursor.y && cursor.y != vStartAt.y)) vStartAt.y--;
      
    vim.Process(GameEngine);

    if (!vim.bUpdated) return;
    lexer.scan(vim.Text()); vim.Load(lexer.dst);
  }

  void Draw(olc::PixelGameEngine* GameEngine) {
    vim.Draw(GameEngine, [&](auto pos) { return absolute + (pos - vStartAt) * vStep + vOffset; });

    for (auto& token : lexer.dst) {
      if (vStartAt.y >= token->line) continue;

      olc::vi2d pos = absolute + (olc::vi2d(token->col, token->line) - vStartAt) * vStep + vOffset;

      // TODO: With X
      if (pos.y > size.y) break;
      GameEngine->DrawString(pos, token->lexeme, token->color);
    }
  }


private:
  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  olc::vi2d vStartAt = olc::vi2d(0, 0);

  Vim vim;

  Interpreter::Lexer lexer;
  Interpreter::Interpreter interpreter;

public:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);
};

};
