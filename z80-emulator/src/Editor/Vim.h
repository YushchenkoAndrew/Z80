#pragma once
#include "Defs.h"

namespace Editor {

/**
 * This code was hardly expired by the next resource
 * Check out this link for more info: https://gist.github.com/countvajhula/0721a5fc40f2124097652071bb9f97fb
 *
 * TODO: Add 'r' verb
 * Grammar:
 *  command     -> phrase | motion | order
 * 
 *  motion      -> NUMBER? verb? noun
 *  order       -> NUMBER? adverb
 *  noun        -> 'h' | 'j' | 'k' | 'l' | 'w' | 'W' | 'b' | 'B' | 'e' | 'E' | '0' | '$' | '^' | '_' | 'gg' | 'G' | '/' | '?' | 'n' | 'N' | 'f' | 'F' | ',' | ';'
 *  verb        -> 'c' | 'd' | 'y' 
 *  adverb      -> 'dd' | 'cc' | 'yy' | 'p' | 'P'
 *  phrase      -> 'i' | 'I' | 'a' | 'A' | 'o' | 'O' | 'C' | 'D' | 'R'
 */
class Vim {
public:
  enum ModeT { NORMAL, INSERT, REPLACE };

  void Process(olc::PixelGameEngine* GameEngine) {
    bUpdated = false;

    switch (mode) {
      case NORMAL:  return Process(Int2Type<NORMAL>(), GameEngine);
      case INSERT:  return Process(Int2Type<INSERT>(), GameEngine);
      case REPLACE: return Process(Int2Type<INSERT>(), GameEngine);
    }
  }

  void Draw(olc::PixelGameEngine* GameEngine, std::function<olc::vi2d(olc::vi2d pos)> lambda) {
    if ((fBlink += AnyType<-1, float>::GetValue()) > 1.2f) fBlink -= 1.2f;
    if (fBlink > 0.6f) return;

    auto size = olc::vi2d();
    auto pos = lambda(this->pos + olc::vi2d(1, 1)) - olc::vi2d(1, 2);

    switch (mode) {
      case NORMAL:  size = olc::vi2d(8, 10); break;
      case INSERT:  size = olc::vi2d(2, 10); break;
      case REPLACE: size = olc::vi2d(8,  2); pos += olc::vi2d(0, 10); break;
    }

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
  inline void Command(Int2Type<T>) {}

  // // phrase
  // CMD_r,

  // // noun
  // CMD_b, CMD_B, CMD_e, CMD_E, CMD_gg, CMD_G, CMD_n, CMD_N, CMD_f, CMD_F,
  //  CMD_BACK_SLASH, CMD_QUESTION, CMD_COMMA, CMD_SEMICOLON,

  // // verb
  // CMD_c, CMD_d, CMD_y,

  inline void Command(Int2Type<VimT::CMD_i>) { mode = INSERT; }
  inline void Command(Int2Type<VimT::CMD_R>) { mode = REPLACE; }
  inline void Command(Int2Type<VimT::CMD_I>) { Command(Int2Type<VimT::CMD_i>()); nLastX = pos.x = startAt(); }
  inline void Command(Int2Type<VimT::CMD_a>) { Command(Int2Type<VimT::CMD_i>()); nLastX = pos.x++; }
  inline void Command(Int2Type<VimT::CMD_A>) { Command(Int2Type<VimT::CMD_i>()); nLastX = pos.x = lines[pos.y].size(); }
  inline void Command(Int2Type<VimT::CMD_C>)  { Command(Int2Type<VimT::CMD_D>()); Command(Int2Type<VimT::CMD_A>()); }
  inline void Command(Int2Type<VimT::CMD_cc>) { Command(Int2Type<VimT::CMD_dd>()); Command(Int2Type<VimT::CMD_I>()); }
  inline void Command(Int2Type<VimT::CMD_yy>) { buffer = { lines[pos.y], true }; }
  inline void Command(Int2Type<VimT::CMD_0>) { nLastX = pos.x = 0; }
  inline void Command(Int2Type<VimT::CMD_CARET>) { nLastX = pos.x = startAt(); }
  inline void Command(Int2Type<VimT::CMD_UNDERLINE>) { nLastX = pos.x = startAt(); }
  inline void Command(Int2Type<VimT::CMD_DOLLAR>) { nLastX = pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); }

  inline void Command(Int2Type<VimT::CMD_o>) { 
    Command(Int2Type<VimT::CMD_I>());
    lines.insert(lines.begin() + ++pos.y, std::string(pos.x, ' '));
  }

