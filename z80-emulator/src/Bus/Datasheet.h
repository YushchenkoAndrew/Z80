#pragma once

#include "src/Defs.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace Bus {

class Datasheet {
public:
  enum { NONE, INPUT, OUTPUT, INPUT_OUTPUT };
  enum { LEFT, RIGHT };
  struct PinT { std::string name; uint8_t state = NONE; uint8_t orientation; bool inverted = false; };

public:
  Datasheet() = default;
  Datasheet(std::string device): name(std::move(device)) {}
 
  Datasheet& L(const std::string& name) { return Add(LEFT, name); }
  Datasheet& R(const std::string& name) { return Add(RIGHT, name); }

public:
  virtual void Symbol(PixelGameEngine* GameEngine, olc::vi2d absolute, olc::Pixel p = Color::GREY) {
    const olc::vi2d vStep(8, 12); constexpr int32_t stub = 40; constexpr int32_t pad  = 5;

    int32_t index[2] = {0, 0 };

    const auto size = (GetSize() + olc::vi2d(1, 1)) * vStep;
 
    GameEngine->DrawRect(absolute, size, p);
    GameEngine->DrawString(absolute - olc::vi2d(0, vStep.y), name, p);

    for (const auto& pin : pins) {
      const int32_t row = index[pin.orientation]++;
      if (pin.name == "_") continue;

      const int32_t top = (size.y - height[pin.orientation] * vStep.y) / 2;   // each side is centred in the box: a gate's lone output is in the middle, not at the top

      const int32_t dir  = pin.orientation ? 1 : -1;
      const int32_t edge = pin.orientation ? absolute.x + size.x + 1 : absolute.x - 1;
 
      const int32_t y    = absolute.y + top + vStep.y * row + 2;
      const int32_t w = (int32_t)pin.name.size() * vStep.x;
      const int32_t tx = pin.orientation ? absolute.x + size.x - pad - w : absolute.x + pad;

      GameEngine->DrawString(olc::vi2d(tx, y), pin.name, p);
      if (pin.inverted) GameEngine->DrawLine(olc::vi2d(tx, y - 2), olc::vi2d(tx + w - 2, y - 2), p);
 
      const olc::vi2d inner(edge, y + 3), outer(edge + dir * stub, y + 3);
      const olc::vi2d start = pin.orientation ? inner : outer;
      const olc::vi2d end   = pin.orientation ? outer : inner;
 
      if (!pin.state) GameEngine->DrawLine(inner, outer, p);                   // power pin etc.
      if (pin.state & Datasheet::INPUT)  GameEngine->DrawArrow(end, start, p);
      if (pin.state & Datasheet::OUTPUT) GameEngine->DrawArrow(start, end, p);
    }
  }

 
private:
  Datasheet& Add(uint8_t index, std::string name) {
    if (name.empty()) return *this;

    uint8_t state = NONE;
    if (name.front() == '<') state = state | INPUT,  name.erase(0, 1);
    if (name.back()  == '>') state = state | OUTPUT, name.pop_back();

    height[index]++; width[index] = std::max(width[index], (int32_t)name.size());

    const bool inverted = name.front() == '/'; if (inverted) name.erase(0, 1);
    pins.push_back({ name, state, index, inverted });

    return *this;
  }

public:
  const olc::vi2d GetSize() const {
    const auto W = width[LEFT] + width[RIGHT];
    const auto H = std::max(height[LEFT], height[RIGHT]);
    return olc::vi2d(std::max(W, std::min(H, 10)), H);
  }

public:
  std::string name;
  std::vector<PinT> pins;

private:
  int32_t width[2]  = { 0, 0 };
  int32_t height[2] = { 0, 0 };
};

};