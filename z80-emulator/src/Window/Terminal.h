#pragma once
#include "Command.h"

namespace Window {

class Terminal : public Window, public Command {
public:
  inline void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  }

  void Process(PixelGameEngine* GameEngine) {
    if ((fBlink += AnyType<-1, float>::GetValue()) > 1.2f) fBlink -= 1.2f;

    bUpdated = false;

    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<KeyEvent, Terminal>::Process(this);

    if (!bUpdated) return;
    else bUpdated = false;

    // TODO: Impl basic terminal aka simple command run (such as :w, :wa :qa ...)
    std::cout << cmd << std::endl;
  }

  void Draw(PixelGameEngine* GameEngine) {
    for (int32_t i = 0, offset = 0; i + offset <= size.y / vStep.y && i < lines.size(); i++, offset--) {
      if (!lines[i].size()) { offset++; continue; }

      for (int32_t j = 0; j < lines[i].size(); j += size.x / vStep.x, offset++) {
        if (i + offset > size.y / vStep.y) break;

        auto pos = olc::vi2d(absolute.x, absolute.y + (i + offset) * vStep.y) + vOffset;
        GameEngine->DrawString(pos, lines[i].substr(j, size.x / vStep.x), *AnyType<GREY, ColorT>::GetValue());
      }
    }

    auto pos = absolute + olc::vi2d(0, (int32_t)lines.size()) * vStep + vOffset;
    GameEngine->DrawString(pos, cmd, *AnyType<GREY, ColorT>::GetValue());

    if (fBlink > 0.6f) return;
    pos = absolute + olc::vi2d((int32_t)cmd.size(), (int32_t)lines.size()) * vStep + vOffset;
    GameEngine->FillRect(pos, olc::vi2d(8, 10), *AnyType<GREY, ColorT>::GetValue());
  }

  template<int32_t T, int32_t U>
  void Process(TypeList<Int2Type<T>, Int2Type<U>>) {
    const char c = static_cast<char>(+U);
    BasicStrokeHandler(static_cast<olc::Key>(+T), c, toupper(c));
  }

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_1>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_1, ';',  ':'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_2>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_2, '/',  '?'); }
  // template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_3>, Int2Type<U>>) { /** // FIXME: Strange bag BasicStrokeHandler(olc::Key::OEM_3, '\'',  '~'); */ }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_4>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_4, '[',  '{'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_5>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_5, '\\', '|'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_6>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_6, ']',  '}'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_7>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_7, '\'', '"'); }

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K1>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K1, '1', '!'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K2>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K2, '2', '@'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K3>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K3, '3', '#'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K4>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K4, '4', '$'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K5>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K5, '5', '%'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K6>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K6, '6', '^'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K7>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K7, '7', '&'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K8>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K8, '8', '*'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K9>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K9, '9', '('); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::K0>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::K0, '0', ')'); }

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::EQUALS>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::EQUALS, '=', '+'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::PERIOD>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::PERIOD, '.', '>'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::COMMA>,  Int2Type<U>>) { BasicStrokeHandler(olc::Key::COMMA,  ',', '<'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::MINUS>,  Int2Type<U>>) { BasicStrokeHandler(olc::Key::MINUS,  '-', '_'); }

  // TODO:
  // template<int32_t U> void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>)    { EscapeStrokeHandler(olc::Key::DEL); }
  // template<int32_t U> void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>)   { EscapeStrokeHandler(olc::Key::BACK); }
  // template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>)  { EscapeStrokeHandler(olc::Key::ENTER); }
  // template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) { EscapeStrokeHandler(olc::Key::ENTER); }

public:
  void PushOut(std::string line) { lines.push_back(line); }
  void PushOut(std::vector<std::string>& l) { lines.clear(); lines.insert(lines.end(), l.begin(), l.end()); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  float fBlink = 0.f;
  std::vector<std::string> lines;

  const olc::vi2d vOffset = olc::vi2d(8, 4);
  const olc::vi2d vStep = olc::vi2d(8, 12);
};
};