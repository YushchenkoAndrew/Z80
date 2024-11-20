#pragma once
#include "Cpu.h"

namespace Bus {

// #define BIT(word, pos) ((word >> pos) & 0x01)
class Bus : public Window::Window, public Device {
private:
  struct MUX { 
    enum MREQ  { W27C512, STACK, HY62256A, KM28C17 };
    enum STACK { IMS1423_1, IMS1423_2, MC146818 };
    enum IORQ  { IN_OUT_PORT, HEX_PORT, PPI_PORT, UART_PORT, KEYBOARD_PORT, INT_PORT, _, _NONE, PIT_PORT  }; // TODO: Check why IORQ 6 is not used, 7 - was used in RLT but it was a bug so it was changed to STACK::MC146818
    enum PPI_CS { CS0, CS1, CSA, CSW, CS3, CS4, CS5, CSB };
    enum PPI_IO { DISABLE, LCD_PORT, PWR_PORT };
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
    auto mmu = KR580VV55->regB(Int2Type<PPI::REG2PORT>());

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return W27C512->Read(addr | ((mmu & 0x03) << 14), true);
      case MUX::MREQ::HY62256A: // TODO:
      case MUX::MREQ::KM28C17: break;
      case MUX::MREQ::STACK: {
        switch((addr16 & 0x3000) >> 12) {
          case MUX::STACK::IMS1423_1: 
          case MUX::STACK::IMS1423_2: return IMS1423->Read(addr, true);
          case MUX::STACK::MC146818: break; // TODO:
        }
      }
    }

    return 0x00;
  }

  inline uint8_t mux(Int2Type<MREQ>, uint32_t addr16, uint8_t data) {
    auto addr = addr16 & 0x3FFF;
    auto mmu = KR580VV55->regB(Int2Type<PPI::REG2PORT>());

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return 0x00;
      case MUX::MREQ::HY62256A: // TODO:
      case MUX::MREQ::KM28C17: break;
      case MUX::MREQ::STACK: {
        switch((addr16 & 0x3000) >> 12) {
          case MUX::STACK::IMS1423_1: 
          case MUX::STACK::IMS1423_2: return IMS1423->Write(addr, data, true);
          case MUX::STACK::MC146818: break; // TODO:
        }
      }
    }

    return 0x00;
  }

  inline uint8_t mux(Int2Type<IORQ>, uint32_t addr) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT:   return switches->Read(addr & 0x00FF, true);
      case MUX::IORQ::HEX_PORT:      return 0x00;
      case MUX::IORQ::PPI_PORT:      return KR580VV55->Read(addr & 0x00FF, true);
      case MUX::IORQ::UART_PORT:     break; // TODO
      case MUX::IORQ::KEYBOARD_PORT: return interrupt->SetFlag(InterruptVector::IRQ::KEYBOARD), keyboard->Read(addr, true);
      case MUX::IORQ::INT_PORT:      return interrupt->Read(addr, true);
      case MUX::IORQ::PIT_PORT: {
        interrupt->SetFlag(InterruptVector::IRQ::CT1);
        interrupt->SetFlag(InterruptVector::IRQ::CT2);

        return KR580VI53->Read(addr, true);
      }
    }

    return 0x00;
  }

  inline uint8_t mux(Int2Type<IORQ>, uint32_t addr, uint8_t data) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT:   return led->Write(addr, data, true);
      case MUX::IORQ::HEX_PORT:      return hexDisplay->Write(addr, data, true); 
      case MUX::IORQ::UART_PORT:     break; // TODO
      case MUX::IORQ::KEYBOARD_PORT: return 0x00;
      case MUX::IORQ::INT_PORT:      return interrupt->Write(addr, data, true);
      case MUX::IORQ::PIT_PORT: {
        interrupt->SetFlag(InterruptVector::IRQ::CT1);
        interrupt->SetFlag(InterruptVector::IRQ::CT2);

        return KR580VI53->Write(addr, data, true);
      }
      case MUX::IORQ::PPI_PORT: {
        KR580VV55->Write(addr, data, true);
        uint8_t ctrl = KR580VV55->regC(Int2Type<PPI::REG2PORT>());

        KR580VI53->SetGate(0, BIT(ctrl, MUX::PPI_CS::CS0));
        KR580VI53->SetGate(1, BIT(ctrl, MUX::PPI_CS::CS1));

        switch ((ctrl >> MUX::PPI_CS::CS3) & 7) {
          case MUX::PPI_IO::LCD_PORT:
            if (BIT(ctrl, MUX::PPI_CS::CSW)) lcd->Read(addr, BIT(ctrl, MUX::PPI_CS::CSA));
            else lcd->Write(addr, KR580VV55->regA(Int2Type<PPI::REG2PORT>()), BIT(ctrl, MUX::PPI_CS::CSA));
            break;
        }

        return data;
      }
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

  std::shared_ptr<PPI> KR580VV55; // PPI КР580ВВ55А
  std::shared_ptr<PIT> KR580VI53; // Counter КР580ВИ53
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
