#include "Bus.h"
#include "src/Bus/IRQ.h"
#include "src/Bus/PIT.h"
#include "src/Registry.h"

namespace Bus {
  #define BIT(word, pos) ((word >> pos) & 0x01)

  void Keyboard::Interrupt() { Registry::IRQ->ResetFlag(IRQ::BIT::KEYBOARD); }
  void RTC::Interrupt() { IRQF(); Registry::IRQ->ResetFlag(IRQ::BIT::RTC); }
  void PIT::Interrupt(int32_t ct) { Registry::IRQ->ResetFlag(IRQ::BIT::CT1 - ct + 1); }

  uint8_t Bus::Read(uint32_t addr, bool mreq) {
    if (mreq) return mux(Int2Type<MREQ>(), addr);
    else return mux(Int2Type<IORQ>(), addr);
  }

  uint8_t Bus::Write(uint32_t addr, uint8_t data, bool mreq) {
    if (mreq) return mux(Int2Type<MREQ>(), addr, data);
    else return mux(Int2Type<IORQ>(), addr, data);
  }

  inline uint8_t Bus::mux(Int2Type<Bus::MREQ>, uint32_t addr16) {
    auto addr = addr16 & 0x3FFF;
    auto mmu = Registry::KR580VV55->pinsB();

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return Registry::W27C512->Read(addr | ((mmu & 0x03) << 14), true);
      case MUX::MREQ::HY62256A: return Registry::HY62256A->Read(addr, true);
      case MUX::MREQ::KM28C17: break;
      case MUX::MREQ::STACK: {
        switch((addr16 & 0x3000) >> 12) {
          case MUX::STACK::IMS1423_1: 
          case MUX::STACK::IMS1423_2: return Registry::IMS1423->Read(addr, true);
          case MUX::STACK::MC146818:  return Registry::MC146818->Read(addr, true);
        }
      }
    }

    return 0x00;
  }

  inline uint8_t Bus::mux(Int2Type<MREQ>, uint32_t addr16, uint8_t data) {
    auto addr = addr16 & 0x3FFF;
    auto mmu = Registry::KR580VV55->pinsB();

    switch((addr16 & 0xC000) >> 14) {
      case MUX::MREQ::W27C512: return 0x00;
      case MUX::MREQ::HY62256A: return Registry::HY62256A->Write(addr, data, true);
      case MUX::MREQ::KM28C17: break;
      case MUX::MREQ::STACK: {
        switch((addr16 & 0x3000) >> 12) {
          case MUX::STACK::IMS1423_1: 
          case MUX::STACK::IMS1423_2: return Registry::IMS1423->Write(addr, data, true);
          case MUX::STACK::MC146818:  return Registry::MC146818->Write(addr, data, true);
        }
      }
    }

    return 0x00;
  }

  inline uint8_t Bus::mux(Int2Type<IORQ>, uint32_t addr) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT:   return Registry::Switch->Read(addr & 0x00FF, true);
      case MUX::IORQ::HEX_PORT:      return 0x00;
      case MUX::IORQ::PPI_PORT:      return Registry::KR580VV55->Read(addr & 0x00FF, true);
      case MUX::IORQ::UART_PORT:     break; // TODO
      case MUX::IORQ::KEYBOARD_PORT: return Registry::IRQ->SetFlag(IRQ::BIT::KEYBOARD), Registry::Keyboard->Read(addr, true);
      case MUX::IORQ::INT_PORT:      return Registry::IRQ->Read(addr, true);
      case MUX::IORQ::PIT_PORT: {
        const int ct = addr & 3; // 0..2 counters, 3 control word
        if (ct == 1 || ct == 2) Registry::IRQ->SetFlag(IRQ::BIT::CT1 - ct + 1); // same mapping PIT::Interrupt uses
        return Registry::KR580VI53->Read(addr, true);
      }
    }

    return 0x00;
  }

  inline uint8_t Bus::mux(Int2Type<IORQ>, uint32_t addr, uint8_t data) {
    switch((addr & 0x00F0) >> 4) {
      case MUX::IORQ::IN_OUT_PORT:   return Registry::Led->Write(addr, data, true);
      case MUX::IORQ::HEX_PORT:      return Registry::Hex->Write(addr, data, true); 
      case MUX::IORQ::UART_PORT:     break; // TODO
      case MUX::IORQ::KEYBOARD_PORT: return 0x00;
      case MUX::IORQ::INT_PORT:      return Registry::IRQ->Write(addr, data, true);
      case MUX::IORQ::PIT_PORT: {
        const int ct = addr & 3; // 0..2 counters, 3 control word
        if (ct == 1 || ct == 2) Registry::IRQ->SetFlag(IRQ::BIT::CT1 - ct + 1); // same mapping PIT::Interrupt uses
        return Registry::KR580VI53->Write(addr, data, true);
      }
      case MUX::IORQ::PPI_PORT: {
        Registry::KR580VV55->Write(addr, data, true);
        const uint8_t ctrl = Registry::KR580VV55->regC(Int2Type<PPI::REG2PORT>());
        const uint8_t prev = this->ctrl; this->ctrl = ctrl;

        Registry::KR580VI53->SetGate(0, BIT(ctrl, MUX::PPI_CS::CS0));
        Registry::KR580VI53->SetGate(1, BIT(ctrl, MUX::PPI_CS::CS1));

        const bool now = ((ctrl >> MUX::PPI_CS::CS3) & 7) == MUX::PPI_IO::LCD_PORT;
        const bool was = ((prev >> MUX::PPI_CS::CS3) & 7) == MUX::PPI_IO::LCD_PORT;

        if (was && !now) {            // RS/RW as they were while the LCD was selected
          const bool rs = BIT(prev, MUX::PPI_CS::CSA);
          if (BIT(prev, MUX::PPI_CS::CSW)) Registry::Lcd->Read(addr, rs);
          else Registry::Lcd->Write(addr, Registry::KR580VV55->regA(Int2Type<PPI::REG2PORT>()), rs);
        }
        return data;
      }
    }

    return 0x00;
  }

  void RTC::Acknowledge() { Registry::IRQ->SetFlag(IRQ::BIT::RTC); }
  void IRQ::Interrupt() { Registry::Z80->Interrupt(Pending() != 0); }

  uint8_t Z80::CPU::Acknowledge() { return Registry::IRQ->Vector(); }
  uint8_t Z80::CPU::Read() { return Registry::Bus->Read(regPC()++, true); }
  uint8_t Z80::CPU::Read(uint32_t addr, bool mreq) { return Registry::Bus->Read(addr, mreq); }
  uint8_t Z80::CPU::Write(uint32_t addr, uint8_t data, bool mreq) { return Registry::Bus->Write(addr, data, mreq); }

};
