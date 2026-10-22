#pragma once

#include "src/Bus/Bus.h"
#include "src/Bus/Device.h"
#include "src/Window/Window.h"
namespace Bus {

class Switch : public Window::Window, public Device {
public:
  Switch(PixelGameEngine* engine, std::string id): Window::Base(engine), Window::Window(engine, std::move(id)) {}
  void Select(DimensionT dimensions) override { this->absolute = dimensions.first; this->size = dimensions.second; }

  void Process() override {
    auto mouse = GameEngine->GetMousePos() - vOffset;
    if (GameEngine->GetMouse(0).bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + vStep.x * sizeof(uint8_t) * 8 && mouse.y < absolute.y + vStep.y) {
      data = data ^ (0x01 << ((mouse.x - absolute.x) / vStep.x));
    }
  }

  void Draw() override {
    for (int32_t i = sizeof(uint8_t) * 7, data = this->data; i >= 0; i--, data = data << 1) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * vStep + vOffset;

      if (data & 0x80) GameEngine->FillRect(pos + olc::vi2d(1, 1), olc::vi2d(8, 8), Color::WHITE);
      GameEngine->DrawRect(pos, olc::vi2d(8, 8), Color::WHITE);
    }
  }

  uint8_t Read(uint32_t addr, bool) { return data; }
  uint8_t Write(uint32_t addr, uint8_t data, bool) { return 0x00; }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(12, 12);
  const olc::vi2d vOffset = olc::vi2d(0, 0);

  uint8_t data = 0x00;
};

};
