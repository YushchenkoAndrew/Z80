#pragma once
#include "Ppi.h"

namespace Bus {
#define LAST_VAL(v)  std::get<0>(v)
#define LIST(v)      std::get<1>(v)
#define IS_EMPTY(v)  (LIST(v).size() == 0)

#define MUTEX(v)     std::get<2>(v)

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
    Utils::Lock l(MUTEX(buffer));

    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<KeyboardScanCodes, Keyboard>::Process(this);
  }

  void Draw(PixelGameEngine* GameEngine) {
    olc::vi2d pos = absolute; const auto name = "buf";

    const olc::vi2d vOffset = olc::vi2d(GameEngine->GetTextSize(name).x, 0) + vStep;
    GameEngine->DrawString(pos, name, *AnyType<DARK_GREY, ColorT>::GetValue());

    int32_t index = 0; Utils::Lock l(MUTEX(buffer));
    for (auto& val : LIST(buffer)) {
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
      { Utils::Lock l(MUTEX(buffer)); if (!IS_EMPTY(buffer)) Interrupt(); }
    }
  }

public:
  uint8_t Read(uint32_t addr, bool) {
    Utils::Lock l(MUTEX(buffer));

    if (!IS_EMPTY(buffer)) { LAST_VAL(buffer) = LIST(buffer).front(); LIST(buffer).pop_front(); }
    return LAST_VAL(buffer);
  }

  uint8_t Write(uint32_t addr, uint8_t data, bool) { return 0x00; }

public:
  template<int32_t T, int32_t U>
  void Process(TypeList<Int2Type<T>, Int2Type<U>>) {
    const olc::Key key = static_cast<olc::Key>(+T);

    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    bool bPressed = GameEngine->GetKey(key).bPressed;

    if (GameEngine->GetKey(key).bReleased) { fStrokeRepeat = .0f; LIST(buffer).push_back(0xF0); }
    if (GameEngine->GetKey(key).bHeld) {
      fStrokeRepeat += AnyType<-1, float>::GetValue();
      if (fStrokeRepeat >= 0.5f) { fStrokeRepeat = .4f; bPressed = true; }
    }

    if (!bPressed && !GameEngine->GetKey(key).bReleased) return;
    LIST(buffer).push_back(static_cast<uint8_t>(+U));
  }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 0);

  Bus* bus;

  std::atomic<bool> bExec = true;
  std::unique_ptr<std::thread> runtime = nullptr;

  // Variables defines animation duration
  float fStrokeRepeat = 0.f;
  std::tuple<uint8_t, std::list<uint8_t>, std::mutex> buffer;
};

#undef LAST_VAL
#undef LIST
#undef IS_EMPTY
#undef MUTEX

};
