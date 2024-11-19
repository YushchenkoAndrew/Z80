#pragma once
#include "Keyboard.h"

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
  PIT(Bus* b, int32_t c): bus(b), clock(std::pair(c, 1.0E9F / c)) {
    olc::SOUND::InitialiseAudio();
    olc::SOUND::SetUserSynthFunction([&](int nChannel, float fGlobalTime, float) { return Sound(fGlobalTime); });
  }

  ~PIT() {
    bExec = false;

    olc::SOUND::DestroyAudio();
    if (runtime != nullptr && runtime->joinable()) runtime->join();
  }

  void Preinitialize() {
    if (runtime == nullptr) runtime = std::make_unique<std::thread>(std::thread(&PIT::Runtime, this));
  }

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {}

  void Draw(PixelGameEngine* GameEngine) {
    olc::vi2d pos = absolute; olc::vi2d vNextLine = olc::vi2d(0, vStep.y);
    GameEngine->DrawString(pos, "PIT", *AnyType<DARK_GREY, ColorT>::GetValue());

    auto freq = "[" + Utils::Int2Scientific(clock.second) + " Hz]";
    olc::vi2d vOffset = olc::vi2d(vStep.x * 4, 0);

    GameEngine->FillRect(pos + vOffset - olc::vi2d(1, 1), GameEngine->GetTextSize(freq) + olc::vi2d(1, 1), *AnyType<DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + vOffset, freq, *AnyType<BLACK, ColorT>::GetValue());
    pos.y += vStep.y;

    GameEngine->DrawString(pos + vOffset, "VAL", *AnyType<DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x, 0), "CL", *AnyType<DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 4, 0), "E", *AnyType<DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 6, 0), "Y", *AnyType<DARK_GREY, ColorT>::GetValue());
    pos.y += vStep.y;

    for (uint8_t i = 0; i < 3; i++) {
      Utils::Lock l(mutex);

      uint8_t cs = GetValue(gate, i);
      uint8_t out = GetValue(output, i);
      uint8_t ctl = GetValue(control, i);
      uint16_t value = GetValue(counter, i);

      GameEngine->DrawString(pos, "CT" + std::to_string(i), *AnyType<DARK_GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + vOffset, Utils::Int2Hex(value, 4), *AnyType<GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x, 0), Utils::Int2Hex(ctl), *AnyType<GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 4, 0), cs ? "1" : "0", *AnyType<GREY, ColorT>::GetValue());
      GameEngine->DrawString(pos + vOffset * 2 + olc::vi2d(vStep.x * 6, 0), out ? "1" : "0", *AnyType<GREY, ColorT>::GetValue());

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
    while (bExec) {
      std::this_thread::sleep_for(std::chrono::nanoseconds(clock.first));

      // NOTE: Because run of this loop takes around 60ms, note generations (CT0) was moved to the func Sound 
      for (uint8_t i = 1; i < 3; i++) {
        Utils::Lock l(mutex);
        uint16_t& value = GetValue(counter, i);
        uint16_t& out = GetValue(output, i);
        uint16_t enabled = GetValue(gate, i);

        if (!enabled) continue;

        if (out & ENABLE_OUTPUT) Interrupt(i);
        if (!(out & KEEP_OUTPUT)) out = !ENABLE_OUTPUT;

        if (value) { value = value - 1; continue; }

        uint16_t ctl = GetValue(control, i);
        uint16_t init = GetValue(initialize, i);

        switch ((ctl & 0x0E) >> 1) {
          case 0x00: break; // NOTE: There is not point to create MODE0 it will not be used at all 
          case 0x01: break; // NOTE: There is not point to create MODE1 it will not be used at all 

          case 0x02: case 0x04: case 0x05: value = init; out = ENABLE_OUTPUT; break;
          case 0x03: value = init; out = ((out & ENABLE_OUTPUT) ^ ENABLE_OUTPUT) | KEEP_OUTPUT; break;
        }
      }
    }
  }

public:
  uint8_t Read(uint32_t addr, bool) { 
    Utils::Lock l(mutex);

    uint16_t& ctl = GetValue(control, addr);
    const uint16_t value = GetValue(counter, addr);
    
    switch ((ctl & 0x0300) >> 8) {
      case 0x00: return (uint8_t)(value & 0x00FF);
      case 0x01: return (uint8_t)((value & 0xFF00) >> 8);
      case 0x02: {
        ctl = (ctl & 0x00FF);
        return (uint8_t)(value & 0x00FF);
      }
    }

    return 0x00;
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) {
    Utils::Lock l(mutex);

    if ((addr & 0x0F) != 0x03) {
      const uint8_t index = addr & 0x0F;
      uint16_t& ctl = GetValue(control, index);
      uint16_t& value = GetValue(counter, index);

      switch ((ctl & 0x0300) >> 8) {
        case 0x02: ctl = (ctl & 0x00FF) | (uint16_t)(0x0100);
        case 0x00: value = (value & 0xFF00) | data; break;
        case 0x01: value = (value & 0x00FF) | (uint16_t)data << 8; break;
      }

      switch ((ctl & 0x0E) >> 1) {
        case 0x02: case 0x03: case 0x05: GetValue(initialize, index) = value;
        default: GetValue(initialize, index) = 0x00;
      }

      return data;
    }

    const uint8_t index = (data & 0xC0) >> 6;
    uint16_t& ctrl = GetValue(control, index);

    switch ((data & 0x30) >> 4) {
      case 0x00: {
        ctrl = (uint16_t)data | (uint16_t)(0x0200);
        GetValue(copy, index) = GetValue(counter, index); // Copy current state of the reg to the copy reg
        break;
      }

      case 0x01: ctrl = (uint16_t)data | (uint16_t)(0x0000); break; // LOW BYTE
      case 0x02: ctrl = (uint16_t)data | (uint16_t)(0x0100); break; // HIGH BYTE
      case 0x03: ctrl = (uint16_t)data | (uint16_t)(0x0200); break; // LOW BYTE + HIGH BYTE
    }

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
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);

  std::pair<int32_t, int32_t> clock; // nanoseconds, Hz

  std::atomic<bool> bExec = true;
  std::unique_ptr<std::thread> runtime = nullptr;

  Bus* bus;

  std::mutex mutex;
  
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
