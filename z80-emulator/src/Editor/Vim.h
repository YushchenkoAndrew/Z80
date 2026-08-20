#pragma once
#include "Defs.h"
#include "include/Typelist.h"
#include "src/Defs.h"
#include "src/Utils/Utils.h"
#include <cstdint>
#include <string>

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
 *  phrase      -> 'i' | 'I' | 'a' | 'A' | 'o' | 'O' | 'C' | 'D' | 'R' | ' ' | 'u' | 'U'
 */
class Vim : public Window::Command {
public:
  enum ModeT { NORMAL, INSERT, REPLACE };

  inline void Lock() { 
    locked = true;

    switch (mode) {
      case NORMAL: break;
      case INSERT: case REPLACE: mode = NORMAL; break;
    }
  }

  inline void Unlock() { locked = false; }

  void Process(PixelGameEngine* GameEngine) {
    if ((fBlink += AnyType<-1, float>::GetValue()) > 1.2f) fBlink -= 1.2f;

    bUpdated = false; bReleased = false;

    // switch (mode) {
    //   case NORMAL:  return Process(Int2Type<NORMAL>(), GameEngine);
    //   case INSERT:  return Process(Int2Type<INSERT>(), GameEngine);
    //   case REPLACE: return Process(Int2Type<INSERT>(), GameEngine);
    // }
  }

  void Draw(PixelGameEngine* GameEngine, std::function<olc::vi2d(olc::vi2d pos)> lambda) {
    if (fBlink > 0.6f) return;

    bool bSearch = search.bEnabled && cmd.size() - search.nStartAt > 0 && search.vPos != this->pos && (cmd.front() == '/' || cmd.front() == '?');

    auto size = bSearch ? olc::vi2d(cmd.size() - search.nStartAt, 1) : olc::vi2d(1, 1);
    auto pos = lambda((bSearch ? search.vPos : this->pos) + olc::vi2d(1, 1)) - olc::vi2d(1, 2);

    switch (mode) {
      case NORMAL:  size *= olc::vi2d(8, 10); break;
      case INSERT:  size *= olc::vi2d(2, 10); break;
      case REPLACE: size *= olc::vi2d(8,  2); pos += olc::vi2d(0, 10); break;
    }

    GameEngine->FillRect(pos, size, *AnyType<GREY, ColorT>::GetValue());
  }

  void Load(std::vector<std::string>& dst) {
    lines.clear(); lines.insert(lines.end(), dst.begin(), dst.end());
  }

  void Load(std::string dst) {
    lines.clear();

    size_t start = 0; // Current search offset
    size_t end = 0; // Find from the offset

    while ((end = dst.find('\n', start)) != std::string::npos) {
      // Extract substring between 'start' and 'end'
      lines.push_back(dst.substr(start, end - start));
        
      // Move the offset past the delimiter for the next search
      start = end + 1;
    }
    
    // Don't forget to grab the very last token after the final delimiter
    lines.push_back(dst.substr(start));
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

  inline std::vector<std::string>& GetLines() { return lines; }
  inline std::string Text() {
    std::string text = "";
    for (auto& ln : lines) text += ln + "\n";
    return text;
  }

  template<int32_t T>
  inline void Command(Int2Type<T>) {}

  Window::StrokeAction Process(Int2Type<olc::Key::I>) { return mode = INSERT, nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::A>) { return mode = INSERT, pos.x++, nullptr; }

  Window::StrokeAction Process(Int2Type<olc::Key::ESCAPE>) { return mode = NORMAL, nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::K0>)     { return pos.x = 0, nullptr; }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::R>) { return mode = REPLACE, nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::I>) { return mode = INSERT, pos.x = startAt(), nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::A>) { return mode = INSERT, pos.x = endAt(),   nullptr; }

  Window::StrokeAction Process(Int2Type<olc::Key::G>,     Int2Type<olc::Key::G>) { return pos.x = pos.y = 0, nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::G>) { return pos.x = startAt(), pos.y = lines.size() - 1, nullptr; }

