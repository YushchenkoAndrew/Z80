#pragma once
#include "Memory.h"

namespace Bus {

#define MASK(pos) (1 << (pos))

class RTC : public Memory<MemoryT::MC146818, MC146818_SIZE> {
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
  RTC(Bus* b): Memory(b) {}

  ~RTC() {
    bExec = false;

    if (runtime != nullptr && runtime->joinable()) runtime->join();
  }

  void Preinitialize() {
    Memory::Preinitialize();

    if (runtime == nullptr) runtime = std::make_unique<std::thread>(std::thread(&RTC::Runtime, this));
  }

  void Interrupt();
  inline void IRQF() { memory[ADDR::REG::C] |= MASK(FLAG::REG_C::IRQF); }

  void Runtime() {
    uint64_t time = 0;

    while (bExec) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      const uint8_t regB = memory[ADDR::REG::B];
      const uint8_t bBIN = regB & MASK(FLAG::REG_B::DM);
      const uint8_t iHour = (regB & MASK(FLAG::REG_B::h24)) ? 24 : 12;

      if (regB & MASK(FLAG::REG_B::SET)) continue;

      time++;
      memory[ADDR::REG::A] |= MASK(FLAG::REG_A::UIP);
      memory[ADDR::REG::C] &= ~(MASK(FLAG::REG_C::UF) | MASK(FLAG::REG_C::IRQF));

      if (bBIN) {
        memory[ADDR::CLOCK::SECONDS] = time % 60;
        memory[ADDR::CLOCK::MINUTES] = (time / 60) % 60;
        memory[ADDR::CLOCK::HOURS]   = (time / 3600) % iHour;
      } else {
        memory[ADDR::CLOCK::SECONDS] = (time % 10) | (((time % 60) / 10) << 4);
        memory[ADDR::CLOCK::MINUTES] = ((time / 60) % 10) | ((((time / 60) % 60) / 10) << 4);
        memory[ADDR::CLOCK::HOURS]   = ((time / 3600) % 10) | ((((time / 3600) % iHour) / 10) << 4);
      }

      memory[ADDR::CALENDAR::DAY_OF_WEEK] = 1; // NOTE: Not sure that I need this in emulator
      memory[ADDR::CALENDAR::DATE_OF_MONTH] = 1 + time / 3600 / 24;
      memory[ADDR::CALENDAR::MONTH] = 1 + time / 3600 / 24 / 30;
      memory[ADDR::CALENDAR::YEAR] = time / 3600 / 24 / 356;

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

  uint8_t Write(uint32_t addr, uint8_t data, bool) {
    switch (addr) {
      case ADDR::REG::C: return Memory::Write(addr, data & 0xF0, false);
      case ADDR::REG::D: return Memory::Write(addr, data & 0x10, false);
    }

    return Memory::Write(addr, data, false);
  }


private:
  std::atomic<bool> bExec = true;
  std::unique_ptr<std::thread> runtime = nullptr;
};

#undef MASK
};
