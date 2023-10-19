#pragma once
#include "Command.h"

namespace Window {

// TODO: Change this to Pop Up
class Terminal : public Window {
public:
  inline void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  }

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    for (int32_t i = 0, offset = 0; i + offset <= size.y / vStep.y && i < lines.size(); i++, offset--) {
      if (!lines[i].size()) { offset++; continue; }

      for (int32_t j = 0; j < lines[i].size(); j += size.x / vStep.x, offset++) {
        if (i + offset > size.y / vStep.y) break;

        auto pos = olc::vi2d(absolute.x, absolute.y + (i + offset) * vStep.y) + vOffset;
        GameEngine->DrawString(pos, lines[i].substr(j, size.x / vStep.x), *AnyType<GREY, ColorT>::GetValue());
      }
    }
  }

public:
  void PushOut(std::string line) { lines.push_back(line); }
  void PushOut(std::vector<std::string>& l) { lines.clear(); lines.insert(lines.end(), l.begin(), l.end()); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  std::vector<std::string> lines;

  const olc::vi2d vOffset = olc::vi2d(8, 4);
  const olc::vi2d vStep = olc::vi2d(8, 12);
};
};