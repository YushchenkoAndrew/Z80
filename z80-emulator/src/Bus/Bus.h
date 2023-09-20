#pragma once
#include "Cpu.h"

namespace Bus {

class Bus : public Window::Window, public Device {
private:
  enum MUX { EEPROM };

public:

  Bus(): Z80(std::make_shared<Z80::CPU>(this)), W27C512(std::make_shared<Memory<MemoryT::EEPROM, W27C512_SIZE>>(this)) {}

  void Initialize(DimensionT) {}
  void Process(PixelGameEngine* GameEngine) {}
  void Draw(PixelGameEngine* GameEngine) {}

  uint8_t Read(uint32_t addr) {
    auto device = mux(addr);
    if (device == nullptr) return 0x00;

    return device->Read(addr);
  }

  uint8_t Write(uint32_t addr, uint8_t data) {
    auto device = mux(addr);
    if (device == nullptr) return 0x00;

    return device->Write(addr, data);
  }

  inline DisassembleT Disassemble() { return Z80->Disassemble(); }

private:
  inline std::shared_ptr<Device> mux(uint32_t addr) {
    // TODO: Change address based on schematics
    // return (addr & 0x0000) >> 16;
    switch(0x00) {
      case MUX::EEPROM: return W27C512;
    }

    return nullptr;
  }


public:
  std::shared_ptr<Z80::CPU> Z80;
  std::shared_ptr<Memory<MemoryT::EEPROM, W27C512_SIZE>> W27C512;
};

};
