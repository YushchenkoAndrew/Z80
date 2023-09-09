#pragma once
#include "Defs.h"

namespace Editor {

/**
 * This code was hardly expired by the next resource
 * Check out this link for more info: https://gist.github.com/countvajhula/0721a5fc40f2124097652071bb9f97fb
 *
 * Grammar:
 *  command     -> phrase | motion | sentence
 * 
 *  motion      -> NUMBER? noun
 *  sentence    -> NUMBER? verb (noun | adverb) | NUMBER? order
 *  adverb      -> 'i'
 *  noun        -> 'h' | 'j' | 'k' | 'l' | 'w' | 'W' | 'b' | 'B' | 'e' | 'E' | '0' | '$' | '^' | '_' | 'gg' | 'G' | '/' | '?' | 'n' | 'N' | 'f' | 'F' | ',' | ';'
 *  verb        -> 'c' | 'd' | 'y' 
 *  order       -> 'dd' | 'cc' | 'yy' | 'p' | 'P'
 *  phrase      -> 'i' | 'I' | 'a' | 'A' | 'o' | 'O'
 */
class Cursor {
public:
  void Process(olc::PixelGameEngine* GameEngine) {
    AnyType<-1, bool>::GetValue() = false;

    switch (AnyType<-1, ModeT>::GetValue()) {
      case NORMAL: return Process(Int2Type<NORMAL>(), GameEngine);
      case INSERT: return Process(Int2Type<INSERT>(), GameEngine);
    }
  }

  void Draw(olc::PixelGameEngine* GameEngine, std::function<olc::vi2d(olc::vi2d pos)> lambda) {
    if ((fBlink += AnyType<-1, float>::GetValue()) > 1.2f) fBlink -= 1.2f;
    if (fBlink > 0.6f) return;

    auto pos = lambda(this->pos + olc::vi2d(1, 1)) - olc::vi2d(1, 2);
    auto size = AnyType<-1, ModeT>::GetValue() == ModeT::NORMAL ? olc::vi2d(8, 10) : olc::vi2d(2, 10);

    GameEngine->FillRect(pos, size, AnyType<WHITE, olc::Pixel>::GetValue());
  }

  void Load(std::vector<std::shared_ptr<Interpreter::Token>>& dst) {
    std::string line = ""; lines.clear();

    for (auto& token : dst) {
      if (token->line == lines.size() + 1) { line += token->lexeme; continue; }
      lines.push_back(line); line = token->lexeme;

      while (token->line != lines.size() + 1) lines.push_back("");
    }
  }

  inline std::string Text() {
    std::string text = "";

    for (int32_t i = 0; i < lines.size(); i++) {
      text += (i ? "\n" : "") + lines[i];
    }

    return text;
  }


  template<int32_t T>
  void Process(Int2Type<T>, olc::PixelGameEngine* GameEngine) { }

  // void Process(Int2Type<NORMAL>, olc::PixelGameEngine* GameEngine) {
  //   auto KeyK = GameEngine->GetKey(olc::Key::K); auto KeyH = GameEngine->GetKey(olc::Key::H);
  //   auto KeyJ = GameEngine->GetKey(olc::Key::J); auto KeyL = GameEngine->GetKey(olc::Key::L);

  //   if (KeyK.bHeld) AutoStroke(KeyK);
  //   if (KeyJ.bHeld) AutoStroke(KeyJ);
  //   if (KeyH.bHeld) AutoStroke(KeyH);
  //   if (KeyL.bHeld) AutoStroke(KeyL);

  //   int32_t lineEndsAt = AnyType<-1, ModeT>::GetValue() == ModeT::NORMAL;

  //   if (KeyK.bPressed && pos.y > 0) MoveVertical(-1);
  //   if (KeyJ.bPressed && pos.y < lines.size()) MoveVertical(1);

