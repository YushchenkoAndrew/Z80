#pragma once

#include <cstdint>

namespace Bus {

class Device {
public:
  ~Device() = default;

  virtual uint8_t Read(uint32_t addr, bool) = 0;
  virtual uint8_t Write(uint32_t addr, uint8_t data, bool) = 0;
};

};