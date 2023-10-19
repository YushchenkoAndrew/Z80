#pragma once
#include "Vim.h"

namespace Editor {
#define PATH(e)     std::get<0>(e)
#define FILENAME(e) std::get<1>(e)
#define LEXER(e)    std::get<2>(e)
#define VIM(e)      std::get<3>(e)

class Editor : public Window::Window {
public:
  typedef std::tuple<std::string, std::string, Interpreter::Lexer, Vim> TabT;

  Editor() {}

  Editor* Open(std::string path) { 
    Utils::Lock l(mutex);

    for (int32_t i = 0; i < tabs.size(); i++) {
      if (PATH(tabs[i]) == path) { nTab = i; return this; }
    }

    std::ifstream f(path); std::stringstream buf;
    buf<< f.rdbuf(); f.close();

    auto filename = std::filesystem::path(path).filename().string();
    tabs.push_back(std::tuple(path, filename, Interpreter::Lexer(true), Vim()));

    auto& tab = tabs.back(); nTab = tabs.size() - 1;
    LEXER(tab).scan(buf.str()); VIM(tab).Load(LEXER(tab).tokens);
    return this;
  }

  void Save() { 
    Utils::Lock l(mutex);

    for (auto& tab : tabs) {
      std::ofstream f(PATH(tab)); f << VIM(tab).Text(); f.close();
    }
  }

  void Save(bool) { 
    Utils::Lock l(mutex);

    std::ofstream f(PATH(tabs[nTab])); f << VIM(tabs[nTab]).Text(); f.close();
  }

  inline void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  }

    
  void Lock() { Utils::Lock l(mutex); for (auto& tab : tabs) VIM(tab).Lock(); }
  void Unlock() { Utils::Lock l(mutex); for (auto& tab : tabs) VIM(tab).Unlock(); }

  void Preprocess() {
    Utils::Lock l(mutex);
    auto& vim = VIM(tabs[nTab]);

    const auto vMax = size / vStep;
    const auto cursor = vim.GetPos();
    auto& vStartAt = vim.vStartAt();

    if (cursor.y - vStartAt.y + 3 > vMax.y && cursor.y < vim.GetLineSize()) vStartAt.y++;
    if ((cursor.y - vStartAt.y - 1 < 0 && cursor.y) || (!cursor.y && cursor.y != vStartAt.y)) vStartAt.y--;

    if (cursor.x - vStartAt.x + 3 > vMax.x && cursor.x < vim.GetLineSize(cursor.y)) vStartAt.x++;
    if ((cursor.x - vStartAt.x - 1 < 0 && cursor.x) || (!cursor.x && cursor.x != vStartAt.x)) vStartAt.x--;
  }

  void Process(PixelGameEngine* GameEngine) {
    Utils::Lock l(mutex);

    const auto mouse = GameEngine->GetMousePos();
    const bool bPressed = GameEngine->GetMouse(0).bPressed;

    if (bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x + vOffset.x && mouse.y < absolute.y + vOffset.y + vStep.y) {
      auto pos = (mouse - absolute) / vStep;

      for (int32_t i = 0, acc = 0; i < tabs.size(); i++) {
        auto filename = GetTabName(tabs[i], 1);
        if (pos.x > acc && pos.x < acc + (int32_t)filename.size() + 1) { nTab = i; return; }
        acc += (int32_t)filename.size() + 1;
      }

      return;
    }

    auto& vim = VIM(tabs[nTab]); auto& lexer = LEXER(tabs[nTab]);

    auto nWheel = GameEngine->GetMouseWheel();
    if (nWheel > 0) vim.MoveTo({ 0, -1 });
    else if (nWheel < 0) vim.MoveTo({ 0, 1 });

    const auto vMax = size / vStep;
    const auto cursor = vim.GetPos();
    const auto vStartAt = vim.vStartAt();

    if (bPressed && mouse.x > absolute.x + vOffset.x && mouse.y > absolute.y + vOffset.y && mouse.x < absolute.x + size.x + vOffset.x && mouse.y < absolute.y + size.y + vOffset.y) {
      vim.MoveTo((mouse - absolute - vOffset) / vStep + vStartAt - vim.GetPos() - olc::vi2d(1 , 1));
    }

    vim.Process(GameEngine);

    if (!vim.bUpdated) return;
    lexer.scan(vim.Text()); vim.Load(lexer.tokens);
    // for (auto& err : lexer.errors) printf("LEXER: %s", err.c_str());
  }

