#pragma once
#include "Defs.h"

namespace Editor {

class Cursor {
public:
  void Process(olc::PixelGameEngine* GameEngine) {
    if (AnyType<-1, ModeT>::GetValue() != ModeT::NORMAL) return;

    auto KeyK = GameEngine->GetKey(olc::Key::K); auto KeyH = GameEngine->GetKey(olc::Key::H);
    auto KeyJ = GameEngine->GetKey(olc::Key::J); auto KeyL = GameEngine->GetKey(olc::Key::L);

    if (KeyK.bHeld) AutoStroke(KeyK);
    if (KeyJ.bHeld) AutoStroke(KeyJ);
    if (KeyH.bHeld) AutoStroke(KeyH);
    if (KeyL.bHeld) AutoStroke(KeyL);

    if (KeyK.bPressed && pos.y - 1 > 0) MoveVertical(-1);
    if (KeyJ.bPressed && pos.y + 1 < lines.size()) MoveVertical(1);

    if (KeyH.bPressed && pos.x - 1 > 0) MoveHorizontal(-1);
    if (KeyL.bPressed && pos.x < lines[pos.y - 1]) MoveHorizontal(1);

    if (KeyK.bReleased || KeyJ.bReleased || KeyK.bReleased || KeyL.bReleased) fStrokeRepeat = 0.f;
  }

  void Draw(olc::PixelGameEngine* GameEngine, std::function<olc::vi2d(olc::vi2d pos)> lambda) {
    if ((fBlink += AnyType<-1, float>::GetValue()) > 1.2f) fBlink -= 1.2f;
    if (fBlink > 0.6f) return;

    auto pos = lambda(this->pos) - olc::vi2d(1, 2);
    auto size = AnyType<-1, ModeT>::GetValue() == ModeT::NORMAL ? olc::vi2d(8, 10) : olc::vi2d(2, 10);

    GameEngine->FillRect(pos, size, AnyType<WHITE, olc::Pixel>::GetValue());
  }

private:
  inline void MoveVertical(int32_t y) {
    pos.x = nLastX; nLastX = (pos += olc::vi2d(0, y)).x;

    if (pos.x > lines[pos.y - 1]) pos.x = lines[pos.y - 1];
    fBlink = 0.f;
  }

  inline void MoveHorizontal(int32_t x) {
    nLastX = (pos += olc::vi2d(x, 0)).x;
    fBlink = 0.f;
  }

  inline void AutoStroke(olc::HWButton& key) {
    fStrokeRepeat += AnyType<-1, float>::GetValue();

    if (fStrokeRepeat < 0.15f) return;
    fStrokeRepeat = .0f; key.bPressed = true;
  }

private:
  int32_t nLastX = 1;
  float fBlink = 0.f;
  float fStrokeRepeat = 0.f;

public:
  std::vector<int32_t> lines;
  olc::vi2d pos = olc::vi2d(1, 1);
};
};