#pragma once

#include "src/Bus/Device.h"
#include "src/Utils/Utils.h"
#include "src/Window/Event/KeyboardEvent.h"
#include "src/Window/Window.h"
#include <cstdint>

namespace Bus {
class Keyboard : public Window::KeyboardEvent, public Window::Window, public Device {
public:
  Keyboard(PixelGameEngine* engine, std::string id): ::Window::Base(engine), ::Window::Window(engine, std::move(id)), ::Window::KeyboardEvent(engine) {}

  void Process() override {
    const auto mouse = GameEngine->GetMousePos();
    const bool bPressed = GameEngine->GetMouse(0).bPressed;

    olc::vi2d pos = absolute - olc::vi2d(1, 1);
    olc::vi2d size = name.second + olc::vi2d(2, 2);

    if (bPressed && mouse.x > pos.x && mouse.y > pos.y && mouse.x < pos.x + size.x && mouse.y < pos.y + size.y) {
      bEnabled = bEnabled ^ true;
    }

    if (!bEnabled) return;
    bool has; { std::lock_guard<std::mutex> l(mutex); has = !buffer.empty(); } if (has) Interrupt();
  }

  void Draw() override {
    std::vector<uint8_t> buff; { std::lock_guard<std::mutex> l(mutex); for (auto v : buffer) { buff.push_back(v); if (buff.size() >= 4) break; } }
    olc::vi2d pos = absolute;

    const auto& fg = bEnabled ? Color::DARK_GREY : Color::BLACK;
    const auto& bg = bEnabled ? Color::BLACK     : Color::DARK_GREY;

    GameEngine->FillRect(pos - olc::vi2d(1, 1), name.second + olc::vi2d(2, 2), bg);
    GameEngine->DrawString(pos, name.first, fg);

    int32_t index = 0;
    const olc::vi2d vOffset = olc::vi2d(name.second.x, 0) + vStep;

    for (auto& val : buff) {
      GameEngine->DrawString(pos + vOffset, Utils::Int2Hex(val), Color::GREY);

      if (++index > 3) break;
      else pos.x += vStep.x * 3;
    }
  }

  void Interrupt();

public:
   
  uint8_t Read(uint32_t addr, bool) override {
    std::lock_guard<std::mutex> l(mutex);
    if (buffer.empty()) return 0;

    auto c = buffer.front();
    return buffer.pop_front(), c; 
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) override { return 0x00; }

public:
  const ::Window::StrokeT<>& GetCommands() const override { return Keyboard::commands; }
  ::Window::StrokeAction Process(int8_t keymap) {
    std::lock_guard<std::mutex> l(mutex); 
    buffer.push_back(keymap); buffer.push_back(0xF0); buffer.push_back(keymap); // TODO: Think about how to impl this better
    return nullptr;
  }

private:
  const olc::vi2d vStep = olc::vi2d(8, 0);

  std::atomic<bool> bEnabled = false;
  std::pair<const char*, olc::vi2d> name = std::pair("buf", olc::vi2d(8 * 3, 8));

