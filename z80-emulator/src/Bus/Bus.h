#pragma once
#include "Cpu.h"

namespace Bus {

class Bus : public Window::Window, public Device {
private:
  struct MUX { 
    enum MREQ { W27C512, IMS1423, HY62256A, KM28C17 };
    enum IORQ { IO_DEVICE, HEX_DEVICE, PPI_PORT, UART_PORT };
  };

  enum REQUEST { MREQ, IORQ };

public:

  Bus():
    led(std::make_shared<Led>(this)),
    switches(std::make_shared<Switch>(this)),

    Z80(std::make_shared<Z80::CPU>(this)),
    W27C512(std::make_shared<Memory<MemoryT::EEPROM, W27C512_SIZE>>(this)) {}

  void Initialize(DimensionT) {
    // TODO:
  }

  void Process(PixelGameEngine* GameEngine) {
    led->Process(GameEngine);
    switches->Process(GameEngine);
  }

  void Draw(PixelGameEngine* GameEngine) {
    led->Draw(GameEngine);
    switches->Draw(GameEngine);
  }

  uint8_t Read(uint32_t addr, bool mreq) {
    auto device = mreq ? mux(Int2Type<MREQ>(), addr, true) : mux(Int2Type<IORQ>(), addr, true);
    if (device == nullptr) return 0x00;

    return device->Read(addr, mreq);
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool mreq) {
    auto device = mreq ? mux(Int2Type<MREQ>(), addr, false) : mux(Int2Type<IORQ>(), addr, false);
    if (device == nullptr) return 0x00;

    return device->Write(addr, data, mreq);
  }

  inline DisassembleT Disassemble() { return Z80->Disassemble(); }

private:
  inline std::shared_ptr<Device> mux(Int2Type<MREQ>, uint32_t addr, bool read) {
    switch((addr & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: if (read) return W27C512; else return nullptr;
      case MUX::MREQ::IMS1423: 
      case MUX::MREQ::HY62256A: // TODO:
      case MUX::MREQ::KM28C17: break;
    }

    return nullptr;
  }

  inline std::shared_ptr<Device> mux(Int2Type<IORQ>, uint32_t addr, bool read) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IO_DEVICE: if (read) return switches; else return led;
      case MUX::IORQ::HEX_DEVICE: 
      case MUX::IORQ::PPI_PORT: // TODO:
      case MUX::IORQ::UART_PORT: break; 
    }

    return nullptr;
  }


public:
  std::shared_ptr<Led> led;
  std::shared_ptr<Switch> switches;

  std::shared_ptr<Z80::CPU> Z80;
  std::shared_ptr<Memory<MemoryT::EEPROM, W27C512_SIZE>> W27C512;

  // TODO: RAM HY62256A A14 = 0 A15 = 1 
  // TODO: RAM ims1423  A14 = 1 A15 = 0  (NOTE: Without A13)
  // TODO: ROM KM28C17-20 A14 = 1 A15 = 1
};

};
