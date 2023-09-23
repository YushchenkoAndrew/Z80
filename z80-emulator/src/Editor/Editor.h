#pragma once
#include "Vim.h"

namespace Editor {
class Editor : public Window::Window {
public:
  Editor(): lexer(true) {}

  // TODO: Change this to more appropriate thing
  void temp(std::string src) { 
    lexer.scan(src); vim.Load(lexer.tokens);
  }

  inline void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  }

  void Process(PixelGameEngine* GameEngine) {
    auto nWheel = GameEngine->GetMouseWheel();
    if (nWheel > 0) vim.MoveTo({ 0, -1 });
    else if (nWheel < 0) vim.MoveTo({ 0, 1 });

    auto vMax = size / vStep;
    auto cursor = vim.GetPos();

    auto mouse = GameEngine->GetMousePos() - vOffset;
    if (GameEngine->GetMouse(0).bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x && mouse.y < absolute.y + size.y) {
      vim.MoveTo((mouse - absolute) / vStep + vStartAt - vim.GetPos() - olc::vi2d(1 , 1));
    }

    if (cursor.y - vStartAt.y + 3 > vMax.y && cursor.y < vim.GetLineSize()) vStartAt.y++;
    if ((cursor.y - vStartAt.y - 1 < 0 && cursor.y) || (!cursor.y && cursor.y != vStartAt.y)) vStartAt.y--;

    if (cursor.x - vStartAt.x + 3 > vMax.x && cursor.x < vim.GetLineSize(cursor.y)) vStartAt.x++;
    if ((cursor.x - vStartAt.x - 1 < 0 && cursor.x) || (!cursor.x && cursor.x != vStartAt.x)) vStartAt.x--;
      
    vim.Process(GameEngine);

    if (!vim.bUpdated) return;
    lexer.scan(vim.Text()); vim.Load(lexer.tokens);
    for (auto& err : lexer.errors) printf("LEXER: %s", err.c_str());
  }

  void Draw(PixelGameEngine* GameEngine) {
    olc::vi2d pos = olc::vi2d(absolute.x + vOffset.x, absolute.y + (vim.GetPos().y + 1 - vStartAt.y) * vStep.y + vOffset.y);
    GameEngine->FillRect(pos, { size.x, 8 }, ~AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());

    vim.Draw(GameEngine, [&](auto pos) { return absolute + (pos - vStartAt) * vStep + vOffset; });

    for (auto& token : lexer.tokens) {
      if (vStartAt.y >= token->line) continue;

      // FIXME: Fix bug wih displaying large lines in a small window
      olc::vi2d pos = absolute + (olc::vi2d(token->col, token->line) - vStartAt) * vStep + vOffset;

      if (pos.x > size.x) continue;
      if (pos.y >= size.y - vStep.y) break;

      if (pos.x < size.x && pos.x + token->lexeme.size() * vStep.x > size.x) {
        GameEngine->DrawString(pos, token->lexeme.substr(0, (size.x - pos.x) / vStep.x), token->color);
      } else GameEngine->DrawString(pos, token->lexeme, token->color);
    }

    auto cursor = vim.GetPos();
    pos = (absolute + (cursor - vStartAt) * vStep + vOffset) / vStep; 

    std::stringstream ss;
    for (int32_t i = 0, max = size.y / vStep.y; i < max; i++) {
      ss.str(""); ss << cursor.y - pos.y + i + 1; auto str = ss.str();
      auto line = olc::vi2d(absolute.x + vOffset.x - str.size() * vStep.x, (i + 1) * vStep.y);

      if (cursor.y - vStartAt.y == i) {
        GameEngine->DrawString(line, str, ~AnyType<GREY, ColorT>::GetValue());
      } else GameEngine->DrawString(line, str, ~AnyType<DARK_GREY, ColorT>::GetValue());
    }

    pos = olc::vi2d(absolute.x, absolute.y + size.y - vStep.y);
    GameEngine->FillRect(pos - olc::vi2d(0, 2), { size.x + vOffset.x, 10 }, ~AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + olc::vi2d(vStep.x, 0), vim.GetMode(), ~AnyType<Colors::DARK_GREY, ColorT>::GetValue());

    auto humanizedPos = vim.GetHumanizedPos();

    pos.x = size.x + vOffset.x - ((int32_t)humanizedPos.size() + 1) * vStep.x;
    GameEngine->DrawString(pos, humanizedPos, ~AnyType<Colors::DARK_GREY, ColorT>::GetValue());

    pos.x -= 5 * vStep.x;
    GameEngine->DrawString(pos, vim.GetCmd(), ~AnyType<Colors::DARK_GREY, ColorT>::GetValue());
  }

public:
  inline void MoveTo(olc::vi2d next) { vim.MoveTo(next - vim.GetPos()); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(24, 0);

  olc::vi2d vStartAt = olc::vi2d(0, 0);

  Vim vim;

  Interpreter::Lexer lexer;
  Interpreter::Interpreter interpreter;
};

};
