#pragma once

#include "src/Bus/Device.h"
#include "src/Window/Window.h"


namespace Bus {

#define BIT(word, pos) ((word >> pos) & 0x01)
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
  Bus(PixelGameEngine* engine, std::string id): Window::Base(engine), Window::Window(engine, std::move(id)) {}

  uint8_t Read(uint32_t addr, bool mreq) override;
  uint8_t Write(uint32_t addr, uint8_t data, bool mreq) override;

private:
  uint8_t mux(Int2Type<MREQ>, uint32_t addr16);
  uint8_t mux(Int2Type<MREQ>, uint32_t addr16, uint8_t data);
  uint8_t mux(Int2Type<IORQ>, uint32_t addr);
  uint8_t mux(Int2Type<IORQ>, uint32_t addr, uint8_t data);

private:
  uint8_t ctrl = 0;
  std::list<std::pair<olc::vi2d, olc::vi2d>> grid;
};

#undef BIT

};
