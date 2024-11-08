#pragma once
#include "Cpu.h"

namespace Bus {

// #define BIT(word, pos) ((word >> pos) & 0x01)
class Bus : public Window::Window, public Device {
private:
  struct MUX { 
    enum MREQ { W27C512, IMS1423, HY62256A, KM28C17 };
    enum IORQ { IN_OUT_PORT, HEX_PORT, PPI_PORT, UART_PORT, _, INT_PORT };
    enum PPI_CS { CS0, CS1, CSA, CSW, CS3, CS4, CS5, CSB };
    enum PPI_IO { LCD_PORT = 1 };
  };

  enum REQUEST { MREQ, IORQ };

public:
  typedef Memory<MemoryT::W27C512, W27C512_SIZE> W27C512_T;
  typedef Memory<MemoryT::IMS1423, IMS1423_SIZE> IMS1423_T;

  Bus(LuaScript& config);

  void Preinitialize();
  void Initialize(DimensionT dimensions);

  void Preprocess();
  void Process(PixelGameEngine* GameEngine);

  void Draw(PixelGameEngine* GameEngine);

  inline DisassembleT Disassemble();

  uint8_t Read(uint32_t addr, bool mreq) {
    if (mreq) return mux(Int2Type<MREQ>(), addr);
    else return mux(Int2Type<IORQ>(), addr);
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool mreq) {
    if (mreq) return mux(Int2Type<MREQ>(), addr, data);
    else return mux(Int2Type<IORQ>(), addr, data);
  }

private:
  inline uint8_t mux(Int2Type<MREQ>, uint32_t addr16) {
    auto addr = addr16 & 0x3FFF;
    auto mmu = ppi->regB(Int2Type<PPI::REG2PORT>());

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return W27C512->Read(addr | ((mmu & 0x03) << 14), true);
      case MUX::MREQ::IMS1423: return IMS1423->Read(addr, true);
      case MUX::MREQ::HY62256A: // TODO:
      case MUX::MREQ::KM28C17: break;
    }

    return 0x00;
  }

  inline uint8_t mux(Int2Type<MREQ>, uint32_t addr16, uint8_t data) {
    auto addr = addr16 & 0x3FFF;
    auto mmu = ppi->regB(Int2Type<PPI::REG2PORT>());

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return 0x00;
      case MUX::MREQ::IMS1423: return IMS1423->Write(addr, data, true);
      case MUX::MREQ::HY62256A: // TODO:
      case MUX::MREQ::KM28C17: break;
    }

    return 0x00;
  }

  inline uint8_t mux(Int2Type<IORQ>, uint32_t addr) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT:return switches->Read(addr & 0x00FF, true);
      case MUX::IORQ::HEX_PORT:   return 0x00;
      case MUX::IORQ::PPI_PORT:   return ppi->Read(addr & 0x00FF, true);
      case 0x03: return keyboard->Read(addr, true);
      // FIXME: I dont exactly know how I impl hardware
      // case MUX::IORQ::UART_PORT:  break; 
    }

    return 0x00;
  }

  inline uint8_t mux(Int2Type<IORQ>, uint32_t addr, uint8_t data) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT: return led->Write(addr, data, true);
      case MUX::IORQ::HEX_PORT: return hexDisplay->Write(addr, data, true); 
      case MUX::IORQ::PPI_PORT: {
        ppi->Write(addr, data, true);
        uint8_t ctrl = ppi->regC(Int2Type<PPI::REG2PORT>());

        switch ((ctrl >> 4) & 7) {
          case MUX::PPI_IO::LCD_PORT:
            if (BIT(ctrl, MUX::PPI_CS::CSW)) lcd->Read(addr, BIT(ctrl, MUX::PPI_CS::CSA));
            else lcd->Write(addr, ppi->regA(Int2Type<PPI::REG2PORT>()), BIT(ctrl, MUX::PPI_CS::CSA));
            break;
        }

        return data;
      }
      // case MUX::IORQ::UART_PORT: break; 

      case MUX::IORQ::INT_PORT: return interrupt->Write(addr, data, true);
    }

    return 0x00;
  }

public:
  inline std::string GetName() { return "bus"; }

public:
  std::shared_ptr<Led> led;
  std::shared_ptr<Switch> switches;
  std::shared_ptr<HexDisplay> hexDisplay;
  std::shared_ptr<LCD> lcd;
  std::shared_ptr<Keyboard> keyboard;

  std::shared_ptr<PPI> ppi;
  std::shared_ptr<InterruptVector> interrupt;

  std::shared_ptr<Z80::CPU> Z80;
  std::shared_ptr<W27C512_T> W27C512;
  std::shared_ptr<IMS1423_T> IMS1423;

  // TODO: RAM HY62256A A14 = 0 A15 = 1 
  // TODO: RAM ims1423  A14 = 1 A15 = 0  (NOTE: Without A13)
  // TODO: ROM KM28C17-20 A14 = 1 A15 = 1


private:
  LuaScript& luaConfig;

  std::list<std::pair<olc::vi2d, olc::vi2d>> grid;
};

#undef BIT

};
