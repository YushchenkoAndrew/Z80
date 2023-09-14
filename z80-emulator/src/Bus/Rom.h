#pragma once
#include "Bus.h"

namespace Bus {

/**
 * EEPROM W27C512 
 *  65536 × 8 bits
 * 
 * Grammar:
 *  command     -> phrase | motion | order
 * 
 *  motion      -> NUMBER? verb? noun
 *  order       -> NUMBER? adverb
 *  noun        -> 'h' | 'j' | 'k' | 'l' | 'w' | 'b' | 'e' | '0' | '$' | 'gg' | 'G' | '/' | '?' | 'n' | 'N' | 'f' | 'F' | ',' | ';'
 *  verb        -> 'c' | 'd' | 'y' 
 *  adverb      -> 'p' | 'x'
 *  phrase      -> 'i' | 'a' | 'r'
 */
class Rom : public Window {
public:
  enum ModeT { NORMAL, REPLACE, CHARACTER };

  void load(std::vector<uint8_t> code) {
    for (auto& bank : memory) bank = 0x00;

    for (uint32_t i = 0; i < code.size(); i++) memory[i] = code[i];
  }

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset;
  }

  void Process(olc::PixelGameEngine* GameEngine) {
    bUpdated = false;

    switch (mode) {
      case NORMAL:    return Process(Int2Type<NORMAL>(), GameEngine);
      case CHARACTER: return Process(Int2Type<NORMAL>(), GameEngine);
      case REPLACE:   return Process(Int2Type<REPLACE>(), GameEngine);
    }
  }


  void Draw(olc::PixelGameEngine* GameEngine) {
    switch (mode) {
      case NORMAL:    return Draw(Int2Type<NORMAL>(), GameEngine);
      case REPLACE:   return Draw(Int2Type<NORMAL>(), GameEngine);
      case CHARACTER: return Draw(Int2Type<CHARACTER>(), GameEngine);
    }
  }

private:
  template<int32_t T>
  void Draw(Int2Type<T>, olc::PixelGameEngine* GameEngine) {}

  void Draw(Int2Type<NORMAL>, olc::PixelGameEngine* GameEngine) {
    for (int32_t i = 0; i < 0x10; i++) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * vStep + olc::vi2d(vOffset.x, 0);
      GameEngine->DrawString(pos, Int2Hex(i), AnyType<DARK_GREY, olc::Pixel>::GetValue());
    }

    for (int32_t i = 0; i < 0x10; i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * vStep + olc::vi2d(0, vOffset.y);
      GameEngine->DrawString(pos, Int2Hex(i, 5), AnyType<DARK_GREY, olc::Pixel>::GetValue());
      
      for (int32_t j = 0; j < 0x10; j++) {
        olc::vi2d pos = absolute + olc::vi2d(j, i) * vStep + vOffset;
        GameEngine->DrawString(pos, Int2Hex(memory[(i << 4) | j]), AnyType<WHITE, olc::Pixel>::GetValue());
      }
    }

