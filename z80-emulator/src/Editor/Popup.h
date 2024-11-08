#pragma once
#include "Editor.h"

namespace Editor {

class Popup : public Window::Window {
public:

  // inline void Preinitialize() { Lock(); }
  inline void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  }

  void Load(std::vector<std::string>& dst) {
    vim.Load(lines = dst); vim.Command(Int2Type<VimT::CMD_gg>());
  }

  void Lock() { vim.Lock(); }
  void Unlock() { vim.Unlock(); }

  void Preprocess() {
    Utils::Lock l(mutex);

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

    auto nWheel = GameEngine->GetMouseWheel();
    if (nWheel > 0) vim.MoveTo({ 0, -1 });
    else if (nWheel < 0) vim.MoveTo({ 0, 1 });

    vim.Process(GameEngine);

    if (!vim.bUpdated) return;
    lines = vim.GetLines();
  }

  void Draw(PixelGameEngine* GameEngine) {
    Utils::Lock l(mutex);
    GameEngine->FillRect(absolute, size, *AnyType<Colors::BLACK, ColorT>::GetValue());

    const auto cursor = vim.GetPos();
    const auto vStartAt = vim.vStartAt();

    olc::vi2d pos = olc::vi2d(absolute.x + vOffset.x, absolute.y + (vim.GetPos().y + 1 - vStartAt.y) * vStep.y + vOffset.y);
    GameEngine->FillRect(pos, { size.x, 8 }, *AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());

    vim.Draw(GameEngine, [&](auto pos) { return absolute + (pos - vStartAt) * vStep + vOffset; });

    auto nHeight = size.y - vStep.y;

    for (int32_t i = vStartAt.y; i < lines.size(); i++) {
      olc::vi2d pos = absolute + (olc::vi2d(1, i + 1) - vStartAt) * vStep + vOffset;

      if (pos.x > absolute.x + vOffset.x + size.x + vStep.x) continue;
      if (pos.y > absolute.y + vOffset.y + nHeight) break;

      auto nWidth = absolute.x + vOffset.x + vStep.x;
      if (pos.x < nWidth && pos.x + lines[i].size() * vStep.x <= nWidth) continue;

      auto line = std::string();
      auto startAt = pos.x < nWidth ? (nWidth - pos.x) / vStep.x : 0;
      auto adj = pos + olc::vi2d(startAt * vStep.x, 0);
      nWidth = absolute.x + vOffset.x + size.x + vStep.x;

      if (pos.x < nWidth && pos.x + lines[i].size() * vStep.x > nWidth) {
        if (startAt > lines[i].size()) continue;
        line = lines[i].substr(startAt, (nWidth - pos.x) / vStep.x - startAt - 1);
      } else line = lines[i].substr(startAt, lines[i].size() - startAt);

      GameEngine->DrawString(adj, line, *AnyType<GREY, ColorT>::GetValue());
    }
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  std::mutex mutex;

  Vim vim;
  std::vector<std::string> lines;

public:
  int32_t window = 0;
};
};