  std::mutex mutex;
  std::list<uint8_t> buffer;
  static const ::Window::StrokeT<> commands;
};

inline const Window::StrokeT<> Keyboard::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::Q>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x15); } },
  { Int2Type<olc::Key::W>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x1D); } },
  { Int2Type<olc::Key::E>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x24); } },
  { Int2Type<olc::Key::R>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x2D); } },
  { Int2Type<olc::Key::T>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x2C); } },
  { Int2Type<olc::Key::Y>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x35); } },
  { Int2Type<olc::Key::U>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x3C); } },
  { Int2Type<olc::Key::I>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x43); } },
  { Int2Type<olc::Key::O>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x44); } },
  { Int2Type<olc::Key::P>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x4D); } },
  { Int2Type<olc::Key::A>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x1C); } },
  { Int2Type<olc::Key::S>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x1B); } },
  { Int2Type<olc::Key::D>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x23); } },
  { Int2Type<olc::Key::F>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x2B); } },
  { Int2Type<olc::Key::G>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x34); } },
  { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x33); } },
  { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x3B); } },
  { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x42); } },
  { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x4B); } },
  { Int2Type<olc::Key::Z>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x1A); } },
  { Int2Type<olc::Key::X>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x22); } },
  { Int2Type<olc::Key::C>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x21); } },
  { Int2Type<olc::Key::V>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x2A); } },
  { Int2Type<olc::Key::B>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x32); } },
  { Int2Type<olc::Key::N>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x31); } },
  { Int2Type<olc::Key::M>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x3A); } },

  { Int2Type<olc::Key::K1>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x16); } },
  { Int2Type<olc::Key::K2>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x1E); } },
  { Int2Type<olc::Key::K3>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x26); } },
  { Int2Type<olc::Key::K4>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x25); } },
  { Int2Type<olc::Key::K5>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x2E); } },
  { Int2Type<olc::Key::K6>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x36); } },
  { Int2Type<olc::Key::K7>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x3D); } },
  { Int2Type<olc::Key::K8>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x3E); } },
  { Int2Type<olc::Key::K9>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x46); } },
  { Int2Type<olc::Key::K0>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x45); } },

  { Int2Type<olc::Key::NP0>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('0'); } },
  { Int2Type<olc::Key::NP1>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('1'); } },
  { Int2Type<olc::Key::NP2>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('2'); } },
  { Int2Type<olc::Key::NP3>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('3'); } },
  { Int2Type<olc::Key::NP4>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('4'); } },
  { Int2Type<olc::Key::NP5>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('5'); } },
  { Int2Type<olc::Key::NP6>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('6'); } },
  { Int2Type<olc::Key::NP7>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('7'); } },
  { Int2Type<olc::Key::NP8>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('8'); } },
  { Int2Type<olc::Key::NP9>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('9'); } },

  { Int2Type<olc::Key::TAB>(),   [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x0D); } },
  { Int2Type<olc::Key::BACK>(),  [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x66); } },
  { Int2Type<olc::Key::DEL>(),   [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process('\0'); } },
  { Int2Type<olc::Key::SPACE>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x29); } },
  { Int2Type<olc::Key::ENTER>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x5A); } },
  { Int2Type<olc::Key::ESCAPE>(),[](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x76); } },

  { Int2Type<olc::Key::OEM_1>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x4C); } },
  { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x4A); } },
  { Int2Type<olc::Key::OEM_3>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x0E); } },
  { Int2Type<olc::Key::OEM_4>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x54); } },
  { Int2Type<olc::Key::OEM_5>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x5D); } },
  { Int2Type<olc::Key::OEM_6>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x5B); } },
  { Int2Type<olc::Key::OEM_7>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x52); } },

  { Int2Type<olc::Key::CAPS_LOCK>(),[](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x58); } },
  { Int2Type<olc::Key::SHIFT>(),    [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x12); } },
  { Int2Type<olc::Key::CTRL>(),     [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x14); } },

  { Int2Type<olc::Key::F1>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x05); } },
  { Int2Type<olc::Key::F2>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x06); } },
  { Int2Type<olc::Key::F3>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x04); } },
  { Int2Type<olc::Key::F4>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x0C); } },
  { Int2Type<olc::Key::F5>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x03); } },
  { Int2Type<olc::Key::F6>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x0B); } },
  { Int2Type<olc::Key::F7>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x83); } },
  { Int2Type<olc::Key::F8>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x0A); } },
  { Int2Type<olc::Key::F9>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x01); } },
  { Int2Type<olc::Key::F10>(),[](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x09); } },
  { Int2Type<olc::Key::F11>(),[](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x78); } },
  { Int2Type<olc::Key::F12>(),[](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x07); } },

  { Int2Type<olc::Key::EQUALS>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x55); } },
  { Int2Type<olc::Key::PERIOD>(), [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x49); } },
  { Int2Type<olc::Key::COMMA>(),  [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x41); } },
  { Int2Type<olc::Key::MINUS>(),  [](void* self, auto... keys) { return static_cast<Keyboard*>(self)->Process(0x4E); } },
});
};