    olc::vi2d cursor = absolute + pos * vStep + vOffset;
    GameEngine->FillRect(cursor - olc::vi2d(1, 2), { 18, 12 }, AnyType<GREY, olc::Pixel>::GetValue());
    GameEngine->DrawString(cursor, Int2Hex(memory[index()]), AnyType<BLACK, olc::Pixel>::GetValue());
  }

  void Draw(Int2Type<CHARACTER>, olc::PixelGameEngine* GameEngine) {
    for (int32_t i = 0; i < 0x10; i++) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * olc::vi2d(vStep.y, vStep.y) + olc::vi2d(vOffset.x, 0);
      GameEngine->DrawString(pos, Int2Hex(i, 1), AnyType<DARK_GREY, olc::Pixel>::GetValue());
    }

    for (int32_t i = 0; i < 0x10; i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * olc::vi2d(vStep.y, vStep.y) + olc::vi2d(0, vOffset.y);
      GameEngine->DrawString(pos, Int2Hex(i, 5), AnyType<DARK_GREY, olc::Pixel>::GetValue());
      
      for (int32_t j = 0; j < 0x10; j++) {
        olc::vi2d pos = absolute + olc::vi2d(j, i) * olc::vi2d(vStep.y, vStep.y) + vOffset;
        GameEngine->DrawString(pos, isprint(memory[(i << 4) | j]) ? std::string(1, memory[(i << 4) | j]) : ".", AnyType<WHITE, olc::Pixel>::GetValue());
      }
    }

    olc::vi2d cursor = absolute + pos * olc::vi2d(vStep.y, vStep.y) + vOffset;
    GameEngine->FillRect(cursor - olc::vi2d(1, 2), { 12, 12 }, AnyType<GREY, olc::Pixel>::GetValue());
    GameEngine->DrawString(cursor, isprint(memory[index()]) ? std::string(1, memory[index()]) : ".", AnyType<BLACK, olc::Pixel>::GetValue());
  }


  // // phrase

  // // noun
  //  CMD_w, CMD_b,  CMD_e,  CMD_G, CMD_n, CMD_N, CMD_f, CMD_F,
  // CMD_DOLLAR, CMD_SLASH, CMD_QUESTION, CMD_COMMA, CMD_SEMICOLON,

  // // verb
  // CMD_c, CMD_d, CMD_y,

  // // adverb
  // CMD_p, CMD_P, 
public:
  template<int32_t T>
  inline void Command(Int2Type<T>) {}

  inline void Command(Int2Type<Editor::VimT::CMD_C>) { mode = mode == NORMAL ? CHARACTER : NORMAL; }
  inline void Command(Int2Type<Editor::VimT::CMD_r>) { mode = REPLACE; Command(Int2Type<Editor::VimT::CMD_x>()); }
  inline void Command(Int2Type<Editor::VimT::CMD_0>) { pos.x = 0; }
  inline void Command(Int2Type<Editor::VimT::CMD_DOLLAR>) { pos.x = 0x0F; }
  inline void Command(Int2Type<Editor::VimT::CMD_gg>) { pos.y = pos.x = 0; }
  inline void Command(Int2Type<Editor::VimT::CMD_x>) { memory[index()] = 0x00; }

  inline void Command(Int2Type<Editor::VimT::CMD_j>) {
    if (pos.y < (int32_t)memory.size() / 0x10) pos.y++;
  }

  inline void Command(Int2Type<Editor::VimT::CMD_k>) {
    if (pos.y > 0x00) pos.y--;
  }

  inline void Command(Int2Type<Editor::VimT::CMD_l>) {
    if (pos.x < 0x0F) pos.x++;
  }

  inline void Command(Int2Type<Editor::VimT::CMD_h>) {
    if (pos.x > 0x00) pos.x--;
  }


