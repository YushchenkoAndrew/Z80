#pragma once
#include "Ppi.h"

namespace Bus {

class Keyboard : public Window::Window, public Device {
public:
  Keyboard(Bus* b): bus(b) {}

  void Initialize(DimensionT dimensions) {}
  void Process(PixelGameEngine* GameEngine) {
    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<KeyboardScanCodes, Keyboard>::Process(this);
  }

  void Draw(PixelGameEngine* GameEngine) {}

  void Interrupt();
  uint8_t Read(uint32_t addr, bool) { return 0x00; }
  uint8_t Write(uint32_t addr, uint8_t data, bool) { return 0x00; }

public:
  template<int32_t T, int32_t U>
  void Process(TypeList<Int2Type<T>, Int2Type<U>>) {
    const char c = static_cast<char>(Int2Type<U>().value);
    BasicStrokeHandler(static_cast<olc::Key>(Int2Type<T>().value), c, toupper(c));
  }

private:
  void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    bool bPressed = GameEngine->GetKey(key).bPressed;

    // TODO: Impl this
    // if (GameEngine->GetKey(key).bReleased) fStrokeRepeat = .0f;
    // if (GameEngine->GetKey(key).bHeld) {
    //   fStrokeRepeat += AnyType<-1, float>::GetValue();
    //   if (fStrokeRepeat >= 0.3f) { fStrokeRepeat = .2f; bPressed = true; }
    // }

    // if (!bPressed) return;
    // // TODO:
    // // if (GameEngine->GetKey(olc::Key::CTRL).bHeld) cmd += "^";

    // const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
    // const char c = toUpper ? upper : lower;

    // // TODO:
    // // bUpdated = true; cmd += std::string(1, c); 
  }

private:
  Bus* bus;

  // Variables defines animation duration
  float fStrokeRepeat = 0.f;

  bool bUpdated = false;
};

};
