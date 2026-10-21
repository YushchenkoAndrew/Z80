#pragma once

#include "src/Bus/Cpu.h"
#include "src/Bus/Defs.h"
#include "src/Bus/IRQ.h"
#include "src/Bus/Keyboard.h"
#include "src/Bus/Lcd.h"
#include "src/Bus/Led.h"
#include "src/Bus/Memory.h"
#include "src/Bus/RTC.h"
#include "src/Bus/Switch.h"
#include "src/Defs.h"
#include "src/Editor/Editor.h"
#include "src/Editor/Popup.h"
#include "src/Interpreter/Interpreter.h"
#include <memory>

class Registry {
private:
  typedef Bus::Memory<Bus::MemoryT::W27C512, W27C512_SIZE> W27C512_T;
  typedef Bus::Memory<Bus::MemoryT::IMS1423, IMS1423_SIZE> IMS1423_T;
  typedef Bus::Memory<Bus::MemoryT::HY62256A, HY62256A_SIZE> HY62256A_T;

public:
  static void Initialize(PixelGameEngine* engine) {
    // Bus() = std::make_shared<Bus::Bus>(nullptr);
    Led() = std::make_shared<Bus::Led>(engine, Bus());
    Lcd() = std::make_shared<Bus::LCD>(engine, Bus());
    // MC146818() { static std::shared_ptr<Bus::RTC>        value = nullptr; return value; }
    // KR580VI53(){ static std::shared_ptr<Bus::PIT>        value = nullptr; return value; }
    // KR580VV55(){ static std::shared_ptr<Bus::PPI>        value = nullptr; return value; }
    // Switch()   { static std::shared_ptr<Bus::Switch>     value = nullptr; return value; }
    // Keyboard() { static std::shared_ptr<Bus::Keyboard>   value = nullptr; return value; }
    // Hex()      { static std::shared_ptr<Bus::HexDisplay> value = nullptr; return value; }
    // Interrupt(){ static std::shared_ptr<Bus::Interrupt>  value = nullptr; return value; }
    // Z80()      { static std::shared_ptr<Bus::Z80::CPU>   value = nullptr; return value; }

    // W27C512()  { static std::shared_ptr<W27C512_T>       value = nullptr; return value; }
    // IMS1423()  { static std::shared_ptr<IMS1423_T>       value = nullptr; return value; }
    // HY62256A() { static std::shared_ptr<HY62256A_T>      value = nullptr; return value; }

    Editor()   = std::make_shared<Editor::Editor>(engine);
    Bindings() = std::make_shared<Editor::Popup>(engine);

    Interpreter() = std::make_shared<Interpreter::Interpreter>();
  }

  static std::shared_ptr<Window::Window> GetInstance(std::string name) {
    if (name == "Z80")      return Z80();
    if (name == "W27C512")  return W27C512();
    if (name == "IMS1423")  return IMS1423();
    if (name == "HY62256A") return HY62256A();
    if (name == "editor")   return Editor();
    if (name == "bindings") return Bindings();
    return nullptr;
  }

public:
  static std::shared_ptr<Bus::Bus>        Bus()       { static std::shared_ptr<Bus::Bus>        value = nullptr; return value; }
  static std::shared_ptr<Bus::Led>        Led()       { static std::shared_ptr<Bus::Led>        value = nullptr; return value; }
  static std::shared_ptr<Bus::LCD>        Lcd()       { static std::shared_ptr<Bus::LCD>        value = nullptr; return value; }
  static std::shared_ptr<Bus::IRQ>        IRQ()       { static std::shared_ptr<Bus::IRQ>        value = nullptr; return value; }
  static std::shared_ptr<Bus::RTC>        MC146818()  { static std::shared_ptr<Bus::RTC>        value = nullptr; return value; }
  static std::shared_ptr<Bus::PIT>        KR580VI53() { static std::shared_ptr<Bus::PIT>        value = nullptr; return value; }
  static std::shared_ptr<Bus::PPI>        KR580VV55() { static std::shared_ptr<Bus::PPI>        value = nullptr; return value; }
  static std::shared_ptr<Bus::Switch>     Switch()    { static std::shared_ptr<Bus::Switch>     value = nullptr; return value; }
  static std::shared_ptr<Bus::Keyboard>   Keyboard()  { static std::shared_ptr<Bus::Keyboard>   value = nullptr; return value; }
  static std::shared_ptr<Bus::HexDisplay> Hex()       { static std::shared_ptr<Bus::HexDisplay> value = nullptr; return value; }
  static std::shared_ptr<Bus::Z80::CPU>   Z80()       { static std::shared_ptr<Bus::Z80::CPU>   value = nullptr; return value; }

  static std::shared_ptr<W27C512_T>       W27C512()   { static std::shared_ptr<W27C512_T>       value = nullptr; return value; }
  static std::shared_ptr<IMS1423_T>       IMS1423()   { static std::shared_ptr<IMS1423_T>       value = nullptr; return value; }
  static std::shared_ptr<HY62256A_T>      HY62256A()  { static std::shared_ptr<HY62256A_T>      value = nullptr; return value; }

  static std::shared_ptr<Editor::Popup>   Bindings()  { static std::shared_ptr<Editor::Popup>   value = nullptr; return value; }
  static std::shared_ptr<Editor::Editor>  Editor()    { static std::shared_ptr<Editor::Editor>  value = nullptr; return value; }

  static std::shared_ptr<Interpreter::Interpreter> Interpreter(){ static std::shared_ptr<Interpreter::Interpreter> value = nullptr; return value; }
};