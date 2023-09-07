#pragma once
#include "Bus.h"

namespace Bus {

/**
 * EEPROM W27C512 
 *  65536 × 8 bits
 */
class Rom {
public:

  void reset() { for (auto& bank : memory) bank = 0x00; }

  void load(std::vector<uint8_t> code) {
    reset();

    for (uint32_t i = 0; i < code.size(); i++) memory[i] = code[i];
  }

  void Draw(olc::PixelGameEngine* GameEngine, olc::vi2d absolute = olc::vi2d(0, 0)) {
    std::stringstream ss;

    olc::vi2d step = { 24, 8 };
    olc::vi2d offset = { 44, 12 };
    
    for (int32_t i = 0; i < 0x10; i++) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * step + olc::vi2d(offset.x, 0);

      ss.str(""); ss << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << i;
      GameEngine->DrawString(pos, ss.str(), AnyType<DARK_GREY, olc::Pixel>::GetValue());
    }

    for (int32_t i = 0; i < 0x10; i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * step + olc::vi2d(0, offset.y);

      ss.str(""); ss << std::setfill('0') << std::setw(5) << std::hex << std::uppercase << i;
      GameEngine->DrawString(pos, ss.str(), AnyType<DARK_GREY, olc::Pixel>::GetValue());
      
      for (int32_t j = 0; j < 0x10; j++) {
        int32_t index = (i << 4) | j;
        olc::vi2d pos = absolute + olc::vi2d(j, i) * step + offset;

        ss.str(""); ss << std::setfill('0') << std::setw(2) << std::hex << +memory[index];
        GameEngine->DrawString(pos, ss.str(), AnyType<WHITE, olc::Pixel>::GetValue());
      }
    }

  }


private:
  std::array<uint8_t, 65536> memory;
};

};