  //   if (KeyH.bPressed && pos.x > 0) MoveHorizontal(-1);
  //   if (KeyL.bPressed && pos.x + lineEndsAt < lines[pos.y].size()) MoveHorizontal(1);

  //   if (KeyK.bReleased || KeyJ.bReleased || KeyK.bReleased || KeyL.bReleased) fStrokeRepeat = 0.f;
  // }

  template<int32_t T>
  void Command(Int2Type<T>) { }


  void Process(Int2Type<NORMAL>, olc::PixelGameEngine* GameEngine) {
    AnyType<-1, olc::PixelGameEngine*>::GetValue() = GameEngine;
    AnyType<-1, Cursor*>::GetValue() = this;

    foreach<KeyEvent, AnyType<-1, Cursor*>>::Process();
    if (!AnyType<-1, bool>::GetValue()) return;

    AnyType<-1, bool>::GetValue() = false;

    switch(cmd[nCurr]) {
      case 'i': return Command(Int2Type<VimT::CMD_i>());
      case 'I': return Command(Int2Type<VimT::CMD_I>());
      case 'a': return Command(Int2Type<VimT::CMD_a>());
      case 'A': return Command(Int2Type<VimT::CMD_A>());
      case 'o': return Command(Int2Type<VimT::CMD_o>());
      case 'O': return Command(Int2Type<VimT::CMD_O>());

      default:
        if (isDigit(cmd[nCurr])) nCurr++;
        else {
          nStart = nCurr;

          if (match<24>({ 'h', 'j', 'k', 'l', 'w', 'W', 'b', 'B', 'e', 'E', '0', '$', '^', '_', 'g',  'G', '/', '?', 'n', 'N', 'f', 'F', ',', ';' })) {
          // TODO: + add expected next char, std::vector
          // TODO: noun

//  *  verb        -> 'c' | 'd' | 'y' 
        } else if (match<1>({ 'i' })) {
          // TODO: noun
        } else if (match<5>({ 'd', 'c', 'y', 'p', 'P' })) {

//  *  order       -> 'dd' | 'cc' | 'yy' | 'p' | 'P'
        }
        }
    }
  }

  void noun() {


      // advance();
  }

  void Process(Int2Type<INSERT>, olc::PixelGameEngine* GameEngine) {
    AnyType<-1, olc::PixelGameEngine*>::GetValue() = GameEngine;
    AnyType<-1, Cursor*>::GetValue() = this;

    foreach<KeyEvent, AnyType<-1, Cursor*>>::Process();
  }