  Window::StrokeAction Process(Int2Type<olc::Key::G>,     Int2Type<olc::Key::D>) { AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<EDITOR_SELECT_CALLBACK>(), pos); return nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::G>,     Int2Type<olc::Key::P>) { AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<EDITOR_SELECT_LINE_CALLBACK>(), pos); return nullptr; }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::K6>)    { return pos.x = startAt(), nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::K4>)    { return pos.x = endAt(),   nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::MINUS>) { return pos.x = startAt(), nullptr; }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::U>) { for (uint8_t i = 0; i < 5; i++) Process(Int2Type<olc::Key::K>()); return nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::D>) { for (uint8_t i = 0; i < 5; i++) Process(Int2Type<olc::Key::J>()); return nullptr; }

  Window::StrokeAction Process(Int2Type<olc::Key::Y>, Int2Type<olc::Key::Y>) { return buffer = { "\n" + line() }, nullptr; }
  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::Y>) {
    auto length = line().size() - pos.x;

    if (!length) return buffer.clear(), nullptr;
    return buffer = { line().substr(pos.x, length) }, nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::C>) {
    Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::D>());
    Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::A>());
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::C>, Int2Type<olc::Key::C>) {
    Process(Int2Type<olc::Key::D>(), Int2Type<olc::Key::D>());
    Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::I>());
    return nullptr;
  };

  Window::StrokeAction Process(Int2Type<olc::Key::O>) { 
    Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::I>());
    lines.insert(lines.begin() + ++pos.y, std::string(pos.x, ' '));
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::O>) { 
    Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::I>());
    lines.insert(lines.begin() + pos.y, std::string(pos.x, ' '));
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::D>) { 
    auto length = line().size() - pos.x;
    if (!length) return buffer.clear(), nullptr;

    Process(Int2Type<olc::Key::Y>(), Int2Type<olc::Key::Y>());
    line().erase(pos.x, line().size()); pos.x = endAt();
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::D>, Int2Type<olc::Key::D>) { 
    if (!lines.size()) return nullptr;

    Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::Y>());
    lines.erase(lines.begin() + pos.y); pos.x = startAt();
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::D>, Int2Type<olc::Key::J>) {
    // TODO: 
    // Process(Int2Type<olc::Key::D>(), Int2Type<olc::Key::D>());
    // Process(Int2Type<olc::Key::D>(), Int2Type<olc::Key::D>());
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::D>, Int2Type<olc::Key::K>) {
    // TODO: 
    // Process(Int2Type<olc::Key::K>());
    // Process(Int2Type<olc::Key::D>(), Int2Type<olc::Key::D>());
    // Process(Int2Type<olc::Key::D>(), Int2Type<olc::Key::D>());
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::D>, Int2Type<olc::Key::L>) {
    Process(Int2Type<olc::Key::X>());
    return nullptr;
  }
  Window::StrokeAction Process(Int2Type<olc::Key::D>, Int2Type<olc::Key::H>) { 
    Process(Int2Type<olc::Key::H>());
    Process(Int2Type<olc::Key::X>());
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::D>, Int2Type<olc::Key::W>) { 
    const auto copy = pos;
    Process(Int2Type<olc::Key::W>());

    const auto length = pos.y == copy.y ? pos.x - copy.x : lines[copy.y].size() - copy.x;

    pos = copy; 

    if (!length) buffer.clear();
    else { buffer = { line().substr(pos.x, length) }; line().erase(copy.x, length); }

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::C>, Int2Type<olc::Key::W>) { 
    Process(Int2Type<olc::Key::D>(), Int2Type<olc::Key::W>());
    Process(Int2Type<olc::Key::I>());
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::P>) { 
    int32_t index = 0;
    for (auto& ln : buffer) {
      if (!index++ && ln.size() && ln.at(0) == '\n') lines.insert(lines.begin() + ++pos.y, ln.substr(1));
      else line().insert(++pos.x, ln);
    }

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::P>) { 
    if (!buffer.size()) return nullptr;

    const auto& ln = buffer.front();
    if (ln.size() && ln.at(0) == '\n') Process(Int2Type<olc::Key::K>());
    else Process(Int2Type<olc::Key::H>());

    Process(Int2Type<olc::Key::P>());
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::X>) { 
    buffer = { { symbol() } };
    line().erase(pos.x, 1);

    pos.x = std::min(pos.x, (int)line().size());
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::R>) { 
    return [this](olc::Key k) {
      line().replace(pos.x, 1, GetKeycode(k)); // FIXME: Think about how to impl shift+ascii ?????
    };
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::OEM_3>) { 
    const char& c = symbol();
    line().replace(pos.x, 1, std::string(1, islower(c) ? toupper(c) : tolower(c)));
    Process(Int2Type<olc::Key::L>());
    return nullptr;
  }

  // // TODO: Impl undo/redo !!
  // inline void Command(Int2Type<VimT::CMD_u>) { }
  // inline void Command(Int2Type<VimT::CMD_U>) { }

  // inline void Command(Int2Type<VimT::CMD_SEMICOLON>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (cmd.size() - search.nStartAt > 1) return;

  //   if (search.bDirection) Command(Int2Type<VimT::CMD_f>());
  //   else Command(Int2Type<VimT::CMD_F>());
  // }

  // inline void Command(Int2Type<VimT::CMD_COMMA>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (cmd.size() - search.nStartAt > 1) return;

  //   if (search.bDirection) Command(Int2Type<VimT::CMD_F>());
  //   else Command(Int2Type<VimT::CMD_f>());
  // }

  // inline void Command(Int2Type<VimT::CMD_f>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (search.bEnabled) search.bDirection = true;

  //   auto charAt = lines[pos.y].find(cmd[search.nStartAt], pos.x + 1);
  //   if (charAt == std::string::npos) return;
  //   nLastX = pos.x = charAt;
  // }

  // inline void Command(Int2Type<VimT::CMD_F>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (search.bEnabled) search.bDirection = false;

  //   auto charAt = lines[pos.y].rfind(cmd[search.nStartAt], std::max(pos.x - 1, 0));
  //   if (charAt == std::string::npos) return;
  //   nLastX = pos.x = charAt;
  // }

  // inline void Command(Int2Type<VimT::CMD_n>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;

  //   if (search.bDirection) Command(Int2Type<VimT::CMD_SLASH>());
  //   else Command(Int2Type<VimT::CMD_QUESTION>());

  //   nLastX = (pos = search.vPos).x;
  // }

  // inline void Command(Int2Type<VimT::CMD_N>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;

  //   if (search.bDirection) Command(Int2Type<VimT::CMD_QUESTION>());
  //   else Command(Int2Type<VimT::CMD_SLASH>());

  //   nLastX = (pos = search.vPos).x;
  // }

  // inline void Command(Int2Type<VimT::CMD_SLASH>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (search.bEnabled) search.bDirection = true;

  //   auto pos = search.vPrev;
  //   std::vector<olc::vi2d> foundAt = {};

  //   for (int32_t line = 0, offset = 0; line < lines.size(); line++) {
  //     auto charAt = lines[line].find(search.sPhrase);
  //     if (charAt == std::string::npos) continue;

  //     if (line > pos.y || (line == pos.y && charAt > pos.x)) { 
  //       foundAt.insert(foundAt.begin() + offset++, olc::vi2d(charAt, line));
  //     } else foundAt.push_back(olc::vi2d(charAt, line));
  //   }

  //   if (foundAt.size()) {
  //     if (search.vPos != foundAt.front()) search.vPrev = search.vPos;

  //     search.vPos = foundAt.front();
  //   } else { search.vPrev = search.vPos = this->pos; error("Pattern '" + search.sPhrase + "' not found."); }
  // }

  // inline void Command(Int2Type<VimT::CMD_QUESTION>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (search.bEnabled) search.bDirection = true;

  //   auto pos = search.vPrev;
  //   std::vector<olc::vi2d> foundAt = {};

  //   for (int32_t line = 0, offset = 0; line < lines.size(); line++) {
  //     auto charAt = lines[line].find(search.sPhrase);
  //     if (charAt == std::string::npos) continue;

  //     if (line < pos.y || (line == pos.y && charAt < pos.x)) { 
  //       foundAt.insert(foundAt.begin(), olc::vi2d(charAt, line));
  //     } else foundAt.push_back(olc::vi2d(charAt, line));
  //   }

  //   if (foundAt.size()) {
  //     if (search.vPos != foundAt.front()) search.vPrev = search.vPos;

  //     search.vPos = foundAt.front();
  //   } else { search.vPrev = search.vPos = this->pos; error("Pattern '" + search.sPhrase + "' not found."); }
  // }

  Window::StrokeAction Process(Int2Type<olc::Key::W>) {
    const char startAt = Utils::Classify(symbol());

    for (bool shouldProceed = true, bFoundGap = false; shouldProceed; ) {
      if (pos.x != endAt()) Process(Int2Type<olc::Key::L>());
      else { Process(Int2Type<olc::Key::J>()); Process(Int2Type<olc::Key::K0>()); }

      if (pos.x == endAt() && pos.y == lines.size() - 1) break; // Infinite loop safety check at EOF
      const char current = Utils::Classify(symbol());

      // Handle transition out of the initial word/punctuation block
      if (startAt != 0 && current != startAt && !bFoundGap) bFoundGap = true;

      // Stop condition: we found the start of the next non-whitespace block
      if ((bFoundGap || startAt == 0) && current != 0) shouldProceed = false;
    }

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::E>) {
    const char startAt = Utils::Classify(symbol());

    for (bool shouldProceed = true, bFoundGap = false; shouldProceed; ) {
      if (pos.x == endAt() && pos.y == lines.size() - 1) break; // EOF safety check

      if (pos.x != endAt()) Process(Int2Type<olc::Key::L>());
      else { Process(Int2Type<olc::Key::J>()); Process(Int2Type<olc::Key::K0>()); }

      const char current = Utils::Classify(symbol());
      const char next = (pos.x == endAt()) ? 0 : Utils::Classify(lines[pos.y][pos.x + 1]);

      // Handle transition out of the initial word/punctuation block
      if (startAt != 0 && current != startAt && !bFoundGap) bFoundGap = true;

      // Stop condition: we are on a valid block, and the next character changes type
      if (current != 0 && (bFoundGap || next != current)) shouldProceed = false;
    }

    return nullptr;
  }
  
  Window::StrokeAction Process(Int2Type<olc::Key::B>) {
    const char startAt = Utils::Classify(symbol());

    for (bool shouldProceed = true, bFoundGap = false; shouldProceed; ) {
      if (!pos.x && !pos.y) break; // Start of File safety check

      if (pos.x != 0) Process(Int2Type<olc::Key::H>());
      else { Process(Int2Type<olc::Key::K>()); Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::K4>()); }

      const char current = Utils::Classify(symbol());
      const char prev = (pos.x == 0) ? 0 : Utils::Classify(lines[pos.y][pos.x - 1]);

      // Handle transition out of the initial word/punctuation block
      if (startAt != 0 && current != startAt && !bFoundGap) bFoundGap = true;

      // Stop condition: we are on a valid block, and the previous character changes type
      if (current != 0 && (bFoundGap || prev != current)) shouldProceed = false;
    }

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::W>) {
    const char startAt = Utils::Classify(symbol()) == 0 ? 0 : 1;

    for (bool shouldProceed = true, bFoundGap = false; shouldProceed; ) {
      if (!pos.x && !pos.y) break; // Start of File safety check

      if (pos.x != 0) Process(Int2Type<olc::Key::H>());
      else { Process(Int2Type<olc::Key::K>()); Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::K4>()); }

      const char current = Utils::Classify(symbol()) == 0 ? 0 : 1;
      const char prev = (pos.x == 0) ? 0 : (Utils::Classify(lines[pos.y][pos.x - 1]) == 0 ? 0 : 1);

      // Handle transition out of the initial WORD block
      if (startAt != 0 && current != startAt && !bFoundGap) bFoundGap = true;

      // Stop condition: we are on a valid block, and the previous character changes type (start of word)
      if (current != 0 && (bFoundGap || prev != current)) shouldProceed = false;
    }

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::E>) {
    const char startAt = Utils::Classify(symbol()) == 0 ? 0 : 1;

    for (bool shouldProceed = true, bFoundGap = false; shouldProceed; ) {
      if (pos.x != 0) Process(Int2Type<olc::Key::H>());
      else { Process(Int2Type<olc::Key::K>()); Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::K4>()); }

      if (!pos.x && !pos.y) break; // Start of File safety check
      const char current = Utils::Classify(symbol()) == 0 ? 0 : 1;

      // Handle transition out of the initial WORD block
      if (startAt != 0 && current != startAt && !bFoundGap) bFoundGap = true;

      // Stop condition: found the start of the next non-whitespace block coming from behind
      if ((bFoundGap || startAt == 0) && current != 0) shouldProceed = false;
    }

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::B>) {
    const char startAt = Utils::Classify(symbol()) == 0 ? 0 : 1;

    for (bool shouldProceed = true, bFoundGap = false; shouldProceed; ) {
      if (!pos.x && !pos.y) break; // Start of File safety check

      if (pos.x != 0) Process(Int2Type<olc::Key::H>());
      else { Process(Int2Type<olc::Key::K>()); Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::K4>()); }

      const char current = Utils::Classify(symbol()) == 0 ? 0 : 1;
      const char prev = (pos.x == 0) ? 0 : (Utils::Classify(lines[pos.y][pos.x - 1]) == 0 ? 0 : 1);

      // Handle transition out of the initial WORD block
      if (startAt != 0 && current != startAt && !bFoundGap) bFoundGap = true;

      // Stop condition: we are on a valid block, and the previous character changes type
      if (current != 0 && (bFoundGap || prev != current)) shouldProceed = false;
    }

    return nullptr;
  }


  Window::StrokeAction Process(Int2Type<olc::Key::J>) {
    if (pos.y + 1 >= lines.size()) return nullptr;

    pos.y++; 

    if (!line().size()) pos.x = 0;
    else if (pos.x > endAt()) pos.x = endAt();

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::K>) {
    if (pos.y <= 0) return nullptr;

    pos.y--;

    if (!line().size()) pos.x = 0;
    else if (pos.x > endAt()) pos.x = endAt();

    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::L>) {
    if (pos.x < endAt()) pos.x++;
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::H>) {
    if (pos.x > 0) pos.x--;
    return nullptr;
  }

  inline int32_t GetLineSize() { return lines.size(); }
  inline int32_t GetLineSize(int32_t i) { return lines.size() > i ? lines[i].size() : 0; }

  inline olc::vi2d GetPos() { return olc::vi2d(pos); }
  inline void MoveTo(olc::vi2d offset) {
    auto prev = pos;

    for (int32_t i = 0; i < std::abs(offset.y); i++) {
      if (offset.y > 0) Process(Int2Type<olc::Key::J>());
      else Process(Int2Type<olc::Key::K>());
    }

    auto curr = pos - prev;
    for (int32_t i = 0; i < std::abs(offset.x - curr.x); i++) {
      if (offset.x - curr.x > 0) Process(Int2Type<olc::Key::L>());
      else Process(Int2Type<olc::Key::H>());
    }
  }

  inline std::string GetMode() {
    switch (mode) {
      case REPLACE: return "-- REPLACE --";
      case INSERT:  return "-- INSERT --";
      case NORMAL:  return "-- NORMAL --";
        // if (search.bEnabled && (cmd.front() == '/' || cmd.front() == '?' || cmd.front() == ':')) return cmd;
        // if (err.size()) return err;
    }

    return "";
  }

  inline std::string GetCmd() { 
    if (search.bEnabled && (cmd.front() == '/' || cmd.front() == '?' || cmd.front() == ':')) return "";
    return cmd;
  }

  inline std::string GetHumanizedPos() {
    return "Ln " + std::to_string(pos.y + 1) + ", Col " + std::to_string(pos.x + 1);
  }

  inline olc::vi2d& vStartAt() { return offset; }

  inline bool IsSelected(int32_t y) { 
    for (auto& line : lSeleted) if (line == y) return true;
    return false;
  }

  inline void SelectLine(int32_t line) {
    if (IsSelected(line)) lSeleted.remove(line);
    else lSeleted.push_back(line);
  }


