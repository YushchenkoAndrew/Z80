#pragma once

#include "lib/olcPGEX_Sound.h"
#include "src/Bus/Device.h"
#include "src/Utils/Utils.h"
#include "src/Window/Window.h"

namespace Bus {

#define CT0(p)  std::get<0>(p)
#define CT1(p)  std::get<1>(p)
#define CT2(p)  std::get<2>(p)

#define ENABLE_OUTPUT  0x01
#define KEEP_OUTPUT    0x02

/**
 * @brief Programmable Interval Timer
 */
class PIT : public Window::Window, public Device {

typedef std::tuple<uint16_t, uint16_t, uint16_t> RegT;

public:
  PIT(PixelGameEngine* engine, std::string id, int32_t c): Window::Base(engine), Window::Window(engine, std::move(id)), clock(std::pair(c, 1.0E9F / c)), runtime(std::thread(std::thread(&PIT::Runtime, this))) {
    // olc::SOUND::InitialiseAudio();
    // olc::SOUND::SetUserSynthFunction([&](int nChannel, float fGlobalTime, float) { return Sound(fGlobalTime); });
  }

  ~PIT() {
    bExec = false;

    olc::SOUND::DestroyAudio();
    if (runtime.joinable()) runtime.join();
  }


  void Process() override {}
  void Draw() override {
    olc::vi2d pos = absolute; olc::vi2d vNextLine = olc::vi2d(0, vStep.y);
    GameEngine->DrawString(pos, "PIT", Color::DARK_GREY);

    auto freq = "[" + Utils::Int2Scientific(clock.second) + " Hz]";
    olc::vi2d vOffset = olc::vi2d(vStep.x * 4, 0);

    GameEngine->FillRect(pos + vOffset - olc::vi2d(1, 1), GameEngine->GetTextSize(freq) + olc::vi2d(1, 1), Color::DARK_GREY);
    GameEngine->DrawString(pos + vOffset, freq, Color::BLACK);
    pos.y += vStep.y;

    GameEngine->DrawString(pos + vOffset, "VAL", Color::DARK_GREY);
    GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x, 0), "CL", Color::DARK_GREY);
    GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 4, 0), "E", Color::DARK_GREY);
    GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 6, 0), "Y", Color::DARK_GREY);
    pos.y += vStep.y;

    for (uint8_t i = 0; i < 3; i++) {
      // Utils::Lock l(mutex);

      uint8_t cs = GetValue(gate, i);
      uint8_t out = GetValue(output, i);
      uint8_t ctl = GetValue(control, i);
      uint16_t value = GetValue(counter, i);

      GameEngine->DrawString(pos, "CT" + std::to_string(i), Color::DARK_GREY);
      GameEngine->DrawString(pos + vOffset, Utils::Int2Hex(value, 4), Color::GREY);
      GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x, 0), Utils::Int2Hex(ctl), Color::GREY);
      GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 4, 0), cs ? "1" : "0", Color::GREY);
      GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 6, 0), !cs || out ? "1" : "0", Color::GREY);

      pos.y += vStep.y;
    }
  }

  void Interrupt(int32_t);

  inline float Sound(float fGlobalTime) {
    if (!GetValue(gate, 0)) return 0.0f;
    float frequency = (float)clock.second / GetValue(counter, 0);

    float output = sinf(2 * 3.14f * frequency * fGlobalTime);
    return output > 0.0f ? 0.5 : -0.5;
  }

  void Runtime() {
  auto step = [&](const int32_t i) {
      // Utils::Lock l(mutex);
      uint16_t& value = GetValue(counter, i);
      uint16_t& out = GetValue(output, i);
      uint16_t enabled = GetValue(gate, i);

      if (!enabled) return;

      if (out & ENABLE_OUTPUT) Interrupt(i);
      if (!(out & KEEP_OUTPUT)) out = !ENABLE_OUTPUT;

      if (value) { value = value - 1; return; }

      uint16_t ctl = GetValue(control, i);
      uint16_t init = GetValue(initialize, i);

      switch ((ctl & 0x0E) >> 1) {
        case 0x00: break; // NOTE: There is not point to create MODE0 it will not be used at all 
        case 0x01: break; // NOTE: There is not point to create MODE1 it will not be used at all 

        case 0x02: case 0x05: value = init; out = ENABLE_OUTPUT; break;
        case 0x04: if (armed[i]) { out = ENABLE_OUTPUT; armed[i] = false; } break;
        case 0x03: value = init; out = ((out & ENABLE_OUTPUT) ^ ENABLE_OUTPUT) | KEEP_OUTPUT; break;
      }
    };

    while (bExec) {
      // std::this_thread::sleep_for(std::chrono::nanoseconds(clock.first));

      // NOTE: Because run of this loop takes around 60ms, note generations (CT0) was moved to the func Sound 
      step(1); step(2);
    }
  }