private:
  template<int32_t T>
  void Process(Int2Type<T>, olc::PixelGameEngine* GameEngine) { }

  void Process(Int2Type<REPLACE>, olc::PixelGameEngine* GameEngine) {
    AnyType<-1, olc::PixelGameEngine*>::GetValue() = GameEngine;
    foreach<Editor::KeyEvent, Rom>::Process(this);
  }

  void Process(Int2Type<NORMAL>, olc::PixelGameEngine* GameEngine) {
    AnyType<-1, olc::PixelGameEngine*>::GetValue() = GameEngine;
    foreach<Editor::KeyEvent, Rom>::Process(this);

    if (!bUpdated) return;
    else bUpdated = false;
    
    printf("%s\n", cmd.c_str());

    if (search.first) {
      // TODO: ADD ability to edit this string
      std::get<0>(search.second).push_back(cmd.back());
      // if (std::get<0>(search.second).size() == std::get<1>(search.second)) 
      return reset(); 
    }

    if (nCurr == 0) {
      if (match<1>({ 'C' })) { phrase(peekPrev()); return reset(); };
      if (isDigit(peek()) && peek() != '0') { nCurr++; return; }

    } else {
      uint8_t mask = ((uint8_t)isDigit(peekPrev()) << 1) + (uint8_t)isDigit(peek());

      switch(mask) {
        case 0b11: nCurr++; return;
        case 0b01: reset(false); return;
      }
    }


    if (match<21>({ 'h', 'j', 'k', 'l', 'x', 'r', 'w', 'b', 'e', '0', '$', 'G', '/', '?', 'n', 'N', 'f', 'F', 'r', ',', ';' })) {
      switch (peekPrev()) {
        case 'r': search = { false, { "", 2, true, {} } }; break;
        case 'g': search = { true, { "", 1, true, {} } }; break;
        case 'f': case 'F': search = { true, { "", 1, true, {} } }; break;
        case '/': case '?': search = { true, { "", -1, true, {} } }; break;
      }

      // (noun * number) + verb
      phrase(peekPrev()); number(); verb(peek(nCurr - 2)); 

      return reset();
    }

    // adverb
    if (match<2>({ 'p', 'P' })) { phrase(peekPrev()); number(); return reset(); }
    if (peekPrev() == 'g' && match<1>({ 'g' })) {  phrase(Int2Type<Editor::VimT::CMD_gg>()); number();  verb(peek(nCurr - 3)); return reset(); } 

    // verb
    if (match<4>({ 'c', 'd', 'y', 'g' })) return;

    reset(false);
  }

  inline void verb(const char c) {
    switch (c) {
      case 'c': case 'd': case 'y': return phrase(c);
    }
  }

  inline void phrase(const char c) {
    AnyType<-1, int32_t>::GetValue() = c;
    // bSync = bSync || foreach<SyncRomCommands, AnyType<-1, int32_t>>::Has();
    bSync = bSync || true;

    auto operation = lambda;
    lambda = [=]() { operation(); AnyType<-1, int32_t>::GetValue() = c; foreach<RomCommands, Rom>::Command(this); };
  }

  template<int32_t T>
  inline void phrase(Int2Type<T> val) {
    // TODO: add foreach support
    // bSync = foreach<VimCommands, AnyType<-1, Vim*>>::Has();
    bSync = bSync || true;

    auto operation = lambda;
    lambda = [=]() { operation(); Command(val); };
  }

  inline void reset(bool exec = true) {
    // if (search.first && std::get<0>(search.second).size() != std::get<1>(search.second)) return;

    if (exec) {
      AnyType<-1, olc::vi2d>::GetValue() = pos;
      if (bSync) { lambda(); bUpdated = true; }
      else { std::thread t(lambda); t.detach(); }
    }

    if (search.first && std::get<0>(search.second).size() != std::get<1>(search.second)) return;
    cmd.clear(); nStart = nCurr = 0; lambda = []() {}; search.first = false; bSync = false;
  }

public:

  template<int32_t T, int32_t U>
  void Process(TypeList<Int2Type<T>, Int2Type<U>>) {
    const char c = static_cast<char>(Int2Type<U>().value);
    BasicStrokeHandler(static_cast<olc::Key>(Int2Type<T>().value), c, toupper(c));
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

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::UP>,    Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::UP).bPressed) Command(Int2Type<Editor::VimT::CMD_k>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::DOWN>,  Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::DOWN).bPressed) Command(Int2Type<Editor::VimT::CMD_j>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::LEFT>,  Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::LEFT).bPressed) Command(Int2Type<Editor::VimT::CMD_h>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::RIGHT>, Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::RIGHT).bPressed) Command(Int2Type<Editor::VimT::CMD_l>()); }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::DEL).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case NORMAL: case CHARACTER: reset(false); break;
      case REPLACE: memory[index()] = 0x00; std::get<0>(search.second) = ""; break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::BACK).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case NORMAL: case CHARACTER: reset(false); break;
      case REPLACE: 
        memory[index()] = (memory[index()] >> 4) & 0xF0;
        if (std::get<0>(search.second).size()) std::get<0>(search.second).pop_back();
        break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::ENTER).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case NORMAL: 
      case CHARACTER:
        if (search.first) {
          std::get<1>(search.second) = std::get<0>(search.second).size();
          pos = std::get<3>(search.second);
        }

        reset(search.first);
        break;

      case REPLACE: mode = NORMAL; break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::ESCAPE).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case CHARACTER:
      case NORMAL: search.first = false; reset(false);
      case REPLACE: mode = NORMAL; break;
    }
  }


