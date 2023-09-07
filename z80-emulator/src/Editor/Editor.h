#pragma once
#include "src/Interpreter/Interpreter.h"

namespace Editor {

class Editor {
public:

  // TODO: Change this to more appropriate thing
  void temp(std::string src) { lexer.scan(src); }

  void Draw(olc::PixelGameEngine* GameEngine, olc::vi2d size, olc::vi2d absolute = olc::vi2d(0, 0)) {
    olc::vi2d step = { 8, 12 };
    olc::vi2d offset = { 0, 0 };

    for (auto& token : lexer.dst) {
      olc::vi2d pos = absolute + olc::vi2d(token->col, token->line) * step + offset;

      // TODO: With X
      if (pos.y > size.y) break;
      GameEngine->DrawString(pos, token->lexeme, token->color);
    }

    // TODO: Add cursor
  
    olc::vi2d pos = absolute + vCurr * step + offset - olc::vi2d(1, 2);
    GameEngine->FillRect(pos, { 8, 10 }, AnyType<WHITE, olc::Pixel>::GetValue());
  }

private:
  olc::vi2d vCurr = { 1, 1 };
  Interpreter::Lexer lexer;

  Interpreter::Interpreter interpreter;
};

};