  template<int32_t T, int32_t U>
  void Process(TypeList<Int2Type<T>, Int2Type<U>>) {
    const char c = static_cast<char>(Int2Type<U>().value);
    BasicStrokeHandler(static_cast<olc::Key>(Int2Type<T>().value), c, toupper(c));
  }

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_1>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_1, ';',  ':'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_2>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_2, '/',  '?'); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_3>, Int2Type<U>>) { BasicStrokeHandler(olc::Key::OEM_3, '`',  '~'); }
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

  // TODO: Refactor this thing to much better impl
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::UP>,    Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::UP).bPressed) MoveVertical(-1); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::DOWN>,  Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::DOWN).bPressed) MoveVertical(1); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::LEFT>,  Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::LEFT).bPressed) MoveHorizontal(-1); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::RIGHT>, Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::RIGHT).bPressed) MoveHorizontal(1); }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::TAB>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::TAB).bPressed) return;

    AnyType<-1, bool>::GetValue() = true;

    switch (AnyType<-1, ModeT>::GetValue()) {
      case NORMAL: break;
      case INSERT: lines[pos.y].insert(pos.x += 2, "  "); break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::DEL).bPressed) return;

    AnyType<-1, bool>::GetValue() = true;

    switch (AnyType<-1, ModeT>::GetValue()) {
      case NORMAL: break;

      case INSERT: 
        if (pos.x < lines[pos.y].size()) lines[pos.y].erase(pos.x, 1);
        else {
          lines[pos.y].insert(lines[pos.y].end(), lines[pos.y + 1].begin(), lines[pos.y + 1].end());
          lines.erase(lines.begin() + pos.y + 1);
        }
        break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::BACK).bPressed) return;

    AnyType<-1, bool>::GetValue() = true;

    switch (AnyType<-1, ModeT>::GetValue()) {
      case NORMAL: break;

      case INSERT: 
        if (pos.x) lines[pos.y].erase(nLastX = --pos.x, 1);
        else {
          if (!pos.y) return;
          int32_t lineEndsAt = AnyType<-1, ModeT>::GetValue() == ModeT::NORMAL;
          pos.y--; nLastX = pos.x = lines[pos.y].size() - lineEndsAt;

          if (pos.y + 1 < lines.size()) {
            lines[pos.y].insert(lines[pos.y].end(), lines[pos.y + 1].begin(), lines[pos.y + 1].end());
            lines.erase(lines.begin() + pos.y + 1);
          }
        }
        break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::ENTER).bPressed) return;

    AnyType<-1, bool>::GetValue() = true;

    switch (AnyType<-1, ModeT>::GetValue()) {
      case NORMAL: break;

      case INSERT: {
        std::string copy = lines[pos.y].substr(pos.x, lines[pos.y].size() - pos.x);
        lines[pos.y].erase(pos.x, copy.size());

        int32_t startAt = 0; while (lines[pos.y][startAt] == ' ') startAt++;
        lines.insert(lines.begin() + ++pos.y, std::string(startAt, ' ') + copy);

        nLastX = pos.x = startAt;
        break;
      }
    }
  }

private:
  inline void MoveVertical(int32_t y) {
    pos.x = nLastX; nLastX = (pos += olc::vi2d(0, y)).x;
    int32_t lineEndsAt = AnyType<-1, ModeT>::GetValue() == ModeT::NORMAL;

    if (pos.x > lines[pos.y].size()) pos.x = lines[pos.y].size() - lineEndsAt;
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

  inline void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(key).bPressed) return;

    const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
    std::string c(1, toUpper ? upper : lower);

    AnyType<-1, bool>::GetValue() = true;

    switch (AnyType<-1, ModeT>::GetValue()) {
      case NORMAL: cmd += c; break;
      case INSERT: lines[pos.y].insert(pos.x++, c); break;
    }
  }

  // inline bool isAtEnd() { return nCurr >= cmd.size(); }
  // // inline const char advance() { nCol++; return src[nCurr++]; }
  // inline const char peek() { return isAtEnd() ? '\0' : src[nCurr]; }
  // inline const char peekPrev() { return !nCurr ? '\0' : src[nCurr - 1]; }
  // inline const char peekNext() { return nCurr + 1 >= src.size() ? '\0' : src[nCurr + 1]; }
  inline bool check(const char c) { return cmd[nCurr] == c; }
  inline bool isDigit(const char &c) { return c >= '0' && c <= '9'; }

  template<int32_t T>
  bool match(std::array<const char, T> str) {
    for (auto& c : str) {
      if (!check(c)) continue;
      nCurr++; return true;
    }

    return false;
  }

  void number() {
    int32_t nCurr = 0; while (isDigit(cmd[nCurr])) nCurr++;
    if (nCurr == 0) return;

    auto len = std::stoul(cmd.substr(0, nCurr));
    lambda = [&]() { for (int32_t i = 0; i < len; i++) lambda(); };
  }


private:
  int32_t nStart = 0; // index of the cmd, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the cmd, which is pointing to the curr char
  std::function<void(void)> lambda;

  int32_t nLastX = 0;
  float fBlink = 0.f;
  float fStrokeRepeat = 0.f;

  std::string cmd;
  std::vector<std::string> lines;

  olc::vi2d pos = olc::vi2d(0, 0);
};
};