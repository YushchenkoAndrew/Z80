#pragma once

#include "lib/olcPixelGameEngine.h"
#include "src/Defs.h"
#include "src/Window/Base.h"
#include "src/Window/Event/StrokeAction.h"
#include <utility>

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


class KeyboardEvent : virtual public Base {
public:
  KeyboardEvent(PixelGameEngine* engine): Base(engine) {}

  void Focus(bool state) override { 
    if (state) GameEngine->Event(Int2Type<KEYBOARD_EVENT_SUBSCRIBE>(), this);
    else GameEngine->Event(Int2Type<KEYBOARD_EVENT_UNSUBSCRIBE>(), this);
  }

  std::pair<bool, StrokeCallback> Executable(const std::vector<olc::Key>& keys) {
    auto result = Executable(&bindings, keys);
    if (result.first) return result;

    return Executable(&GetCommands(), keys);
  }

  std::vector<std::string> Combinations(const StrokeT<>* node) {
    if (node == nullptr) return {};
    std::vector<std::string> keys;

    for (const auto& [key, child] : node->children) {
      auto code = std::string(GetKeycode(key, true));
      // for (auto& c : code) c = std::toupper(c);

      if (child.func != nullptr) { keys.push_back(code); keys.push_back("<key>"); continue; }
      for (const auto& combination : Combinations(&child)) {
        keys.push_back(code + " " + combination);
      }
    }

    return keys;
  }

  static const char* GetKeycode(olc::Key key, bool upper = false) {
    auto keycode = keycodes.at(key);
    return upper ? keycode.second : keycode.first;
  };

protected:
  inline void reset(bool exec = true) {
    if (exec) lambda();
    cmd = ""; nStart = nCurr = 0; lambda = []() {}; 
  }