private:
  inline int32_t startAt() {
    int32_t startAt = 0;
    while (line().at(startAt) == ' ') startAt++;
    return startAt;
  }

  inline int32_t endAt() { return std::max((int32_t)line().size() - 1, 0); }
  inline std::string& line() { return lines[pos.y]; }
  inline char& symbol() { return line().at(pos.x); }

public:
  const Window::StrokeT<>& GetCommands() const override { return Vim::commands; }

private:
  ModeT mode = NORMAL;
  bool locked = false;

  std::list<std::string> buffer = {};

  // std::list<std::string> history = {};

  // std::string replaced = "";
  
  // Variables defines animation duration
  float fBlink = 0.f;
  float fStrokeRepeat = 0.f;

  // int32_t nLastX = 0; // Used for saving max x pos, when moving from line to line

  olc::vi2d pos = olc::vi2d(0, 0);
  olc::vi2d offset = olc::vi2d(0, 0);

public:
  std::vector<std::string> lines;
  std::list<int32_t> lSeleted;

  static const Window::StrokeT<> commands;
};

inline const Window::StrokeT<> Vim::commands = Window::StrokeT<>::Init({
  { Int2Type<olc::Key::K0>(),     [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::ESCAPE>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },

  { Int2Type<olc::Key::I>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::A>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::O>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::W>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::E>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::B>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::P>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::X>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::R>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::Y>(), {
    { Int2Type<olc::Key::Y>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  }},
  { Int2Type<olc::Key::D>(), {
    { Int2Type<olc::Key::D>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::W>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  }},
  { Int2Type<olc::Key::C>(), {
    { Int2Type<olc::Key::C>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::W>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  }},
  { Int2Type<olc::Key::G>(), {
    { Int2Type<olc::Key::G>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::P>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::D>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  }},
  { Int2Type<olc::Key::SHIFT>(), {
    { Int2Type<olc::Key::G>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::I>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::A>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::R>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::O>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::D>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::C>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::Y>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::W>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::E>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::B>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::P>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },

    { Int2Type<olc::Key::K4>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::K6>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::OEM_3>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::MINUS>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  }},
  { Int2Type<olc::Key::CTRL>(), {
    { Int2Type<olc::Key::U>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::D>(), [](void* self, auto... keys) { return static_cast<Vim*>(self)->Process(keys...); } },
  }}

  //   { Int2Type<olc::Key::SPACE>(), {
  //     { Int2Type<olc::Key::K1>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::K2>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::K3>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::K4>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::K5>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },

  //     { Int2Type<olc::Key::A>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::C>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::D>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::Q>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     { Int2Type<olc::Key::Z>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },

  //     { Int2Type<olc::Key::SHIFT>(), {
  //       { Int2Type<olc::Key::K7>(),    [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //       { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  //     }},
  //   }}
  // }}
});
};