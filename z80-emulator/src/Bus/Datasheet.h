#pragma once

#include "src/Defs.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector>

namespace Bus {

class Datasheet {
public:
  enum Side : uint8_t { LEFT, RIGHT };
  enum State : uint8_t { NONE, INPUT, OUTPUT, INPUT_OUTPUT };

  struct Pin { std::string name; uint8_t pin; int32_t row; Side side; State state = NONE; bool inverted = false; };
  struct Anchor { olc::vi2d inner; olc::vi2d outer; }; // inner touches the body, outer is the free end

public:
  Datasheet() = default;
  Datasheet(std::string device): name(std::move(device)) {}
 
  Datasheet& LABEL(std::string name) { return label = std::move(name),  *this; }
  Datasheet& W(const int32_t width)  { return size.x = width,  *this; }
  Datasheet& H(const int32_t height) { return size.y = height, *this; }
  Datasheet& L(const std::string& name, uint8_t pin = 0) { return Add(LEFT, name, pin); }
  Datasheet& R(const std::string& name, uint8_t pin = 0) { return Add(RIGHT, name, pin); }

public:
  Anchor AnchorOf(const Pin& pin, olc::vi2d absolute = { 0, 0 }) const {
    const olc::vi2d size = GetSize();
    const int32_t top = (size.y - height[pin.side] * vStep.y) / 2;

    const int32_t y = absolute.y + top + vStep.y * pin.row + 2 + 3;
    const int32_t x = pin.side ? absolute.x + size.x + 1 :  absolute.x - 1;
    return { { x, y }, { x + (pin.side ? 48 : -48), y } };
  }
 
  const Pin* Find(const std::string& pin) const {
    for (const auto& p : pins) if (p.name == pin) return &p;
    return nullptr;
  }

private:
  Datasheet& Add(Side side, std::string name, uint8_t pin) {
    if (name.empty()) return *this;

    uint8_t state = NONE;
    if (name.front() == '<') state = state | INPUT,  name.erase(0, 1);
    if (name.back()  == '>') state = state | OUTPUT, name.pop_back();

    width[side] = std::max(width[side], (int32_t)name.size() + 1);

    const bool inverted = name.front() == '/'; if (inverted) name.erase(0, 1);
    pins.push_back({ name, pin, height[side]++, side, (State)state, inverted });

    offset[LEFT] = (GetSize().y - height[LEFT] * vStep.y) / 2;
    offset[RIGHT] = (GetSize().y - height[RIGHT] * vStep.y) / 2;

    return *this;
  }

public:
  const olc::vi2d GetSize() const {
    const auto W = width[LEFT] + width[RIGHT];
    const auto H = std::max(height[LEFT], height[RIGHT]);
    return olc::vi2d(size.x ? size.x : W + 1, size.y ? size.y : H + 1) * vStep;
  }

public:
  std::string name;
  std::string label;
  std::vector<Pin> pins;

private:
  const olc::vi2d vStep = olc::vi2d(8, 14);

  olc::vi2d size = olc::vi2d(0, 0);

  int32_t width[2]  = { 0, 0 };
  int32_t height[2] = { 0, 0 };
  int32_t offset[2] = { 0, 0 };
};

};