  void Draw(PixelGameEngine* GameEngine) {
    Utils::Lock l(mutex);
    auto& vim = VIM(tabs[nTab]); auto& lexer = LEXER(tabs[nTab]);

    const auto cursor = vim.GetPos();
    const auto vStartAt = vim.vStartAt();

    olc::vi2d pos = olc::vi2d(absolute.x + vOffset.x, absolute.y + (vim.GetPos().y + 1 - vStartAt.y) * vStep.y + vOffset.y);
    GameEngine->FillRect(pos, { size.x, 8 }, *AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());

    vim.Draw(GameEngine, [&](auto pos) { return absolute + (pos - vStartAt) * vStep + vOffset; });
    auto nHeight = size.y - vStep.y;

    for (auto& token : lexer.tokens) {
      if (vStartAt.y >= token->line) continue;
      olc::vi2d pos = absolute + (olc::vi2d(token->col, token->line) - vStartAt) * vStep + vOffset;

      if (pos.x > absolute.x + vOffset.x + size.x + vStep.x) continue;
      if (pos.y > absolute.y + vOffset.y + nHeight) break;

      auto nWidth = absolute.x + vOffset.x + vStep.x;
      if (pos.x < nWidth && pos.x + token->lexeme.size() * vStep.x <= nWidth) continue;

      auto startAt = pos.x < nWidth ? (nWidth - pos.x) / vStep.x : 0;
      auto adj = pos + olc::vi2d(startAt * vStep.x, 0);
      nWidth = absolute.x + vOffset.x + size.x + vStep.x;

      if (pos.x < nWidth && pos.x + token->lexeme.size() * vStep.x > nWidth) {
        if (startAt > token->lexeme.size()) continue;
        GameEngine->DrawString(adj, token->lexeme.substr(startAt, (nWidth - pos.x) / vStep.x - startAt - 1), token->color);
      } else GameEngine->DrawString(adj, token->lexeme.substr(startAt, token->lexeme.size() - startAt), token->color);
    }


    pos = olc::vi2d(absolute.x, absolute.y + vOffset.y - 10);
    for (int32_t i = 0; i < tabs.size(); i++) {
      auto filename = GetTabName(tabs[i], 1);
      auto offset = olc::vi2d((filename.size() + 1) * vStep.x, 14);

      pos.x += 2;
      if (nTab == i) {
        GameEngine->FillRect(pos, offset, *AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());

        GameEngine->DrawLine(pos + olc::vi2d(0, offset.y), pos + offset - olc::vi2d(1, 0), *AnyType<Colors::GREY, ColorT>::GetValue());
        GameEngine->DrawString(pos + olc::vi2d(4, 4), filename, *AnyType<Colors::GREY, ColorT>::GetValue());
      } else {
        GameEngine->DrawLine(pos + olc::vi2d(0, offset.y), pos + offset - olc::vi2d(1, 0), *AnyType<Colors::DARK_GREY, ColorT>::GetValue());
        GameEngine->DrawString(pos + olc::vi2d(4, 4), filename, *AnyType<Colors::DARK_GREY, ColorT>::GetValue());
      }

      pos.x += offset.x;
    }


    for (int32_t i = 0, max = nHeight / vStep.y; i < max; i++) {
      auto str = std::to_string(vStartAt.y + i + 1);
      auto line = olc::vi2d(absolute.x + vOffset.x - str.size() * vStep.x, (i + 1) * vStep.y + vOffset.y);
      auto color =
        vim.IsSelected(vStartAt.y + i) ? AnyType<RED, ColorT>::GetValue() : 
        (cursor.y - vStartAt.y == i ? AnyType<GREY, ColorT>::GetValue() : AnyType<DARK_GREY, ColorT>::GetValue());

      GameEngine->DrawString(line, str, *color);
    }

    pos = olc::vi2d(absolute.x, absolute.y + nHeight + vOffset.y);
    GameEngine->FillRect(pos - olc::vi2d(0, 2), { size.x + vOffset.x, 10 }, *AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + olc::vi2d(vStep.x, 0), vim.GetMode(), *AnyType<Colors::DARK_GREY, ColorT>::GetValue());

    auto humanizedPos = vim.GetHumanizedPos();

    pos.x = size.x + vOffset.x - ((int32_t)humanizedPos.size() + 1) * vStep.x;
    GameEngine->DrawString(pos, humanizedPos, *AnyType<Colors::DARK_GREY, ColorT>::GetValue());

    pos.x -= 5 * vStep.x;
    GameEngine->DrawString(pos, vim.GetCmd(), *AnyType<Colors::DARK_GREY, ColorT>::GetValue());
  }

public:
  inline void MoveTo(olc::vi2d next) { Utils::Lock l(mutex); VIM(tabs[nTab]).MoveTo(next - VIM(tabs[nTab]).GetPos()); }
  inline std::string GetTabName(TabT& tab, int32_t offset = 0) {
    auto& filename = FILENAME(tab);
    const int32_t nWidth = (vOffset.x + size.x) / (((int32_t)tabs.size() + offset) * vStep.x);

    if (nWidth >= filename.size()) return filename;
    return filename.substr(0, nWidth);
  }

  inline std::string File() { Utils::Lock l(mutex); return PATH(tabs[nTab]); }
  inline void SelectLine(int32_t line) { Utils::Lock l(mutex); VIM(tabs[nTab]).SelectLine(line); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(24, 16);

  std::mutex mutex;

  int32_t nTab = 0;
  std::vector<TabT> tabs;
};

};