  inline void Command(Int2Type<VimT::CMD_O>) { 
    Command(Int2Type<VimT::CMD_I>());
    lines.insert(lines.begin() + pos.y, std::string(pos.x, ' '));
  }

  inline void Command(Int2Type<VimT::CMD_D>) { 
    auto len = lines[pos.y].size() - pos.x;
    if (!len) return;

    buffer = { lines[pos.y].substr(pos.x, len), false };
    lines[pos.y].erase(pos.x, buffer.first.size());

    nLastX = pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); 
  }

  inline void Command(Int2Type<VimT::CMD_dd>) {
    buffer = { lines[pos.y], true }; lines.erase(lines.begin() + pos.y);

    int32_t lineEndsAt = mode == ModeT::NORMAL;
    if (pos.x > lines[pos.y].size()) pos.x = lines[pos.y].size() - lineEndsAt;
  }

  inline void Command(Int2Type<VimT::CMD_p>) {
    if (!buffer.second) lines[pos.y].insert(++pos.x, buffer.first);
    else {
      lines.insert(lines.begin() + ++pos.y, buffer.first);
      nLastX = pos.x = startAt();
    }
  }

  inline void Command(Int2Type<VimT::CMD_P>) {
    if (!buffer.second) lines[pos.y].insert(++pos.x, buffer.first);
    else {
      lines.insert(lines.begin() + pos.y, buffer.first);
      nLastX = pos.x = startAt();
    }
  }

  // TODO: add ability to require one more char after initial command
  inline void Command(Int2Type<VimT::CMD_f>) {
    // lines.insert(lines.begin() + ++pos.y, buffer.first);
  }

  inline void Command(Int2Type<VimT::CMD_w>) { 
    auto curr = lines[pos.y][pos.x];
    while ((isAlpha(lines[pos.y][pos.x]) && isAlpha(curr)) || (!isAlpha(curr) && lines[pos.y][pos.x] == curr)) {
      if (pos.x + 1 < lines[pos.y].size()) { pos.x++; continue; }
      if (pos.y + 1 < lines.size()) { pos.y++; pos.x = 0; }
      break;
    }

    while (lines[pos.y][pos.x] == ' ') {
     if (pos.x + 1 < lines[pos.y].size()) pos.x++;
     else break;
    }

    nLastX = pos.x;
  }

  inline void Command(Int2Type<VimT::CMD_W>) { 
    while (lines[pos.y][pos.x] != ' ') {
      if (pos.x + 1 < lines[pos.y].size()) { pos.x++; continue; }
      if (pos.y + 1 < lines.size()) { pos.y++; pos.x = 0; }
      break;
    }

    while (lines[pos.y][pos.x] == ' ') {
     if (pos.x + 1 < lines[pos.y].size()) pos.x++;
     else break;
    }

    nLastX = pos.x;
  }

  // TODO: Some is wrong with this function, fix it
  inline void Command(Int2Type<VimT::CMD_b>) { 
    auto curr = lines[pos.y][pos.x];
    while ((isAlpha(lines[pos.y][pos.x]) && isAlpha(curr)) || (!isAlpha(curr) && lines[pos.y][pos.x] == curr)) {
      if (pos.x - 1 >= 0) { pos.x--; continue; }
      if (pos.y - 1 >= 0) { pos.y--; pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); }
      break;
    }

    curr = lines[pos.y][pos.x];
    while (lines[pos.y][pos.x] == ' ') {
     if (pos.x - 1 >= 0) pos.x--;
     else break;
    }