public:
  uint8_t Read(uint32_t addr, bool) override {
    const uint8_t i = addr & 3; if (i > 2) return 0x00;
    const uint16_t value = latched[i] ? GetValue(copy, i) : GetValue(counter, i);
    const uint8_t rw = (GetValue(control, i) >> 4) & 3;
    const bool high = rw == 2 || (rw == 3 && hi[i]);

    if (rw == 3) { if (hi[i]) latched[i] = false; hi[i] = !hi[i]; }   // LSB first, then MSB
    else latched[i] = false;
    return high ? value >> 8 : value & 0xFF;
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) override {
    const uint8_t i = addr & 3;
    if (i != 3) {
      if (i > 2) return data;
      uint16_t& value = GetValue(counter, i); uint16_t& init = GetValue(initialize, i);
      const uint8_t rw = (GetValue(control, i) >> 4) & 3;

      if (rw == 1) init = (init & 0xFF00) | data;
      else if (rw == 2) init = (init & 0x00FF) | (uint16_t)data << 8;
      else if (rw == 3) { init = hi[i] ? (init & 0x00FF) | (uint16_t)data << 8 : (init & 0xFF00) | data; hi[i] = !hi[i]; }

      if (rw != 3 || !hi[i]) { value = init; armed[i] = true; }       // counter loads once the count is complete
      return data;
    }

    const uint8_t c = (data & 0xC0) >> 6; if (c > 2) return data;
    if (((data >> 4) & 3) == 0) { GetValue(copy, c) = GetValue(counter, c); latched[c] = true; return data; }   // latch only
    GetValue(control, c) = data; hi[c] = latched[c] = false;
    return data;
  }

  void SetGate(uint8_t addr, uint16_t flag) {
    if (!addr) GetValue(gate, 0) = flag;
    else GetValue(gate, 2) = (GetValue(gate, 1) = flag);
  }

private:
  uint16_t& GetValue(RegT& reg, uint8_t addr) {
    switch (addr & 0xFF) {
      case 0x00: return CT0(reg);
      case 0x01: return CT1(reg);
      case 0x02: return CT2(reg);
    }

    static uint16_t none;
    return none;
  }

private:
  const olc::vi2d vStep = olc::vi2d(8, 12);

  std::pair<int32_t, int32_t> clock; // nanoseconds, Hz
  std::array<bool, 3> hi = {}, latched = {}, armed = {};   // new members

  std::atomic<bool> bExec = true;
  std::thread runtime;

  // std::mutex mutex;
  
  RegT copy;
  RegT counter = { 0, 0, 0 };
  RegT initialize;

  RegT control;

  RegT gate = { 0x00, 0x00, 0x00 };
  RegT output = { 0x01, 0x01, 0x01 };
};

#undef CT0
#undef CT1
#undef CT2

#undef ENABLE_OUTPUT
#undef KEEP_OUTPUT
};
