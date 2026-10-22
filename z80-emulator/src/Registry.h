#pragma once

#include "include/LuaScript.h"
#include "src/Bus/Cpu.h"
#include "src/Bus/Defs.h"
#include "src/Bus/Hex.h"
#include "src/Bus/IRQ.h"
#include "src/Bus/Keyboard.h"
#include "src/Bus/Lcd.h"
#include "src/Bus/Led.h"
#include "src/Bus/Memory.h"
#include "src/Bus/PIT.h"
#include "src/Bus/PPI.h"
#include "src/Bus/RTC.h"
#include "src/Bus/Switch.h"
#include "src/Defs.h"
#include "src/Editor/Editor.h"
#include "src/Editor/Popup.h"
#include "src/Interpreter/Interpreter.h"
#include "src/Window/Window.h"
#include <memory>
#include <unordered_set>

class Registry {
private:
  typedef Bus::Memory<W27C512_SIZE> W27C512_T;
  typedef Bus::Memory<IMS1423_SIZE> IMS1423_T;
  typedef Bus::Memory<HY62256A_SIZE> HY62256A_T;

public:
  static void Initialize(PixelGameEngine* engine, LuaScript& lua) {
    const auto clock = lua.GetOr("CLOCK", 1);

    windows.insert(Bus = std::make_shared<Bus::Bus>(engine, "bus"));
    windows.insert(Led = std::make_shared<Bus::Led>(engine, "led"));
    windows.insert(Lcd = std::make_shared<Bus::LCD>(engine, "lcd"));
    windows.insert(Hex = std::make_shared<Bus::Hex>(engine, "hex"));
    windows.insert(IRQ = std::make_shared<Bus::IRQ>(engine, "irq"));

    windows.insert(KR580VV55 = std::make_shared<Bus::PPI>(engine, "KR580VV55"));
    windows.insert(Switch = std::make_shared<Bus::Switch>(engine, "switch"));
    windows.insert(Keyboard = std::make_shared<Bus::Keyboard>(engine, "keyboard"));

    windows.insert(Z80 = std::make_shared<Bus::Z80::CPU>(engine, "Z80", clock));
    windows.insert(KR580VI53 = std::make_shared<Bus::PIT>(engine, "KR580VI53", clock * 8));

    windows.insert(W27C512 = std::make_shared<W27C512_T>(engine, "W27C512"));
    windows.insert(IMS1423 = std::make_shared<IMS1423_T>(engine, "IMS1423"));
    windows.insert(MC146818 = std::make_shared<Bus::RTC>(engine, "MC146818"));
    windows.insert(HY62256A = std::make_shared<HY62256A_T>(engine, "HY62256A"));

    windows.insert(Editor = std::make_shared<Editor::Editor>(engine, "editor"));
    windows.insert(Bindings = std::make_shared<Editor::Popup>(engine, "bindings"));

    Interpreter = std::make_shared<Interpreter::Interpreter>();
  }

  static std::shared_ptr<Window::Window> GetInstance(std::string id) {
    for (auto& ptr : windows) {
      if (ptr->windowId == id) return ptr;
    }

    return nullptr;
  }

public:
  inline static std::shared_ptr<Bus::Bus> Bus = nullptr;
  inline static std::shared_ptr<Bus::Led>        Led;
  inline static std::shared_ptr<Bus::LCD>        Lcd;
  inline static std::shared_ptr<Bus::Hex>        Hex;

  inline static std::shared_ptr<Bus::IRQ>        IRQ;
  inline static std::shared_ptr<Bus::Switch>     Switch;
  inline static std::shared_ptr<Bus::RTC>        MC146818;
  inline static std::shared_ptr<Bus::PIT>        KR580VI53;
  inline static std::shared_ptr<Bus::PPI>        KR580VV55;
  inline static std::shared_ptr<Bus::Keyboard>   Keyboard;
  inline static std::shared_ptr<Bus::Z80::CPU>   Z80;

  inline static std::shared_ptr<W27C512_T>       W27C512;
  inline static std::shared_ptr<IMS1423_T>       IMS1423;
  inline static std::shared_ptr<HY62256A_T>      HY62256A;

  inline static std::shared_ptr<Editor::Popup>   Bindings;
  inline static std::shared_ptr<Editor::Editor>  Editor;

  inline static std::shared_ptr<Interpreter::Interpreter> Interpreter;

private:
  inline static std::unordered_set<std::shared_ptr<Window::Window>> windows;
};