  template <typename T>
  inline std::pair<bool, StrokeCallback> Executable(T* node, const std::vector<olc::Key>& keys) {
    for (auto& key : keys) {
      auto item = node->children.find(key);
      if (item != node->children.end()) node = &item->second;
      else return std::pair(false, nullptr);
    }

    if (node->func == nullptr) return std::pair(true, nullptr);
    return std::pair(true, node->func);
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

  inline const char peek() { return cmd.at(nCurr); }
  inline const char peek(int32_t nCurr) { return nCurr >= 0  && nCurr < cmd.size() ? cmd.at(nCurr) : '\0'; }
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

  virtual const StrokeT<>& GetCommands() const {
    static const StrokeT<> empty = ::Window::StrokeT<>::Init({});
    return empty;
  };

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

  StrokeT<> bindings = ::Window::StrokeT<>::Init({});

private:
  static inline const std::unordered_map<olc::Key, std::pair<const char*, const char*>> keycodes = {
    { olc::Key::SPACE, { "SPACE",  "SPACE" } }, { olc::Key::TAB,   { "TAB",    "TAB"   } }, { olc::Key::SHIFT, { "SHIFT",  "SHIFT" } },
    { olc::Key::CTRL,  { "CTRL",   "CTRL"  } }, { olc::Key::INS,   { "INS",    "INS"   } }, { olc::Key::DEL,   { "DEL",    "DEL"   } },
    { olc::Key::HOME,  { "HOME",   "HOME"  } }, { olc::Key::END,   { "END",    "END"   } }, { olc::Key::PGUP,  { "PGUP",   "PGUP"  } },
    { olc::Key::PGDN,  { "PGDN",   "PGDN"  } }, { olc::Key::BACK,  { "BACK",   "BACK"  } }, { olc::Key::ESCAPE,{ "ESCAPE", "ESCAPE"} },
    { olc::Key::RETURN,{ "RETURN", "RETURN"} }, { olc::Key::ENTER, { "ENTER",  "ENTER" } }, { olc::Key::PAUSE, { "PAUSE",  "PAUSE" } },
    { olc::Key::SCROLL,{ "SCOLL",  "SCOLL" } }, { olc::Key::OEM_1, { ";",     ":"      } }, { olc::Key::OEM_2, { "/",      "?"     } },
    { olc::Key::OEM_3, { "`",      "~"     } }, { olc::Key::OEM_4, { "[",     "{"      } }, { olc::Key::OEM_5, { "\\",     "|"     } },
    { olc::Key::OEM_6, { "]",      "}"     } }, { olc::Key::OEM_7, { "'",     "\""     } }, { olc::Key::OEM_8, { "#",      "#"     } },
    { olc::Key::COMMA, { ",",      "<"     } }, { olc::Key::PERIOD,{ ".",     ">"      } }, { olc::Key::MINUS, { "-",      "_"     } },
    { olc::Key::UP,    { "UP",     "UP"    } }, { olc::Key::DOWN,  { "DOWN",  "DOWN"   } }, { olc::Key::RIGHT, { "RIGHT",  "RIGHT" } },
    { olc::Key::LEFT,  { "LEFT",   "LEFT"  } }, { olc::Key::NP_MUL,{ "*",     "*"      } }, { olc::Key::NP_DIV,{ "/",      "/"     } },
    { olc::Key::NP_ADD,{ "+",      "+"     } }, { olc::Key::NP_SUB,{ "-",     "-"      } }, { olc::Key::EQUALS,{ "=",      "+"     } },

    { olc::Key::NP0,   { "0",      ")"     } }, { olc::Key::NP1,   { "1",     "!"      } }, { olc::Key::NP2,   { "2",      "@"     } },
    { olc::Key::NP3,   { "3",      "#"     } }, { olc::Key::NP4,   { "4",     "$"      } }, { olc::Key::NP5,   { "5",      "%"     } },
    { olc::Key::NP6,   { "6",      "^"     } }, { olc::Key::NP7,   { "7",     "&"      } }, { olc::Key::NP8,   { "8",      "*"     } },
    { olc::Key::NP9,   { "9",      "("     } },

    { olc::Key::K0,    { "0",      ")"     } }, { olc::Key::K1,    { "1",     "!"      } }, { olc::Key::K2,    { "2",      "@"     } },
    { olc::Key::K3,    { "3",      "#"     } }, { olc::Key::K4,    { "4",     "$"      } }, { olc::Key::K5,    { "5",      "%"     } },
    { olc::Key::K6,    { "6",      "^"     } }, { olc::Key::K7,    { "7",     "&"      } }, { olc::Key::K8,    { "8",      "*"     } },
    { olc::Key::K9,    { "9",      "("     } },

    { olc::Key::A,     { "a",      "A"     } }, { olc::Key::B,     { "b",     "B"      } }, { olc::Key::C,     { "c",      "C"     } },
    { olc::Key::D,     { "d",      "D"     } }, { olc::Key::E,     { "e",     "E"      } }, { olc::Key::F,     { "f",      "F"     } },
    { olc::Key::G,     { "g",      "G"     } }, { olc::Key::H,     { "h",     "H"      } }, { olc::Key::I,     { "i",      "I"     } },
    { olc::Key::J,     { "j",      "J"     } }, { olc::Key::K,     { "k",     "K"      } }, { olc::Key::L,     { "l",      "L"     } },
    { olc::Key::M,     { "m",      "M"     } }, { olc::Key::N,     { "n",     "N"      } }, { olc::Key::O,     { "o",      "O"     } },
    { olc::Key::P,     { "p",      "P"     } }, { olc::Key::Q,     { "q",     "Q"      } }, { olc::Key::R,     { "r",      "R"     } },
    { olc::Key::S,     { "s",      "S"     } }, { olc::Key::T,     { "t",     "T"      } }, { olc::Key::U,     { "u",      "U"     } },
    { olc::Key::V,     { "v",      "V"     } }, { olc::Key::W,     { "w",     "W"      } }, { olc::Key::X,     { "x",      "X"     } },
    { olc::Key::Y,     { "y",      "Y"     } }, { olc::Key::Z,     { "z",     "Z"      } },
  };
};
};