#pragma once
#include "Window.h"


namespace Window {

class Command {
protected:
  inline void reset(bool exec = true) {
    if (exec) lambda();
    cmd = ""; nStart = nCurr = 0; lambda = []() {}; 
  }

  virtual void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(key).bPressed) return;

    if (GameEngine->GetKey(olc::Key::CTRL).bHeld) cmd += "^";

    const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
    const char c = toUpper ? upper : lower;

    bUpdated = true; cmd += std::string(1, c); 
  }

  inline const char peek() { return cmd[nCurr]; }
  inline const char peek(int32_t nCurr) { return cmd[nCurr]; }
  inline const char peekPrev() { return nCurr == 0 ? '\0' : cmd[nCurr - 1]; }
  inline bool check(const char c) { return peek() == c; }
  inline bool isDigit(const char &c) { return c >= '0' && c <= '9'; }
  inline bool isAlpha(const char &c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
  inline bool isHex(const char &c) { return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
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
    std::stringstream ss; 
    ss << "Error at '" << c << "': " << message << "\n";
    reset(false); err = ss.str();
  }

  void error(std::string message) {
    std::stringstream ss; ss << "Error " << message << "\n"; err = ss.str();
  }

protected:
  int32_t nStart = 0; // index of the cmd, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the cmd, which is pointing to the curr char

  std::string err;
  std::string cmd = "";
  std::pair<bool, std::tuple<std::string, int32_t, bool, olc::vi2d>> search = { false, { "", 0, false, {} } }; // if first is true, then require one more "clock" to save after coming char

  bool bSync = false; // Flag that define is lambda func is sync or async
  std::function<void(void)> lambda = []() {};

public:
  bool bUpdated = false;
};
};