    auto space = curr; curr = lines[pos.y][pos.x];
    while (space == ' ' && ((isAlpha(lines[pos.y][pos.x]) && isAlpha(curr)) || (!isAlpha(curr) && lines[pos.y][pos.x] == curr))) {
      if (pos.x - 1 >= 0) { pos.x--; continue; }
      if (pos.y - 1 >= 0) { pos.y--; pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); }
      break;
    }

    nLastX = pos.x++;
  }

  inline void Command(Int2Type<VimT::CMD_j>) {
    if (pos.y >= lines.size()) return;

    pos.x = nLastX; nLastX = (pos += olc::vi2d(0, 1)).x;
    int32_t lineEndsAt = mode == ModeT::NORMAL;

    if (pos.x > lines[pos.y].size()) pos.x = lines[pos.y].size() - lineEndsAt;
    fBlink = 0.f;
  }

  inline void Command(Int2Type<VimT::CMD_k>) {
    if (pos.y <= 0) return;

    pos.x = nLastX; nLastX = (pos += olc::vi2d(0, -1)).x;
    int32_t lineEndsAt = mode == ModeT::NORMAL;

    if (pos.x > lines[pos.y].size()) pos.x = lines[pos.y].size() - lineEndsAt;
    fBlink = 0.f;
  }

  inline void Command(Int2Type<VimT::CMD_l>) {
    int32_t lineEndsAt = mode == ModeT::NORMAL;

    if (pos.x + lineEndsAt >= lines[pos.y].size()) return;
    nLastX = (pos += olc::vi2d(1, 0)).x; fBlink = 0.f;
  }

  inline void Command(Int2Type<VimT::CMD_h>) {
    if (pos.x <= 0) return;
    nLastX = (pos += olc::vi2d(-1, 0)).x; fBlink = 0.f;
  }

  template<int32_t T>
  void Process(Int2Type<T>, olc::PixelGameEngine* GameEngine) { }

  void Process(Int2Type<NORMAL>, olc::PixelGameEngine* GameEngine) {
    AnyType<-1, olc::PixelGameEngine*>::GetValue() = GameEngine;
    AnyType<-1, Vim*>::GetValue() = this;

    foreach<KeyEvent, AnyType<-1, Vim*>>::Process();
    if (!bUpdated) return;
    else bUpdated = false;
    
    printf("%s err: '%s'\n", cmd.c_str(), err.c_str());

    if (nCurr == 0) {
      if (match<9>({ 'i', 'I', 'a', 'A', 'o', 'O', 'C', 'D', 'R' })) { phrase(peekPrev()); return reset(); };
      if (isDigit(peek()) && peek() != '0') { nCurr++; return; }

    } else {
      uint8_t mask = ((uint8_t)isDigit(peekPrev()) << 1) + (uint8_t)isDigit(peek());

      switch(mask) {
        case 0b11: nCurr++; return;
        case 0b01: error(peek(), "Unexpected number."); return;
      }
    }

    if (peekPrev() != 'g' && match<1>({ 'g' })) { nCurr++; return; }
    if (match<25>({ 'h', 'j', 'k', 'l', 'w', 'W', 'b', 'B', 'e', 'E', '0', '$', '^', '_', 'g', 'G', '/', '?', 'n', 'N', 'f', 'F', 'r', ',', ';' })) {
      phrase(peekPrev()); // noun

      // TODO: add ability to require one more char after initial command

      auto prev = peek(nCurr - (peek(nCurr - 2) == 'g' ? 3 : 2));
      if (prev == 'c' || prev == 'd' || prev == 'y') phrase(prev); // verb

      number();
      return reset();
    }

    // adverb
    if (match<2>({ 'p' , 'P' })) { phrase(peekPrev()); number(); return reset(); }
    if (peekPrev() == 'c' && match<1>({ 'c' })) { phrase(Int2Type<VimT::CMD_cc>()); number(); return reset(); } 
    if (peekPrev() == 'd' && match<1>({ 'd' })) { phrase(Int2Type<VimT::CMD_dd>()); number(); return reset(); } 
    if (peekPrev() == 'y' && match<1>({ 'y' })) { phrase(Int2Type<VimT::CMD_yy>()); number(); return reset(); } 

    // verb
    if (match<3>({ 'c', 'd', 'y' })) return;

    error(peek(), "Invalid operation");
  }


  inline void phrase(const char c) {
    auto prev = lambda;
    AnyType<-1, Vim*>::GetValue() = this;

    bSync = foreach<VimCommands, AnyType<-1, Vim*>>::Has();

    lambda = [=]() {
      AnyType<-1, int32_t>::GetValue() = c;
      foreach<VimCommands, AnyType<-1, Vim*>>::Command();
      
      prev();
    };
  }

  template<int32_t T>
  inline void phrase(Int2Type<T> val) {
    // TODO:
    // bSync = foreach<VimCommands, AnyType<-1, Vim*>>::Has();
    bSync = true;

    auto prev = lambda;
    lambda = [=]() { Command(val); prev(); };
  }

  inline void reset(bool exec = true) {
    if (exec) {
      if (bSync) { lambda(); bUpdated = true; }
      else { std::thread t(lambda); t.detach(); }
    }

    err.clear(); cmd.clear(); nStart = nCurr = 0; lambda = []() {};
  }

  void Process(Int2Type<INSERT>, olc::PixelGameEngine* GameEngine) {
    AnyType<-1, olc::PixelGameEngine*>::GetValue() = GameEngine;
    AnyType<-1, Vim*>::GetValue() = this;

    foreach<KeyEvent, AnyType<-1, Vim*>>::Process();
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

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::UP>,    Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::UP).bPressed) Command(Int2Type<VimT::CMD_k>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::DOWN>,  Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::DOWN).bPressed) Command(Int2Type<VimT::CMD_j>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::LEFT>,  Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::LEFT).bPressed) Command(Int2Type<VimT::CMD_h>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::RIGHT>, Int2Type<U>>) { if (AnyType<-1, olc::PixelGameEngine*>::GetValue()->GetKey(olc::Key::RIGHT).bPressed) Command(Int2Type<VimT::CMD_l>()); }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::TAB>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::TAB).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case NORMAL: break;
      case INSERT: lines[pos.y].insert(pos.x += 2, "  "); break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::DEL).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case NORMAL: reset(false); break;

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

    bUpdated = true;

    switch (mode) {
      case NORMAL: reset(false); break;

      case REPLACE: 
        if (!replaced.size()) break;

        lines[pos.y].replace(--pos.x, 1, std::string(1, replaced.back()));
        replaced.pop_back();
        break;

      case INSERT: 
        if (pos.x) lines[pos.y].erase(nLastX = --pos.x, 1);
        else {
          if (!pos.y) return;
          int32_t lineEndsAt = mode == ModeT::NORMAL;
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

    bUpdated = true;

    switch (mode) {
      case NORMAL: reset(false); break;

      case INSERT: {
        std::string copy = lines[pos.y].substr(pos.x, lines[pos.y].size() - pos.x);
        lines[pos.y].erase(pos.x, copy.size());

        nLastX = pos.x = startAt();
        lines.insert(lines.begin() + ++pos.y, std::string(pos.x, ' ') + copy);
        break;
      }
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, olc::PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::ESCAPE).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case NORMAL: reset(false); break;
      case REPLACE: replaced.clear();
      case INSERT: {
        mode = NORMAL;

        auto len = lines[pos.y].size();
        nLastX = pos.x = std::min(pos.x, len ? (int32_t)len - 1 : 0);
        break;
      }
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
    std::string c(1, toUpper ? upper : lower);

    bUpdated = true;

    switch (mode) {
      case NORMAL: cmd += c; break;
      case INSERT: lines[pos.y].insert(pos.x++, c); break;
      case REPLACE: replaced.push_back(lines[pos.y][pos.x]); lines[pos.y].replace(pos.x++, 1, c); break;
    }
  }

  inline const char peek() { return cmd[nCurr]; }
  inline const char peek(int32_t nCurr) { return cmd[nCurr]; }
  inline const char peekPrev() { return nCurr == 0 ? '\0' : cmd[nCurr - 1]; }
  inline bool check(const char c) { return peek() == c; }
  inline bool isDigit(const char &c) { return c >= '0' && c <= '9'; }
  inline bool isAlpha(const char &c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }

  template<int32_t T>
  bool match(std::array<const char, T> str) {
    for (auto& c : str) {
      if (!check(c)) continue;
      nCurr++; return true;
    }

    return false;
  }

  template<int32_t T = VimT::CMD_NONE>
  void number(Int2Type<T> val = Int2Type<T>()) {
    int32_t nCurr = 0; while (isDigit(peek(nCurr))) nCurr++;
    if (nCurr == 0) return;

    auto len = std::stoul(cmd.substr(0, nCurr)); if (len == 0) return;

    auto prev = lambda;
    lambda = [=]() { for (int32_t i = 0; i < len; i++) { if (i) Command(val); prev(); } };
  }

  inline int32_t startAt() {
    int32_t startAt = 0;
    while (lines[pos.y][startAt] == ' ') startAt++;
    return startAt;
  }

  void error(const char c, std::string message) {
    std::stringstream ss; 
    ss << "Error at '" << c << "': " << message << "\n";
    reset(false); err = ss.str();
  }


private:
  int32_t nStart = 0; // index of the cmd, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the cmd, which is pointing to the curr char

  std::pair<std::string, bool> buffer; // second value is responsible to distinguish if buffer has a whole line or just a part of it
  std::string replaced;
  
  bool bSync = false; // Flag that define is lambda func is sync or async
  std::function<void(void)> lambda = []() {};

  float fBlink = 0.f;
  float fStrokeRepeat = 0.f;

  int32_t nLastX = 0;
  std::vector<std::string> lines;

public: 
  ModeT mode = NORMAL;
  bool bUpdated = false;

  olc::vi2d pos = olc::vi2d(0, 0);

  std::string cmd;
  std::string err;
};
};