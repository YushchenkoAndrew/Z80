#pragma once
#include "Defs.h"

namespace Editor {

/**
 * This code was hardly expired by the next resource
 * Check out this link for more info: https://gist.github.com/countvajhula/0721a5fc40f2124097652071bb9f97fb
 *
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

    bool bSearch = search.first && std::get<1>(search.second) < 0;

    auto size = bSearch ? olc::vi2d(std::get<0>(search.second).size(), 1) : olc::vi2d(1, 1);
    auto pos = lambda((bSearch ?  std::get<3>(search.second) : this->pos) + olc::vi2d(1, 1)) - olc::vi2d(1, 2);

    switch (mode) {
      case NORMAL:  size *= olc::vi2d(8, 10); break;
      case INSERT:  size *= olc::vi2d(2, 10); break;
      case REPLACE: size *= olc::vi2d(8,  2); pos += olc::vi2d(0, 10); break;
    }

    GameEngine->FillRect(pos, size, AnyType<GREY, olc::Pixel>::GetValue());
  }

  void Load(std::vector<std::shared_ptr<Interpreter::Token>>& dst) {
    std::string line = ""; lines.clear();

    for (auto& token : dst) {
      if (token->line == lines.size() + 1) { line += token->lexeme; continue; }
      lines.push_back(line); line = token->lexeme;

      while (token->line != lines.size() + 1) lines.push_back("");
    }

    lines.push_back(line);
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

  inline void Command(Int2Type<VimT::CMD_i>) { mode = INSERT; }
  inline void Command(Int2Type<VimT::CMD_R>) { mode = REPLACE; }
  inline void Command(Int2Type<VimT::CMD_I>) { Command(Int2Type<VimT::CMD_i>()); nLastX = pos.x = startAt(); }
  inline void Command(Int2Type<VimT::CMD_a>) { Command(Int2Type<VimT::CMD_i>()); nLastX = pos.x++; }
  inline void Command(Int2Type<VimT::CMD_A>) { Command(Int2Type<VimT::CMD_i>()); nLastX = pos.x = lines[pos.y].size(); }


  inline void Command(Int2Type<VimT::CMD_0>)         { nLastX = pos.x = 0; }
  inline void Command(Int2Type<VimT::CMD_gg>)        { nLastX = pos.x = pos.y = 0; }
  inline void Command(Int2Type<VimT::CMD_CARET>)     { nLastX = pos.x = startAt(); }
  inline void Command(Int2Type<VimT::CMD_UNDERLINE>) { nLastX = pos.x = startAt(); }
  inline void Command(Int2Type<VimT::CMD_G>)         { nLastX = pos.x = startAt(); pos.y = lines.size() - 1; }
  inline void Command(Int2Type<VimT::CMD_DOLLAR>)    { nLastX = pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); }

  inline void Command(Int2Type<VimT::CMD_yy>) { printf("YES\n"); buffer = { { lines[pos.y] }, true }; }

  inline void Command(Int2Type<VimT::CMD_C>)  { Command(Int2Type<VimT::CMD_D>()); Command(Int2Type<VimT::CMD_A>()); }
  inline void Command(Int2Type<VimT::CMD_c>)  { Command(Int2Type<VimT::CMD_d>()); Command(Int2Type<VimT::CMD_O>()); }
  inline void Command(Int2Type<VimT::CMD_cc>) { Command(Int2Type<VimT::CMD_dd>()); Command(Int2Type<VimT::CMD_I>()); }


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

    buffer = { { lines[pos.y].substr(pos.x, len) }, false };
    lines[pos.y].erase(pos.x, buffer.first.front().size());

    nLastX = pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); 
  }

  inline void Command(Int2Type<VimT::CMD_dd>) {
    buffer = { { lines[pos.y] }, true }; lines.erase(lines.begin() + pos.y);

    int32_t lineEndsAt = mode == ModeT::NORMAL;
    if (pos.x > lines[pos.y].size()) pos.x = lines[pos.y].size() - lineEndsAt;
  }

  inline void Command(Int2Type<VimT::CMD_p>) {
    if (!buffer.second) lines[pos.y].insert(++pos.x, buffer.first.front());
    else {
      lines.insert(lines.begin() + ++pos.y, buffer.first.begin(), buffer.first.end());
      nLastX = pos.x = startAt();
    }
  }

  inline void Command(Int2Type<VimT::CMD_P>) {
    if (!buffer.second) lines[pos.y].insert(++pos.x, buffer.first.front());
    else {
      lines.insert(lines.begin() + pos.y, buffer.first.begin(), buffer.first.end());
      nLastX = pos.x = startAt();
    }
  }

  // TODO: Fix bug with text disappearing when cursor at pos (0, 0)
  inline void Command(Int2Type<VimT::CMD_x>) {
    buffer = { { lines[pos.y].substr(nLastX = pos.x, 1) }, false };
    lines[pos.y].erase(pos.x, 1);

    if (pos.x >= lines[pos.y].size()) nLastX = pos.x = std::max(pos.x - 1, 0);
  }

  inline void Command(Int2Type<VimT::CMD_r>) {
    buffer = { { lines[pos.y].substr(nLastX = pos.x, 1) }, false };

    auto c = std::get<0>(search.second).back();
    lines[pos.y].replace(pos.x, 1, std::string(1, c == '\0' ? ' ': c));
  }

  inline void Command(Int2Type<VimT::CMD_SQUIGGLE>) { 
    const char c = lines[pos.y][pos.x];
    lines[pos.y].replace(pos.x, 1, std::string(1, islower(c) ? toupper(c) : tolower(c)));
    Command(Int2Type<VimT::CMD_l>()); nLastX = pos.x;
  }

  inline void Command(Int2Type<VimT::CMD_d>) {
    auto diff = AnyType<-1, olc::vi2d>::GetValue() - pos;
    if (diff.y && (diff.x == 0 || nLastX != pos.x)) { 
      buffer = { {}, true };
      for (int32_t i = 0; i <= std::abs(diff.y); i++, pos.y -= diff.y < 0) {
        if (diff.y > 0) buffer.first.push_back(lines[pos.y]);
        else buffer.first.insert(buffer.first.begin(), lines[pos.y]);
        lines.erase(lines.begin() + pos.y);
      }

      pos.y += diff.y < 0;
      int32_t lineEndsAt = mode == ModeT::NORMAL;
      if (pos.x > lines[pos.y].size()) pos.x = lines[pos.y].size() - lineEndsAt;
    } else if (diff.x)  {
      buffer = { { lines[pos.y].substr(nLastX = pos.x += diff.x, std::abs(diff.x) + search.first) }, false };
      lines[pos.y].erase(pos.x, buffer.first.front().size());
    }
  }

  inline void Command(Int2Type<VimT::CMD_y>) {
    auto diff = AnyType<-1, olc::vi2d>::GetValue() - pos;
    if (diff.y && (diff.x == 0 || nLastX != pos.x)) { 
      buffer = { {}, true };
      for (int32_t i = 0; i <= std::abs(diff.y); i++, pos.y += (diff.y > 0) * 2 - 1) {
        if (diff.y > 0) buffer.first.push_back(lines[pos.y]);
        else buffer.first.insert(buffer.first.begin(), lines[pos.y]);
      }

      pos.y += diff.y < 0;
      int32_t lineEndsAt = mode == ModeT::NORMAL;
      if (pos.x > lines[pos.y].size()) pos.x = lines[pos.y].size() - lineEndsAt;
    } else if (diff.x)  {
      buffer = { { lines[pos.y].substr(nLastX = pos.x += diff.x, std::abs(diff.x) + search.first) }, false };
    }
  }

  inline void Command(Int2Type<VimT::CMD_SEMICOLON>) {
    if (!std::get<0>(search.second).size()) return;
    if (!std::get<0>(search.second).size() > 1) return;

    if (std::get<2>(search.second)) Command(Int2Type<VimT::CMD_f>());
    else Command(Int2Type<VimT::CMD_F>());
  }

  inline void Command(Int2Type<VimT::CMD_COMMA>) {
    if (!std::get<0>(search.second).size()) return;
    if (!std::get<0>(search.second).size() > 1) return;

    if (std::get<2>(search.second)) Command(Int2Type<VimT::CMD_F>());
    else Command(Int2Type<VimT::CMD_f>());
  }

  inline void Command(Int2Type<VimT::CMD_f>) {
    if (!std::get<0>(search.second).size()) return;
    if (search.first) std::get<2>(search.second) = true;

    auto charAt = lines[pos.y].find(std::get<0>(search.second)[0], pos.x + 1);
    if (charAt == std::string::npos) return;
    nLastX = pos.x = charAt;
  }

  inline void Command(Int2Type<VimT::CMD_F>) {
    if (!std::get<0>(search.second).size()) return;
    if (search.first) std::get<2>(search.second) = false;

    auto charAt = lines[pos.y].rfind(std::get<0>(search.second)[0], std::max(pos.x - 1, 0));
    if (charAt == std::string::npos) return;
    nLastX = pos.x = charAt;
  }

  inline void Command(Int2Type<VimT::CMD_n>) {
    if (!std::get<0>(search.second).size()) return;

    if (std::get<2>(search.second)) Command(Int2Type<VimT::CMD_SLASH>());
    else Command(Int2Type<VimT::CMD_QUESTION>());

    nLastX = (pos = std::get<3>(search.second)).x;
  }

  inline void Command(Int2Type<VimT::CMD_N>) {
    if (!std::get<0>(search.second).size()) return;

    if (std::get<2>(search.second)) Command(Int2Type<VimT::CMD_QUESTION>());
    else Command(Int2Type<VimT::CMD_SLASH>());

    nLastX = (pos = std::get<3>(search.second)).x;
  }

  inline void Command(Int2Type<VimT::CMD_SLASH>) {
    if (!std::get<0>(search.second).size()) return;
    if (search.first) std::get<2>(search.second) = true;

    std::vector<olc::vi2d> foundAt = {};

    for (int32_t line = 0, offset = 0; line < lines.size(); line++) {
      auto charAt = lines[line].find(std::get<0>(search.second));
      if (charAt == std::string::npos) continue;

      if (line > pos.y || (line == pos.y && charAt > pos.x)) { 
        foundAt.insert(foundAt.begin() + offset++, olc::vi2d(charAt, line));
      } else foundAt.push_back(olc::vi2d(charAt, line));
    }

    if (!foundAt.size()) return error("Pattern '" + std::get<0>(search.second) + "' not found.");
    std::get<3>(search.second) = foundAt.front();
  }

  inline void Command(Int2Type<VimT::CMD_QUESTION>) {
    if (!std::get<0>(search.second).size()) return;
    if (search.first) std::get<2>(search.second) = true;

    std::vector<olc::vi2d> foundAt = {};

    for (int32_t line = 0, offset = 0; line < lines.size(); line++) {
      auto charAt = lines[line].find(std::get<0>(search.second));
      if (charAt == std::string::npos) continue;

      if (line < pos.y || (line == pos.y && charAt < pos.x)) { 
        foundAt.insert(foundAt.begin(), olc::vi2d(charAt, line));
      } else foundAt.push_back(olc::vi2d(charAt, line));
    }

    if (!foundAt.size()) return error("Pattern '" + std::get<0>(search.second) + "' not found.");
    std::get<3>(search.second) = foundAt.front();
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

  // TODO: Create impl for this func
  inline void Command(Int2Type<VimT::CMD_e>) { }

  inline void Command(Int2Type<VimT::CMD_b>) { 
    auto curr = lines[pos.y][pos.x];
    while (lines[pos.y][pos.x] == ' ' || lines[pos.y][pos.x] == curr) {
      if (pos.x - 1 >= 0) { pos.x--; continue; }
      if (pos.y - 1 >= 0) { pos.y--; pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); }
      break;
    }

    curr = lines[pos.y][pos.x];
    while (isAlpha(lines[pos.y][pos.x])) {
      if (pos.x - 1 >= 0) { pos.x--; continue; }
      break;
    }

    nLastX = pos.x += isAlpha(curr);
  }

  inline void Command(Int2Type<VimT::CMD_B>) { 
    auto curr = lines[pos.y][pos.x];
    while (lines[pos.y][pos.x] == ' ' || lines[pos.y][pos.x] == curr) {
      if (pos.x - 1 >= 0) { pos.x--; continue; }
      if (pos.y - 1 >= 0) { pos.y--; pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); }
      break;
    }

    curr = lines[pos.y][pos.x];
    while (lines[pos.y][pos.x] != ' ') {
      if (pos.x - 1 >= 0) { pos.x--; continue; }
      if (pos.y - 1 >= 0) { pos.y--; pos.x = std::max((int32_t)lines[pos.y].size() - 1, 0); continue; }
      break;
    }

    nLastX = pos.x += 1;
  }

  inline void Command(Int2Type<VimT::CMD_j>) {
    if (pos.y >= lines.size()) return;

    pos.x = nLastX; nLastX = (pos += olc::vi2d(0, 1)).x;
    int32_t lineEndsAt = mode == ModeT::NORMAL;

    if (!lines[pos.y].size()) pos.x = 0;
    else if (pos.x > lines[pos.y].size() - lineEndsAt) pos.x = lines[pos.y].size() - lineEndsAt;
    fBlink = 0.f;
  }

  inline void Command(Int2Type<VimT::CMD_k>) {
    if (pos.y <= 0) return;

    pos.x = nLastX; nLastX = (pos += olc::vi2d(0, -1)).x;
    int32_t lineEndsAt = mode == ModeT::NORMAL;

    if (!lines[pos.y].size()) pos.x = 0;
    else if (pos.x > lines[pos.y].size() - lineEndsAt) pos.x = lines[pos.y].size() - lineEndsAt;
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

    if (search.first) {
      // TODO: ADD ability to edit this string
      std::get<0>(search.second).push_back(cmd.back());
      // if (std::get<0>(search.second).size() == std::get<1>(search.second)) 
      return reset(); 

      return;
    }

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


    if (match<26>({ 'h', 'j', 'k', 'l', 'x', 'w', 'W', 'b', 'B', 'e', '0', '$', '^', '_', '~', 'G', '/', '?', 'n', 'N', 'f', 'F', 'r', ',', ';' })) {
      switch (peekPrev()) {
        case 'g': search = { true, { "", 1, true, {} } }; break;
        case 'r': search = { true, { "", 1, true, {} } }; break;
        case 'f': case 'F': search = { true, { "", 1, true, {} } }; break;
        case '/': case '?': search = { true, { "", -1, true, {} } }; break;
      }

      // (noun * number) + verb
      phrase(peekPrev()); number(); verb(peek(nCurr - 2)); 

      return reset();
    }

    // adverb
    if (match<2>({ 'p' , 'P' })) { phrase(peekPrev()); number(); return reset(); }
    if (peekPrev() == 'c' && match<1>({ 'c' })) { phrase(Int2Type<VimT::CMD_cc>()); number(); return reset(); } 
    if (peekPrev() == 'd' && match<1>({ 'd' })) { phrase(Int2Type<VimT::CMD_dd>()); number(); return reset(); } 
    if (peekPrev() == 'y' && match<1>({ 'y' })) { phrase(Int2Type<VimT::CMD_yy>()); number(); return reset(); } 

    if (peekPrev() == 'g' && match<1>({ 'g' })) {  phrase(Int2Type<VimT::CMD_gg>()); number();  verb(peek(nCurr - 3)); return reset(); } 

    // verb
    if (match<4>({ 'c', 'd', 'y', 'g' })) return;

    error(peek(), "Invalid operation");
  }

  inline void verb(const char c) {
    switch (c) {
      case 'c': case 'd': case 'y': return phrase(c);
    }
  }

  inline void phrase(const char c) {
    AnyType<-1, Vim*>::GetValue() = this;
    AnyType<-1, int32_t>::GetValue() = c;

    bSync = bSync || foreach<SyncVimCommands, AnyType<-1, Vim*>>::Has();

    auto operation = lambda;
    lambda = [=]() { operation(); AnyType<-1, int32_t>::GetValue() = c; foreach<VimCommands, AnyType<-1, Vim*>>::Command(); };
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
    err.clear(); cmd.clear(); nStart = nCurr = 0; lambda = []() {}; search.first = false; bSync = false;
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
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::OEM_3>, Int2Type<U>>) { /** // FIXME: Strange bag BasicStrokeHandler(olc::Key::OEM_3, '\'',  '~'); */ }
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
      case NORMAL: 
        if (search.first) {
          std::get<1>(search.second) = std::get<0>(search.second).size();
          nLastX = (pos = std::get<3>(search.second)).x;
        }

        reset(search.first);
        break;

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
      case NORMAL: search.first = false; reset(false); break;
      case REPLACE: replaced.clear();
      case INSERT: {
        mode = NORMAL;

        auto len = lines[pos.y].size();
        nLastX = pos.x = std::min(pos.x, len ? (int32_t)len - 1 : 0);
        break;
      }
    }
  }

  inline int32_t GetLinesSize() { return lines.size(); }
  inline olc::vi2d GetPos() { return olc::vi2d(pos); }
  inline void MoveTo(olc::vi2d offset) {
    auto prev = pos;

    for (int32_t i = 0; i < std::abs(offset.y); i++) {
      if (offset.y > 0) Command(Int2Type<VimT::CMD_j>());
      else Command(Int2Type<VimT::CMD_k>());
    }

    auto curr = pos - prev;
    for (int32_t i = 0; i < std::abs(offset.x - curr.x); i++) {
      if (offset.x - curr.x > 0) Command(Int2Type<VimT::CMD_l>());
      else Command(Int2Type<VimT::CMD_h>());
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

  void error(std::string message) {
    std::stringstream ss; ss << "Error " << message << "\n"; err = ss.str();
  }


private:
  int32_t nStart = 0; // index of the cmd, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the cmd, which is pointing to the curr char

  std::string replaced = "";
  std::pair<std::vector<std::string>, bool> buffer = { { "" }, false }; // second value is responsible to distinguish if buffer has a whole line or just a part of it
  
  bool bSync = false; // Flag that define is lambda func is sync or async
  std::function<void(void)> lambda = []() {};

  // Variables defines animation duration
  float fBlink = 0.f;
  float fStrokeRepeat = 0.f;

  int32_t nLastX = 0; // Used for saving max x pos, when moving from line to line
  olc::vi2d pos = olc::vi2d(0, 0);

  std::vector<std::string> lines;

public: 
  ModeT mode = NORMAL;
  bool bUpdated = false;

  std::string cmd;
  std::string err;

  std::pair<bool, std::tuple<std::string, int32_t, bool, olc::vi2d>> search = { false, { "", 0, false, {} } }; // if first is true, then require one more "clock" to save after coming char
};
};