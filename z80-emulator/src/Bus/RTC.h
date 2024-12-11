#pragma once
#include "Memory.h"

namespace Bus {

class RTC : public Memory<MemoryT::MC146818, MC146818_SIZE> {
  struct ADDR {
    struct CLOCK { enum { SECONDS = 0, MINUTES = 2, HOURS = 4 }; };
    struct ALARM { enum { SECONDS = 1, MINUTES = 3, HOURS = 5 }; };
    struct CALENDAR { enum { DAY_OF_WEEK = 6, DATE_OF_MONTH, MONTH, YEAR }; };
    struct REG { enum { A = 10, B, C, D }; };
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

  void Runtime() {
    while (bExec) {
      std::this_thread::sleep_for(std::chrono::seconds(1));

      // TODO: Finish impl this !!!
      const uint8_t seconds = memory[ADDR::CLOCK::SECONDS] + 1;
      const uint8_t minutes = memory[ADDR::CLOCK::MINUTES] + seconds / 60;
      const uint8_t hours   = memory[ADDR::CLOCK::HOURS]   + minutes / 60;

      memory[ADDR::CLOCK::SECONDS] = seconds % 60;
      memory[ADDR::CLOCK::MINUTES] = minutes % 60;
      memory[ADDR::CLOCK::HOURS]   = hours   % 24;
    }
  }

  // void Draw(PixelGameEngine* GameEngine) {
  //   int32_t index = 0;
  //   olc::vi2d pos = absolute;

  //   auto DrawReg = [&](std::string name, uint16_t val) {
  //     GameEngine->DrawString(pos, name, *AnyType<DARK_GREY, ColorT>::GetValue());
  //     GameEngine->DrawString(pos + vOffset, Utils::Int2Hex(val), *AnyType<GREY, ColorT>::GetValue());

  //     pos.x += vOffset.x + vStep.x * 3;
  //   };

  //   DrawReg("A", regA()); DrawReg("B", regB()); DrawReg("C", regC());
  // }


  // uint8_t Read(uint32_t addr, bool) { 
  //   switch (addr & 3) {
  //     case REG::A: return regA(Int2Type<REG2DATA>());
  //     case REG::B: return regB(Int2Type<REG2DATA>());
  //     case REG::C: return regC(Int2Type<REG2DATA>());
  //   }

  //   return 0x00;
  // }


  // uint8_t Write(uint32_t addr, uint8_t data, bool) {
  //   switch (addr & 3) {
  //     case REG::A: return regA(Int2Type<DATA2REG>(), data);
  //     case REG::B: return regB(Int2Type<DATA2REG>(), data);
  //     case REG::C: return regC(Int2Type<DATA2REG>(), data);
  //   }


  //   // Mode set
  //   if (BIT(data, 7)) {
  //     options[REG::CL].first = options[REG::B].first = BIT(data, 2) ? MODE1 : MODE0;
  //     options[REG::CH].first = options[REG::A].first = BIT(data, 6) ? MODE2 : BIT(data, 5) ? MODE1 : MODE0;

  //     options[REG::CL].second = BIT(data, 0); options[REG::B].second = BIT(data, 1);
  //     options[REG::CH].second = BIT(data, 3);  options[REG::A].second = BIT(data, 4);
  //   } else {
  //     uint8_t mask = MASK((data >> 1) & 0x07);
  //     regC() = regC() & (mask ^ 0xFF) | (BIT(data, 0) ? mask : 0x00);
  //   }

  //   return data;
  // }

private:
  std::atomic<bool> bExec = true;
  std::unique_ptr<std::thread> runtime = nullptr;
};
};
