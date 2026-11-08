#pragma once

#include "src/Bus/Defs.h"
#include "src/Bus/Memory.h"
#include "src/Defs.h"

namespace Bus {

#define MASK(pos) (1 << (pos))

class RTC : public Memory {
  struct ADDR {
    struct CLOCK { enum { SECONDS = 0, MINUTES = 2, HOURS = 4 }; };
    struct ALARM { enum { SECONDS = 1, MINUTES = 3, HOURS = 5 }; };
    struct CALENDAR { enum { DAY_OF_WEEK = 6, DATE_OF_MONTH, MONTH, YEAR }; };
    struct REG { enum { A = 10, B, C, D }; };

  };

  struct FLAG {
    struct REG_A { enum { RS0, RS1, RS2, RS3, DV0, DV1, DV2, UIP }; };
    struct REG_B { enum { DSE, h24, DM, SQWE, UIE, AIE, PIE, SET }; };
    struct REG_C { enum { UF = 4, AF, PF, IRQF }; };
    struct REG_D { enum { VRT = 7 }; };
  };

public:
  RTC(PixelGameEngine* engine, std::string id): Window::Base(engine), Memory(engine, std::move(id), MC146818_SIZE), runtime(std::thread(std::thread(&RTC::Runtime, this))) {}

  ~RTC() { bExec = false; if (runtime.joinable()) runtime.join(); }

  void Interrupt();
  inline void IRQF() { memory[ADDR::REG::C] |= MASK(FLAG::REG_C::IRQF); }

  void Runtime() {
    while (bExec) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      const uint8_t regB = memory[ADDR::REG::B];
      const uint8_t bBIN = regB & MASK(FLAG::REG_B::DM);
      const uint8_t iHour = (regB & MASK(FLAG::REG_B::h24)) ? 24 : 12;

      if (regB & MASK(FLAG::REG_B::SET)) continue;

      time++;
      memory[ADDR::REG::A] |= MASK(FLAG::REG_A::UIP);
      memory[ADDR::REG::C] &= ~(MASK(FLAG::REG_C::UF) | MASK(FLAG::REG_C::IRQF));

      auto get = [&](int r) -> uint8_t { return bBIN ? memory[r] : FromBCD(memory[r]); };
      auto set = [&](int r, uint8_t v)  { memory[r] = bBIN ? v : ToBCD(v); };

      set(ADDR::CLOCK::SECONDS, time % 60);
      set(ADDR::CLOCK::MINUTES, (time / 60) % 60);
      set(ADDR::CLOCK::HOURS,   (time / 3600) % iHour); // wrap first, then split into digits

      // midnight: once per day
      if (time % 86400 == 0) {                                     
        set(ADDR::CALENDAR::DAY_OF_WEEK, get(ADDR::CALENDAR::DAY_OF_WEEK) % 7 + 1);
        uint8_t date = get(ADDR::CALENDAR::DATE_OF_MONTH) + 1, month = get(ADDR::CALENDAR::MONTH), year = get(ADDR::CALENDAR::YEAR);
        if (date > DaysIn(month, year)) { date = 1; if (++month > 12) { month = 1; year = (year + 1) % 100; } }
        set(ADDR::CALENDAR::DATE_OF_MONTH, date); set(ADDR::CALENDAR::MONTH, month); set(ADDR::CALENDAR::YEAR, year);
      }

      memory[ADDR::REG::A] &= ~MASK(FLAG::REG_A::UIP);

      const uint8_t bAFSec  = memory[ADDR::CLOCK::SECONDS] == memory[ADDR::ALARM::SECONDS] || memory[ADDR::ALARM::SECONDS] >= 0xC0;
      const uint8_t bAFMin  = memory[ADDR::CLOCK::MINUTES] == memory[ADDR::ALARM::MINUTES] || memory[ADDR::ALARM::MINUTES] >= 0xC0;
      const uint8_t bAFHour = memory[ADDR::CLOCK::HOURS]   == memory[ADDR::ALARM::HOURS]   || memory[ADDR::ALARM::HOURS]   >= 0xC0;


      if (bAFSec && bAFMin && bAFHour) {
        memory[ADDR::REG::C] |= MASK(FLAG::REG_C::AF);
        if (memory[ADDR::REG::B] & MASK(FLAG::REG_B::AIE)) Interrupt();
      }

      memory[ADDR::REG::C] |= MASK(FLAG::REG_C::UF);
      if (memory[ADDR::REG::B] & MASK(FLAG::REG_B::UIE)) Interrupt();


      // TODO: PIE interrupt
    }
  }

  void Acknowledge();
  uint8_t Read(uint32_t addr, bool) override {
    addr &= memory.size() - 1;
    
    const uint8_t v = Memory::Read(addr, false);
    if (addr == ADDR::REG::C) { memory[addr] &= 0x0F; Acknowledge(); }

    return v;
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) override {
    addr &= memory.size() - 1;
    const bool bin = memory[ADDR::REG::B] & MASK(FLAG::REG_B::DM);
    const uint64_t v = bin ? data : FromBCD(data);

    switch (addr) {
      case ADDR::REG::C: return Memory::Write(addr, data & 0xF0, false);
      case ADDR::REG::D: return Memory::Write(addr, data & 0x10, false);
      case ADDR::CLOCK::SECONDS: time = (time / 60) * 60 + v; break;
      case ADDR::CLOCK::MINUTES: time = (time / 3600) * 3600 + v * 60 + time % 60; break;
      case ADDR::CLOCK::HOURS:   time = (time / 86400) * 86400 + v * 3600 + time % 3600; break;
    }
    return Memory::Write(addr, data, false);
  }

private:
  static uint8_t FromBCD(uint8_t v) { return (v >> 4) * 10 + (v & 0x0F); }
  static uint8_t ToBCD(uint8_t v)   { return ((v / 10) << 4) | (v % 10); }
  static uint8_t DaysIn(uint8_t m, uint8_t y) {
    static const uint8_t d[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    return (m == 2 && y % 4 == 0) ? 29 : d[(m + 11) % 12];
  }

private:
  std::atomic<uint64_t> time = 0;
  std::atomic<bool> bExec = true;

  std::thread runtime;
};

#undef MASK
};
