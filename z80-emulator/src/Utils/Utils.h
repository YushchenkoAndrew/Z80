#pragma once
#include "src/Defs.h"

namespace Utils {

  inline std::string Int2Hex(int32_t i, int32_t width = 2) { 
    std::stringstream ss; ss << std::setfill('0') << std::setw(width) << std::hex << std::uppercase << +i;
    return ss.str();
  }

  inline std::string Int2Bin(int32_t num, int32_t size = 1) { 
    std::string str = "";

    for (int8_t j = size - 1; j >= 0; j--) {
      for (int8_t i = 7; i >= 0; i--) {
        str += !!(num & (1 << (i + j * 8))) + '0';
        if (i + j && (i + j) % 4 == 0) str += ' ';
      }
    }

    return str;
  }

  inline bool IsDigit(const char &c) { return c >= '0' && c <= '9'; }
  inline bool IsBinDigit(const char &c) { return c == '0' || c == '1'; }
  inline bool IsOctDigit(const char &c) { return c >= '0' && c <= '7'; }
  inline bool IsHexDigit(const char &c) { return IsDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
  inline bool IsAlpha(const char &c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }


  inline uint8_t Hex2Int(const char c) { 
    if (IsDigit(c)) return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 0x0A;
    if (c >= 'A' && c <= 'F') return c - 'A' + 0x0A;
    return 0;
  }


class Lock {
public:
  Lock(std::mutex& m): mut(m) { while (!mut.try_lock()) std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
  ~Lock() { mut.unlock(); }

private:
  std::mutex& mut;
};
};
