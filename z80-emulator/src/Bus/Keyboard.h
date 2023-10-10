#pragma once
#include "Ppi.h"

namespace Bus {
class Keyboard : public Window::Window, public Device {
public:
  Keyboard(Bus* b): bus(b) {}
  ~Keyboard() {
    bExec = false; 

    if (runtime != nullptr && runtime->joinable()) runtime->join();
  }

  void Preinitialize() { 
    if (runtime == nullptr) runtime = std::make_unique<std::thread>(std::thread(&Keyboard::Runtime, this));
  }

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second;
  }

  void Process(PixelGameEngine* GameEngine) {
    Utils::Lock l(mutex);

    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<KeyboardScanCodes, Keyboard>::Process(this);
  }

  void Draw(PixelGameEngine* GameEngine) {
    olc::vi2d pos = absolute; const auto name = "buf";

    const olc::vi2d vOffset = olc::vi2d(GameEngine->GetTextSize(name).x, 0) + vStep;
    GameEngine->DrawString(pos, name, *AnyType<DARK_GREY, ColorT>::GetValue());

    int32_t index = 0; Utils::Lock l(mutex);
    for (auto& val : buffer) {
      GameEngine->DrawString(pos + vOffset, Utils::Int2Hex(val), *AnyType<GREY, ColorT>::GetValue());

      if (++index > 3) break;
      else pos.x += vStep.x * 3;
    }
  }

  void Interrupt();

private:
  void Runtime() {
    while (bExec) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      if (bHandled) {
        Utils::Lock l(mutex);

        if (buffer.size()) { bHandled = false; Interrupt(); }
      }
    }
  }

public:
  uint8_t Read(uint32_t addr, bool) {
    Utils::Lock l(mutex); bHandled = true;

    uint8_t code = buffer.size() ? buffer.front() : '\0';
    if (buffer.size()) buffer.pop_front();

    return code;
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) { return 0x00; }

public:
  template<int32_t T, int32_t U>
  void Process(TypeList<Int2Type<T>, Int2Type<U>>) {
    const olc::Key key = static_cast<olc::Key>(+T);

    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    bool bPressed = GameEngine->GetKey(key).bPressed;

    if (GameEngine->GetKey(key).bPressed) fStrokeRepeat[+T] = .0f;
    if (GameEngine->GetKey(key).bReleased) { fStrokeRepeat[+T] = .0f; buffer.push_back(0xF0); }
    if (GameEngine->GetKey(key).bHeld) {
      fStrokeRepeat[+T] += AnyType<-1, float>::GetValue();
      if (fStrokeRepeat[+T] >= 0.3f) { fStrokeRepeat[+T] = .2f; bPressed = true; }
    }

    if (!bPressed && !GameEngine->GetKey(key).bReleased) return;
    buffer.push_back(static_cast<uint8_t>(+U));
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 0);

  Bus* bus;

  std::atomic<bool> bExec = true;
  std::atomic<bool> bHandled = true;

  std::unique_ptr<std::thread> runtime = nullptr;

  // Variables defines animation duration
  std::unordered_map<int32_t, float> fStrokeRepeat;

  std::mutex mutex;
  std::list<uint8_t> buffer;
};
};
