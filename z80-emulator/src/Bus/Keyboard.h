#pragma once
#include "InterruptVector.h"

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
    const auto mouse = GameEngine->GetMousePos();
    const bool bPressed = GameEngine->GetMouse(0).bPressed;

    olc::vi2d pos = absolute - olc::vi2d(1, 1);
    olc::vi2d size = name.second + olc::vi2d(2, 2);

    if (bPressed && mouse.x > pos.x && mouse.y > pos.y && mouse.x < pos.x + size.x && mouse.y < pos.y + size.y) {
      bEnabled = bEnabled ^ true;
    }

    Utils::Lock l(mutex);
    if (bEnabled) foreach<KeyboardScanCodes, Keyboard>::Process(this);
  }

  void Draw(PixelGameEngine* GameEngine) {
    olc::vi2d pos = absolute;

    auto fg = bEnabled ? AnyType<DARK_GREY, ColorT>::GetValue() : AnyType<BLACK, ColorT>::GetValue();
    auto bg = bEnabled ? AnyType<BLACK, ColorT>::GetValue()     : AnyType<DARK_GREY, ColorT>::GetValue();

    GameEngine->FillRect(pos - olc::vi2d(1, 1), name.second + olc::vi2d(2, 2), *bg);
    GameEngine->DrawString(pos, name.first, *fg);

    int32_t index = 0; Utils::Lock l(mutex);
    const olc::vi2d vOffset = olc::vi2d(name.second.x, 0) + vStep;

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
      if (!bEnabled || !bHandled) continue;

      Utils::Lock l(mutex);
      if (buffer.size()) { bHandled = false; Interrupt(); }
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

    // TODO: Fix a bug with caps-lock
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
  std::atomic<bool> bEnabled = false;
  std::atomic<bool> bHandled = true;

  std::unique_ptr<std::thread> runtime = nullptr;
  std::pair<const char*, olc::vi2d> name = std::pair("buf", olc::vi2d(8 * 3, 8));

  // Variables defines animation duration
  std::unordered_map<int32_t, float> fStrokeRepeat;

  std::mutex mutex;
  std::list<uint8_t> buffer;
};
};