private:
  inline void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    bool bPressed = GameEngine->GetKey(key).bPressed;

    if (GameEngine->GetKey(key).bReleased) fStrokeRepeat = .0f;
    if (GameEngine->GetKey(key).bHeld) {
      fStrokeRepeat += AnyType<-1, float>::GetValue();
      if (fStrokeRepeat >= 0.3f) { fStrokeRepeat = .2f; bPressed = true; }
    }

    if (!bPressed) return;
    const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
    const char c = toUpper ? upper : lower;

    bUpdated = true;

    switch (mode) {
      case NORMAL: case CHARACTER: cmd += std::string(1, c); break;
      case REPLACE: {
        if (!isHex(c)) { memory[index()] = c & 0xFF; mode = NORMAL; break; }

        memory[index()] = ((memory[index()] & 0x0F) << 4) | Hex2Int(c);

        std::get<0>(search.second).push_back(c);
        if (std::get<0>(search.second).size() == std::get<1>(search.second)) mode = NORMAL;

        break;
      }
    }
  }

  template<int32_t T = Editor::VimT::CMD_NONE>
  void number(Int2Type<T> val = Int2Type<T>()) {
    int32_t nCurr = 0; while (isDigit(peek(nCurr))) nCurr++;
    if (nCurr == 0) return;

    auto len = std::stoul(cmd.substr(0, nCurr)); if (len == 0) return;

    auto prev = lambda;
    lambda = [=]() { for (int32_t i = 0; i < len; i++) { if (i) Command(val); prev(); } };
  }

  inline const char peek() { return cmd[nCurr]; }
  inline const char peek(int32_t nCurr) { return cmd[nCurr]; }
  inline const char peekPrev() { return nCurr == 0 ? '\0' : cmd[nCurr - 1]; }
  inline bool check(const char c) { return peek() == c; }
  inline bool isDigit(const char &c) { return c >= '0' && c <= '9'; }
  inline bool isAlpha(const char &c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
  inline bool isHex(const char &c) { return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }

  inline int32_t index() { return (pos.y << 4) + pos.x; }

  inline uint8_t Hex2Int(const char c) { 
    if (isDigit(c)) return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 0x0A;
    if (c >= 'A' && c <= 'F') return c - 'A' + 0x0A;
    return 0;
  }

  inline std::string Int2Hex(uint8_t i, int32_t width = 2) { 
    std::stringstream ss; ss << std::setfill('0') << std::setw(width) << std::hex << std::uppercase << +i;
    return ss.str();
  }


  template<int32_t T>
  bool match(std::array<const char, T> str) {
    for (auto& c : str) {
      if (!check(c)) continue;
      nCurr++; return true;
    }

    return false;
  }


private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(20, 12);
  const olc::vi2d vOffset = olc::vi2d(44, 12);

  ModeT mode = NORMAL;

  int32_t nStart = 0; // index of the cmd, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the cmd, which is pointing to the curr char
  
  std::string cmd;
  std::pair<std::vector<std::string>, bool> buffer = { { "" }, false }; // second value is responsible to distinguish if buffer has a whole line or just a part of it
  std::pair<bool, std::tuple<std::string, int32_t, bool, olc::vi2d>> search = { false, { "", 0, false, {} } }; // if first is true, then require one more "clock" to save after coming char
  
  bool bSync = false; // Flag that define is lambda func is sync or async
  std::function<void(void)> lambda = []() {};

  // Variables defines animation duration
  float fStrokeRepeat = 0.f;

  olc::vi2d pos = olc::vi2d(0, 0);

  bool bUpdated = false;
  std::array<uint8_t, 65536> memory;

};

};
