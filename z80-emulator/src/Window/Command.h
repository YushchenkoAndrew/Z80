#pragma once
#include "Window.h"


namespace Window {

struct SearchT {
  SearchT(bool e, int32_t start, int32_t size, bool d, olc::vi2d p):
    bEnabled(e), nStartAt(start), nSize(size), bDirection(d), vPos(p), vPrev(p), sPhrase("") {}

  bool bEnabled;
  bool bDirection;
  int32_t nStartAt;
  std::string sPhrase;
  int32_t nSize;
  olc::vi2d vPos;
  olc::vi2d vPrev;
};

class Command {
protected:
  inline void reset(bool exec = true) {
    if (exec) lambda();
    cmd = ""; nStart = nCurr = 0; lambda = []() {}; 
  }

  virtual void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    bool bPressed = GameEngine->GetKey(key).bPressed;

    if (GameEngine->GetKey(key).bReleased) { fStrokeRepeat = .0f; bReleased = true; }
    if (GameEngine->GetKey(key).bHeld) {
      fStrokeRepeat += AnyType<-1, float>::GetValue();
      if (fStrokeRepeat >= 0.3f) { fStrokeRepeat = .2f; bPressed = true; }
    }

    if (!bPressed) return;
    if (GameEngine->GetKey(olc::Key::CTRL).bHeld) cmd += "^";

    const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
    const char c = toUpper ? upper : lower;

    bUpdated = true; cmd += std::string(1, c); 
  }

  inline const char peek() { return cmd[nCurr]; }
  inline const char peek(int32_t nCurr) { return cmd[nCurr]; }
  inline const char peekPrev() { return nCurr == 0 ? '\0' : cmd[nCurr - 1]; }
  inline bool check(const char c) { return peek() == c; }
  inline int32_t digit() { return peek() - '0'; }

  template<int32_t T>
  bool match(std::array<const char, T> str) {
    for (auto& c : str) {
      if (!check(c)) continue;
      nCurr++; return true;
    }

    return false;
  }

  void error(const char c, std::string message) {
    reset(false); err = "Error at '" + std::to_string(c) + "': " + message + "\n";
  }

  void error(std::string message) { err = "Error " + message + "\n"; }

protected:
  int32_t nStart = 0; // index of the cmd, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the cmd, which is pointing to the curr char

  std::string err;
  std::string cmd = "";
  SearchT search = SearchT(false, 0, 0 , false, {}); // if first is true, then require one more "clock" to save after coming char
  
  // Variables defines animation duration
  float fStrokeRepeat = 0.f;

  bool bSync = false; // Flag that define is lambda func is sync or async
  std::function<void(void)> lambda = []() {};

public:
  bool bUpdated = false;
  bool bReleased = false;
};
};