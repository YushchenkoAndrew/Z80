#pragma once

#include "include/LuaScript.h"
#include "src/Bus/Defs.h"
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

  void Select(DimensionT dimensions) override {}
  void Process() override {}
  void Draw() override {}

  inline DisassembleT Disassemble();

  uint8_t Read(uint32_t addr, bool mreq);
  uint8_t Write(uint32_t addr, uint8_t data, bool mreq);

private:
  inline uint8_t mux(Int2Type<MREQ>, uint32_t addr16);
  inline uint8_t mux(Int2Type<MREQ>, uint32_t addr16, uint8_t data);
  inline uint8_t mux(Int2Type<IORQ>, uint32_t addr);
  inline uint8_t mux(Int2Type<IORQ>, uint32_t addr, uint8_t data);

// public:
  // inline std::string GetId() { return typeid(this).name(); }
  // inline std::string GetName() { return "bus"; }
  // inline std::string GetBindings() { return "bus"; }

public:
  std::list<::Window::Window> windows;

  // std::shared_ptr<Led> led;
  // std::shared_ptr<Switch> switches;
  // std::shared_ptr<HexDisplay> hexDisplay;
  // std::shared_ptr<LCD> lcd;
  // std::shared_ptr<Keyboard> keyboard;

  // std::shared_ptr<PPI> KR580VV55; // PPI КР580ВВ55А
  // std::shared_ptr<PIT> KR580VI53; // Counter КР580ВИ53
  // std::shared_ptr<RTC> MC146818;

  // std::shared_ptr<IRQ> interrupt;

  // std::shared_ptr<Z80::CPU> Z80;
  // std::shared_ptr<W27C512_T> W27C512;
  // std::shared_ptr<IMS1423_T> IMS1423;
  // std::shared_ptr<HY62256A_T> HY62256A;

  // TODO: RAM HY62256A A14 = 0 A15 = 1 
  // TODO: ROM KM28C17-20 A14 = 1 A15 = 1


private:
  // LuaScript& luaConfig;

  std::list<std::pair<olc::vi2d, olc::vi2d>> grid;
};

#undef BIT

};
