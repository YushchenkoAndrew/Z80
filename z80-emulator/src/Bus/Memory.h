#pragma once

#include "src/Bus/Defs.h"
#include "src/Bus/Device.h"
#include "src/Defs.h"
#include "src/Interpreter/Disassembler.h"
#include "src/Interpreter/Lexer.h"
#include "src/Utils/Utils.h"
#include "src/Window/Window.h"
#include "src/Window/Event/KeyboardEvent.h"
#include <algorithm>

namespace Bus {

// typedef Window::SearchT SearchT;

/**
 * Grammar:
 *  command     -> phrase | motion | order
 * 
 *  motion      -> NUMBER? verb? noun
 *  order       -> NUMBER? adverb
 *  noun        -> 'h' | 'j' | 'k' | 'l' | 'w' | 'b' | 'r' | 'e' | '0' | '$' | 'gg'  | 'gd' | 'G' | '/' | '?' | 'n' | 'N' | 'f' | 'F' | ',' | ';'
 *  verb        -> 'c' | 'd' | 'y' 
 *  adverb      -> 'dd' | 'yy' | 'p' | 'P' | 'x'
 *  phrase      -> 'C' | ' ' | 'i' | 'a' | 'u' | 'U'
 */
class Memory : public Window::KeyboardEvent, public Window::Window, public Device {
public:
  enum ModeT { NORMAL, CHARACTER, DISASSEMBLE };

  // Memory(): mode(NORMAL), bus(nullptr), Window::Window(nullptr), ::Window::KeyboardEvent(nullptr) {}
  // Memory(ModeT m): mode(m), bus(nullptr), Window::Window(nullptr), ::Window::KeyboardEvent(nullptr) {}
  // Memory(int32_t s): pages(s, 0), bus(nullptr) {}

  // Memory(Bus* b): bus(b), Window::Window(nullptr),{}
  Memory(PixelGameEngine* engine, std::string id, uint32_t size): Window::Base(engine), Window::Window(engine, id), ::Window::KeyboardEvent(engine), memory(size, 0), disassembler(memory) { }

  void Disassemble() {
    disassembler.scan();
    // dasm = Registry::Z80->Disassemble();
    // lexer.scan(dasm.first); 

    // for (auto& addr : dasm.second) LINES_SIZE = std::max(LINES_SIZE, addr.second);

  }

  void Load(std::vector<uint8_t>& code) {
    memory.assign(memory.size(), 0); const auto size = std::min(code.size(), memory.size());
    std::copy(code.begin(), code.begin() + size, memory.begin());
  }

  inline uint8_t Read(uint32_t addr, bool) override { return memory[addr & (uint32_t)(memory.size() - 1)]; }
  inline uint8_t Write(uint32_t addr, uint8_t data, bool) override { return (memory[addr & (uint32_t)(memory.size() - 1)] = data); }

  // TODO: Load disassembler here
  // void Initialize() { 
  //   // if (type == MemoryT::IMS1423) Command(Int2Type<Editor::VimT::CMD_G>());
  // }

  // TODO: Maybe add ability to set local state with initialization
  void Select(DimensionT dimensions) override {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset;

    const int32_t prev = index();
    pages.x = (int32_t)(1 << (int32_t)std::floor(std::log2f((float)size.x / vStep.x)));
    pages.y = (int32_t)(size.y - vOffset.y) / vStep.y;
    Index2Pos(prev);
  }

  // inline void Lock() { 
  //   locked = true;

  //   switch (mode) {
  //     // case REPLACE: mode = NORMAL;
  //     case NORMAL: case CHARACTER: case DISASSEMBLE: break;
  //   }
  // }

  // inline void Unlock() { locked = false; }


  void Process() override {
    if (pos.y - vStartAt.y >= pages.y && pos.y < ((int32_t)memory.size() / pages.x)) {
      if (vStartAt.y + pages.y <= (int32_t)memory.size()) vStartAt.y += pages.y;
      else vStartAt.y += (vStartAt.y + pages.y) % (int32_t)memory.size();
    }

    if (pos.y - vStartAt.y < 0 && pos.y) vStartAt.y -= pages.y;

    auto nWheel = GameEngine->GetMouseWheel();
    if (nWheel > 0) MoveTo({ 0, -1 });
    else if (nWheel < 0) MoveTo({ 0, 1 });

    auto mouse = GameEngine->GetMousePos() - vOffset;
    auto nWidth = (mode == CHARACTER ? vStep.y : vStep.x) * pages.x;

    if ((mode == NORMAL || mode == CHARACTER) && GameEngine->GetMouse(0).bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + nWidth && mouse.y < absolute.y + pages.y * vStep.y) {
      MoveTo((mouse - absolute) / olc::vi2d(mode == CHARACTER ? vStep.y : vStep.x, vStep.y) + vStartAt - pos);
      // Command(Int2Type<Editor::VimT::CMD_gd>());
    } else if (mode == DISASSEMBLE && GameEngine->GetMouse(0).bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x && mouse.y < absolute.y + size.y) {
      // auto addr = Line2Index((mouse.y - absolute.y) / vStep.second.y + vStartAt.second.y - 1);
      // Index2Pos(addr); cursor.y = dasm.second[addr];
    }

    // bUpdated = false; bReleased = false;

    // switch (mode) {
    //   case NORMAL:    return Process(Int2Type<NORMAL>());
    //   case CHARACTER: return Process(Int2Type<NORMAL>());
    //   case DISASSEMBLE: return Process(Int2Type<NORMAL>()); 
    //   // case REPLACE:   return Process(Int2Type<REPLACE>(), GameEngine);
    // }
  }


  void Draw() override {
    switch (mode) {
      case NORMAL:    Draw(Int2Type<NORMAL>()); break;
      // case REPLACE:   Draw(Int2Type<NORMAL>(), GameEngine); break;
      case CHARACTER: Draw(Int2Type<CHARACTER>()); break;
      case DISASSEMBLE: Draw(Int2Type<DISASSEMBLE>()); break;
    }

    auto nWidth = (mode == CHARACTER ? vStep.y : vStep.x) * pages.x + vOffset.x;
    auto nHight = mode == DISASSEMBLE ? size.y : (absolute.y + vOffset.y + pages.y * vStep.y);

    auto pos = olc::vi2d(absolute.x, nHight);
    GameEngine->FillRect(pos - olc::vi2d(0, 2), { nWidth, 10 }, Color::VERY_DARK_GREY);
    GameEngine->DrawString(pos, GetMode(), Color::DARK_GREY);

    // auto cmd = GetCmd();
    // if (!cmd.size()) {
    pos.x += nWidth - GameEngine->GetTextSize(windowId + " ").x;
    GameEngine->DrawString(pos, windowId, Color::DARK_GREY);
    // } else {
    //   pos.x += nWidth - ((int32_t)cmd.size() + 2) * vStep.second.x;
    //   GameEngine->DrawString(pos, cmd, Color::DARK_GREY);
    // }
  }

private:
  void Draw(Int2Type<NORMAL>) {
    for (int32_t i = 0; i < pages.x; i++) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * vStep + olc::vi2d(vOffset.x, std::max(0, vOffset.y - 10));
      const auto& color = i == this->pos.x ? Color::GREY : Color::DARK_GREY;

      GameEngine->DrawString(pos, Utils::Int2Hex(i), color);
    }

    for (int32_t i = 0; i < pages.y; i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * vStep + olc::vi2d(0, vOffset.y);
      const auto& color = i + vStartAt.y == this->pos.y ? Color::GREY : Color::DARK_GREY;

      GameEngine->DrawString(pos, Utils::Int2Hex((i + vStartAt.y) * pages.x, 5), color);
      
      for (int32_t j = 0; j < pages.x; j++) {
        olc::vi2d pos = absolute + olc::vi2d(j, i) * vStep + vOffset;

        std::string val = index(j, i + vStartAt.y) >= (int32_t)memory.size() ? "--" : Utils::Int2Hex(memory[index(j, i + vStartAt.y)]);
        GameEngine->DrawString(pos, val, Color::WHITE);
      }
    }

    bool bSearch = search.bEnabled && cmd.size() - search.nStartAt > 0 && search.vPos != this->pos && (cmd.front() == '/' || cmd.front() == '?');
    auto pos = bSearch ? search.vPos : this->pos;

    olc::vi2d cursor = absolute + (pos - vStartAt) * vStep + vOffset;
    GameEngine->FillRect(cursor - olc::vi2d(1, 2), { 18, 12 }, Color::GREY);
    GameEngine->DrawString(cursor, Utils::Int2Hex(memory[index(pos)]), Color::BLACK);
  }

  void Draw(Int2Type<CHARACTER>) {
    for (int32_t i = 0; i < pages.x; i++) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * olc::vi2d(vStep.y, vStep.y) + olc::vi2d(vOffset.x, std::max(0, vOffset.y - 10));
      const auto& color = i == this->pos.x ? Color::GREY : Color::DARK_GREY;

      GameEngine->DrawString(pos, Utils::Int2Hex(i, 1), color);
    }

    for (int32_t i = 0; i < pages.y; i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * olc::vi2d(vStep.y, vStep.y) + olc::vi2d(0, vOffset.y);
      const auto& color = i + vStartAt.y == this->pos.y ? Color::GREY : Color::DARK_GREY;

      GameEngine->DrawString(pos, Utils::Int2Hex((i + vStartAt.y) * pages.x, 5), color);
      
      for (int32_t j = 0; j < pages.x; j++) {
        olc::vi2d pos = absolute + olc::vi2d(j, i) * olc::vi2d(vStep.y, vStep.y) + vOffset;
        GameEngine->DrawString(pos, isprint(memory[index(j, i + vStartAt.y)]) ? std::string(1, memory[index(j, i + vStartAt.y)]) : ".", Color::WHITE);
      }
    }

    bool bSearch = search.bEnabled && cmd.size() - search.nStartAt > 0 && search.vPos != this->pos && (cmd.front() == '/' || cmd.front() == '?');
    auto pos = bSearch ? search.vPos : this->pos;

    olc::vi2d cursor = absolute + (pos - vStartAt) * olc::vi2d(vStep.y, vStep.y) + vOffset;
    GameEngine->FillRect(cursor - olc::vi2d(1, 2), { 12, 12 }, Color::GREY);
    GameEngine->DrawString(cursor, isprint(memory[index(pos)]) ? std::string(1, memory[index(pos)]) : ".", Color::BLACK);
  }

  void Draw(Int2Type<DISASSEMBLE>) {
    // const olc::vi2d size = this->size + this->vOffset.first;

    // olc::vi2d pos = olc::vi2d(absolute.x + vOffset.second.x, absolute.y + (cursor.y + 1 - vStartAt.second.y) * vStep.second.y + vOffset.second.y);
    // GameEngine->FillRect(pos, { size.x, 8 }, Color::VERY_DARK_GREY);

    // for (auto& token : lexer.tokens) {
    //   if (vStartAt.second.y >= token.line) continue;
    //   // printf("%s \n", token->lexeme.c_str());

    //   olc::vi2d pos = absolute + (olc::vi2d(token.col, token.line) - vStartAt.second) * vStep.second + vOffset.second;

    //   // TODO: FIX bug with not displaying anything
    //   // if (pos.x > size.x) continue;
    //   if (pos.y >= size.y - vStep.second.y) break;

    //   if (pos.x < size.x && pos.x + token.lexeme.size() * vStep.second.x > size.x) {
    //     GameEngine->DrawString(pos, token.lexeme.substr(0, (size.x - pos.x) / vStep.second.x), olc::WHITE);
    //   } else GameEngine->DrawString(pos, token.lexeme, olc::WHITE);
    // }


    // pos = (absolute + (cursor - vStartAt.second) * vStep.second + vOffset.second) / vStep.second; 

    // for (int32_t i = 0, max = size.y / vStep.second.y; i < max; i++) {
    //   auto str = std::to_string(cursor.y - pos.y + i + 1);
    //   auto line = olc::vi2d(absolute.x + vOffset.second.x - str.size() * vStep.second.x, (i + 1) * vStep.second.y);

    //   if (cursor.y - vStartAt.second.y == i) {
    //     GameEngine->DrawString(line, str, Color::GREY);
    //   } else GameEngine->DrawString(line, str, Color::DARK_GREY);
    // }
  }

  // inline void Preprocess() {
  // }

  // inline void Preprocess(Int2Type<DISASSEMBLE>) {
  //   if (cursor.y - vStartAt.second.y + 3 >  (size.y + vOffset.second.y) / vStep.second.y && cursor.y < LINES_SIZE) vStartAt.second.y++;
  //   if ((cursor.y - vStartAt.second.y - 1 < 0 && cursor.y) || (!cursor.y && cursor.y != vStartAt.second.y)) vStartAt.second.y--;
  // }

public:
  // template<int32_t T>
  // inline void Command(Int2Type<T>) {}

  // { Int2Type<olc::Key::I>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::A>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::O>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::W>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::E>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::B>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::P>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::X>(), [](void* self, auto... keys) { return static_cast<Memory<SizeT>*>(self)->Process(keys...); } },

  ::Window::StrokeAction Process(Int2Type<olc::Key::X>) { memory[index()] = 0x00; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::E>) { Process(Int2Type<olc::Key::B>()); return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::K0>) { pos.x = 0; return nullptr; }

  // ::Window::StrokeAction Process(Int2Type<olc::Key::A>) { Process(Int2Type<olc::Key::W>()); Process(Int2Type<olc::Key::I>()); return nullptr; }
  // ::Window::StrokeAction Process(Int2Type<olc::Key::C>) { Process(Int2Type<olc::Key::D>()); Process(Int2Type<olc::Key::I>()); return nullptr; }
  // ::Window::StrokeAction Process(Int2Type<olc::Key::R>) { mode = REPLACE; Process(Int2Type<olc::Key::X>()); return nullptr; }
  // inline void Command(Int2Type<Editor::VimT::CMD_yy>) { buffer = { memory[index()] }; }

  ::Window::StrokeAction Process(Int2Type<olc::Key::G>,     Int2Type<olc::Key::G>)  { pos.y = pos.x = vStartAt.y = 0; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::K4>) { pos.x = pages.x - 1; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::G>)  { pos.x = pages.x - 1; pos.y = ((int32_t)memory.size() - 1) / pages.x; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::C>)  { mode = mode == NORMAL ? CHARACTER   : NORMAL; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::D>)  { mode = mode == NORMAL ? DISASSEMBLE : NORMAL; return nullptr;  }

  // inline void Command(Int2Type<Editor::VimT::CMD_i>) { 
  //   for (int32_t i = memory.size() - 1, size = index() + 1; i >= size; i--) memory[i] = memory[i - 1];

  //   search = SearchT(false, 1, 2, true, pos); 
  //   // TODO: Impl this
  //   // Command(Int2Type<Editor::VimT::CMD_r>()); 
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_gd>) { 
  //   if (type != MemoryT::W27C512) return;
  //   AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<MEMORY_SELECT_CALLBACK>(), index());
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_SEMICOLON>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (cmd.size() - search.nStartAt > 1) return;

  //   if (search.bDirection) Command(Int2Type<Editor::VimT::CMD_f>());
  //   else Command(Int2Type<Editor::VimT::CMD_F>());
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_COMMA>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (cmd.size() - search.nStartAt > 1) return;

  //   if (search.bDirection) Command(Int2Type<Editor::VimT::CMD_F>());
  //   else Command(Int2Type<Editor::VimT::CMD_f>());
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_f>) { 
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (!Utils::IsHexDigit(search.sPhrase.front())) return;
  //   if (search.bEnabled) search.bDirection = true;

  //   uint8_t digit = std::stoul(search.sPhrase, nullptr, 16);
  //   for (int32_t i = index() + 1; i < memory.size(); i++) {
  //     if (digit != memory[i]) continue;

  //     pos.x = i & (pages.x - 1); pos.y = i / pages.x;
  //     break;
  //   }
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_F>) { 
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (!Utils::IsHexDigit(search.sPhrase.front())) return;
  //   if (search.bEnabled) search.bDirection = false;

  //   uint8_t digit = std::stoul(search.sPhrase, nullptr, 16);
  //   for (int32_t i = index() - 1; i >= 0; i--) {
  //     if (digit != memory[i]) continue;
  //     pos.x = i & (pages.x - 1); pos.y = i / pages.x;
  //   }
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_n>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;

  //   if (search.bDirection) Command(Int2Type<Editor::VimT::CMD_SLASH>());
  //   else Command(Int2Type<Editor::VimT::CMD_QUESTION>());

  //   pos = search.vPos;
  // }

  // inline void Command(Int2Type<Editor:: VimT::CMD_N>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;

  //   if (search.bDirection) Command(Int2Type<Editor::VimT::CMD_QUESTION>());
  //   else Command(Int2Type<Editor::VimT::CMD_SLASH>());

  //   pos = search.vPos;
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_SLASH>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (search.bEnabled) search.bDirection = true;

  //   bool isValid = true;
  //   for (auto& c : search.sPhrase) isValid = isValid && Utils::IsHexDigit(c);
  //   if (!isValid) return;

  //   std::vector<int32_t> foundAt = {};
  //   uint8_t digit = std::stoul(search.sPhrase, nullptr, 16);

  //   for (int32_t i = 0, offset = 0; i < memory.size(); i++) {
  //     if (memory[i] != digit) continue;

  //     if (i - index() - 1 < 0) foundAt.push_back(i);
  //     else foundAt.insert(foundAt.begin() + offset++, i);
  //   }

  //   if (foundAt.size())  {
  //     int32_t index = foundAt.front();
  //     auto next = olc::vi2d(index & (pages.x - 1), index / pages.x);

  //     if (search.vPos != next) search.vPrev = search.vPos;
  //     search.vPos = next;
  //   } else { search.vPrev = search.vPos = this->pos; }
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_QUESTION>) {
  //   if (cmd.size() - search.nStartAt <= 0) return;
  //   if (search.bEnabled) search.bDirection = false;

  //   bool isValid = true;
  //   for (auto& c : search.sPhrase) isValid = isValid && Utils::IsHexDigit(c);
  //   if (!isValid) return;

  //   std::vector<int32_t> foundAt = {};
  //   uint8_t digit = std::stoul(search.sPhrase, nullptr, 16);

  //   for (int32_t i = 0, offset = 0; i < memory.size(); i++) {
  //     if (memory[i] != digit) continue;

  //     if (index() - 1 - i > 0) foundAt.insert(foundAt.begin(), i);
  //     else foundAt.insert(foundAt.end() - offset++, i);
  //   }

  //   if (foundAt.size())  {
  //     int32_t index = foundAt.front();
  //     auto next = olc::vi2d(index & (pages.x - 1), index / pages.x);

  //     if (search.vPos != next) search.vPrev = search.vPos;
  //     search.vPos = next;
  //   } else { search.vPrev = search.vPos = this->pos; }
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_P>) { 
  //   int32_t offset = buffer.size(); if (!offset) return;

  //   for (int32_t i = memory.size() - 1, size = index() + offset; i >= size; i--) memory[i] = memory[i - offset];
  //   for (int32_t i = 0, j = index(); i < offset; i++) memory[j + i] = buffer[i];
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_p>) { 
  //   if (!buffer.size()) return;

  //   Command(Int2Type<Editor::VimT::CMD_w>()); 
  //   Command(Int2Type<Editor::VimT::CMD_P>()); 

  //   for (int32_t i = 0; i < buffer.size() - 1; i++) Command(Int2Type<Editor::VimT::CMD_w>()); 
  // }


  // inline void Command(Int2Type<Editor::VimT::CMD_dd>) { 
  //   Command(Int2Type<Editor::VimT::CMD_yy>()); 

  //   for (int32_t i = index(); i < memory.size() - 1; i++) memory[i] = memory[i + 1];
  //   memory[memory.size() - 1] = 0x00;
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_y>) { 
  //   auto diff = index(AnyType<-1, olc::vi2d>::GetValue()) - index(); if (diff == 0) return;
  //   auto size = (int32_t)memory.size(); buffer = {};

  //   for (int32_t i = 0, j = index(); diff > 0 && i < diff; i++) buffer.push_back(memory[i + j]);
  //   for (int32_t i = 0, j = index(); diff < 0 && i < std::abs(diff); i++) buffer.push_back(memory[i + j + diff]);

  //   if (diff < 0) pos = AnyType<-1, olc::vi2d>::GetValue();
  // }
  
  // inline void Command(Int2Type<Editor::VimT::CMD_d>) { 
  //   auto diff = index(AnyType<-1, olc::vi2d>::GetValue()) - index(); if (diff == 0) return;
  //   auto size = (int32_t)memory.size(); buffer = {};

  //   for (int32_t i = 0, j = index(); diff > 0 && i < diff; i++) buffer.push_back(memory[i + j]);
  //   for (int32_t i = 0, j = index(); diff < 0 && i < std::abs(diff); i++) buffer.push_back(memory[i + j + diff]);

  //   for (int32_t i = index(); diff > 0 && i < size - diff; i++) memory[i] = memory[i + diff];
  //   for (int32_t i = index() + diff; diff < 0 && i < size + diff; i++) memory[i] = memory[i - diff];

  //   for (int32_t i = 0; i < std::abs(diff); i++) memory[size - 1 - std::abs(diff) + i] = 0x00;
  //   if (diff < 0) pos = AnyType<-1, olc::vi2d>::GetValue();
  // }

  ::Window::StrokeAction Process(Int2Type<olc::Key::W>) {
    if (pos.x < (pages.x - 1)) return Process(Int2Type<olc::Key::L>());
    return Process(Int2Type<olc::Key::K0>()), Process(Int2Type<olc::Key::J>());
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::B>) {
    if (pos.x > 0x00) return Process(Int2Type<olc::Key::H>());
    return Process(Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::K4>()), Process(Int2Type<olc::Key::K>());
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) { if (pos.y > 0x00) pos.y--; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::H>) { if (pos.x > 0x00) pos.x--; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) { if (pos.x < (pages.x - 1)) pos.x++; return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) { if (pos.y < ((int32_t)memory.size() - 1) / pages.x) pos.y++; return nullptr; }

  // // TODO: Impl undo/redo !!
  // inline void Command(Int2Type<Editor::VimT::CMD_u>) { }
  // inline void Command(Int2Type<Editor::VimT::CMD_U>) { }

  // inline void Command(Int2Type<Editor::VimT::CMD_CTRL_u>) {
  //   for (int32_t i = 0; i < 20; i++) Command(Int2Type<Editor::VimT::CMD_k>());
  // }
  
  // inline void Command(Int2Type<Editor::VimT::CMD_CTRL_d>) {
  //   for (int32_t i = 0; i < 20; i++) Command(Int2Type<Editor::VimT::CMD_j>());
  // }

private:
  // void Process(Int2Type<REPLACE>) {
  //   // FIXME: ....
  //   // foreach<KeyEvent, Memory>::Process(this);
  // }

  // void Process(Int2Type<NORMAL>) {
  //   // FIXME: .....
  //   // foreach<KeyEvent, Memory>::Process(this);

  //   // if (!bUpdated) return;
  //   // else bUpdated = false;
    
  //   // #ifdef DEBUG_MODE
  //   // printf("Memory: '%s'\n", cmd.c_str());
  //   // #endif

  //   // if (search.bEnabled) {
  //   //   // TODO: ADD ability to edit this string
  //   //   // std::get<0>(search2.second).push_back(cmd.back());
  //   //   // if (std::get<0>(search.second).size() == std::get<1>(search.second)) 
  //   //   if (cmd.size() - search.nStartAt >= 0) search.sPhrase = cmd.substr(search.nStartAt);
  //   //   return reset(); 
  //   // }

  //   // if (nCurr == 0) {
  //   //   if (match<6>({ 'i', 'a', 'C', 'D',  'u', 'U' })) { phrase(peekPrev()); return reset(); };
  //   //   if (Utils::IsDigit(peek()) && peek() != '0') { nCurr++; return; }

  //   //   if (cmd.size() > 1 && match<1>({ '^' })) {
  //   //     if (peek() == 'd') { phrase(Int2Type<Editor::VimT::CMD_CTRL_d>(), false); return reset(); }
  //   //     if (peek() == 'u') { phrase(Int2Type<Editor::VimT::CMD_CTRL_u>(), false); return reset(); }

  //   //     error(peek(), "Invalid operation");
  //   //   }

  //   // } else {
  //   //   uint8_t mask = ((uint8_t)Utils::IsDigit(peekPrev()) << 1) + (uint8_t)Utils::IsDigit(peek());

  //   //   switch(mask) {
  //   //     case 0b11: nCurr++; return;
  //   //     case 0b01: reset(false); return;
  //   //   }
  //   // }


  //   // if (match<21>({ 'h', 'j', 'k', 'l', 'x', 'r', 'w', 'b', 'e', '0', '$', 'G', '/', '?', 'n', 'N', 'f', 'F', 'r', ',', ';' })) {
  //   //   switch (peekPrev()) {
  //   //     case 'g': search = SearchT(true, 1, 1, true, pos); return;
  //   //     // case 'r': search = SearchT(true, 1, 2, true, pos); break; // TODO: Impl replace logic
  //   //     case 'f': case 'F': search = SearchT(true, 1, 1, true, pos); break;
  //   //     case '/': case '?': search = SearchT(true, 1, -1, true, pos); break;
  //   //   }

  //   //   // (noun * number) + verb
  //   //   phrase(peekPrev()); number(); verb(peek(nCurr - 2)); 

  //   //   return reset();
  //   // }

  //   // // adverb
  //   // if (match<2>({ 'p', 'P' })) { phrase(peekPrev()); number(); return reset(); }
  //   // if (peekPrev() == 'd' && match<1>({ 'd' })) { phrase(Int2Type<Editor::VimT::CMD_dd>(), false); number(); return reset(); } 
  //   // if (peekPrev() == 'y' && match<1>({ 'y' })) { phrase(Int2Type<Editor::VimT::CMD_yy>(), false); number(); return reset(); } 
  //   // if (peekPrev() == 'g' && match<1>({ 'd' })) { phrase(Int2Type<Editor::VimT::CMD_gd>(), false); return reset(); } 
  //   // if (peekPrev() == 'g' && match<1>({ 'g' })) { phrase(Int2Type<Editor::VimT::CMD_gg>(), true, true); number();  verb(peek(nCurr - 3)); return reset(); } 

  //   // // verb
  //   // if (match<4>({ 'c', 'd', 'y', 'g' })) return;

  //   // reset(false);
  // }

//   inline void verb(const char c) {
//     switch (c) {
//       case 'c': case 'd': case 'y': return phrase(c);
//     }
//   }

//   inline void phrase(const char c) {
//     // AnyType<-1, int32_t>::GetValue() = c;
//     // bSync = bSync || foreach<SyncMemoryCommands, AnyType<-1, int32_t>>::Has();
    
//     // auto operation = lambda;
//     // if (locked && foreach<LockedMemoryCommands, AnyType<-1, int32_t>>::Has()) lambda = []() {};
//     // else lambda = [=]() { operation(); AnyType<-1, int32_t>::GetValue() = c; foreach<MemoryCommands, Memory>::Command(this); };
//   }

//   template<int32_t T>
//   inline void phrase(Int2Type<T> val, bool isSync = true, bool forced = false) {
//     bSync = bSync || isSync;

//     auto operation = lambda;
//     if (locked && !forced) lambda = []() {};
//     else lambda = [=]() { operation(); Command(val); };
//   }

//   inline void reset(bool exec = true) {
//     // if (search.first && std::get<0>(search.second).size() != std::get<1>(search.second)) return;

//     if (exec) {
//       AnyType<-1, olc::vi2d>::GetValue() = pos;
//       if (bSync) { lambda(); bUpdated = true; }
//       else { std::thread t(lambda); t.detach(); }
//     }

//     if (search.bEnabled && cmd.size() - search.nStartAt != search.nSize) return;
//     cmd.clear(); nStart = nCurr = 0; lambda = []() {}; search.bEnabled = false; bSync = false;
//   }

public:

//   template<int32_t U>
//   void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>) {
//     auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
//     if (!GameEngine->GetKey(olc::Key::DEL).bPressed) return;

//     bUpdated = true;

//     switch (mode) {
//       case DISASSEMBLE: 
//       case CHARACTER:
//       case NORMAL: reset(false); break;

//       case REPLACE: memory[index()] = 0x00; break;
//     }
//   }

//   template<int32_t U>
//   void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>) {
//     auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
//     if (!GameEngine->GetKey(olc::Key::BACK).bPressed) return;

//     bUpdated = true;

//     switch (mode) {
//       case CHARACTER: 
//       case DISASSEMBLE: 
//       case NORMAL: if (cmd.size()) cmd.pop_back(); reset(false); break;

//       case REPLACE: 
//         memory[index()] = (memory[index()] / pages.x) & 0xF0;
//         if (cmd.size()) cmd.pop_back();
//         break;
//     }
//   }

//   template<int32_t U>
//   void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>) {
//     auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
//     if (!GameEngine->GetKey(olc::Key::ENTER).bPressed) return;

//     bUpdated = true;

//     switch (mode) {
//       case NORMAL: 
//       case CHARACTER:
//         if (search.bEnabled) { search.nSize = cmd.size() - search.nStartAt; pos = search.vPos; UpdateCursor(); }

//         reset(search.bEnabled);
//         break;

//       case DISASSEMBLE: 
//       case REPLACE: mode = NORMAL; break;
//     }
//   }

//   template<int32_t U>
//   void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) {
//     auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
//     if (!GameEngine->GetKey(olc::Key::ESCAPE).bPressed) return;

//     bUpdated = true;

//     switch (mode) {
//       case CHARACTER:
//       case DISASSEMBLE:
//       case NORMAL: search.bEnabled = false; reset(false);
//       case REPLACE: mode = NORMAL; break;
//     }
//   }

  inline void Move2Addr(uint32_t addr) { Index2Pos(addr & (memory.size() - 1)); }
  inline void MoveTo(olc::vi2d offset) {
    for (int32_t i = 0; i < std::abs(offset.y); i++) {
      if (offset.y > 0) Process(Int2Type<olc::Key::J>());
      else Process(Int2Type<olc::Key::K>());
    }

    for (int32_t i = 0; i < std::abs(offset.x); i++) {
      if (offset.x > 0) Process(Int2Type<olc::Key::L>());
      else Process(Int2Type<olc::Key::H>());
    }
  }

  inline std::string GetMode() {
    switch (mode) {
      // case REPLACE: return "-- REPLACE --";
      case DISASSEMBLE: return "-- DISASSEMBLE --";
      case CHARACTER:   return "-- CHARACTER --";
      case NORMAL:      return "-- NORMAL --";
    }

    return "";
  }

  const ::Window::StrokeT<>& GetCommands() const override { return Memory::commands; }
  // inline std::string GetCmd() { 
  //   if (search.bEnabled && (cmd.front() == '/' || cmd.front() == '?')) return "";
  //   return cmd;
  // }

  // inline std::string GetName() { 
  //   switch (type) {
  //     case MemoryT::W27C512: return "W27C512";
  //     case MemoryT::IMS1423: return "IMS1423";
  //     case MemoryT::HY62256A: return "HY62256A";
  //   }
  //   return "";
  // }


private:
  // inline void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
  //   auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
  //   bool bPressed = GameEngine->GetKey(key).bPressed;

  //   if (GameEngine->GetKey(key).bReleased) fStrokeRepeat = .0f;
  //   if (GameEngine->GetKey(key).bHeld) {
  //     fStrokeRepeat += AnyType<-1, float>::GetValue();
  //     if (fStrokeRepeat >= 0.3f) { fStrokeRepeat = .2f; bPressed = true; }
  //   }

  //   if (!bPressed) return;
  //   const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
  //   const bool isCtrl = GameEngine->GetKey(olc::Key::CTRL).bHeld;
  //   const char c = toUpper ? upper : lower;

  //   bUpdated = true;

  //   switch (mode) {
  //     case DISASSEMBLE: cmd += isCtrl ? "^" : "";
  //     case CHARACTER: case NORMAL: cmd += std::string(1, c); break;

  //     // case REPLACE: {
  //     //   if (!Utils::IsHexDigit(c)) { memory[index()] = c & 0xFF; mode = NORMAL; break; }

  //     //   memory[index()] = ((memory[index()] & 0x0F) << 4) | Utils::Hex2Int(c);
  //     //   // TODO: Fix some strange behavior with cmd 'r12'

  //     //   cmd.push_back(c);
  //     //   if (cmd.size() - search.nStartAt == search.nSize) mode = NORMAL;
  //     //   break;
  //     // }
  //   }
  // }

  // void UpdateCursor() {
  //   const int32_t posAt = index(); cursor.y = 0;

  //   // FIXME:
  //   // for (auto& addr : dasm.second) {
  //   //   if (addr.first <= posAt && addr.first >= cursor.y) cursor.y = addr.first;
  //   // }

  //   // cursor.y = dasm.second[cursor.y];
  // }

  // template<int32_t T = Editor::VimT::CMD_NONE>
  // void number(Int2Type<T> val = Int2Type<T>()) {
  //   int32_t nCurr = 0; while (Utils::IsDigit(peek(nCurr))) nCurr++;
  //   if (nCurr == 0) return;

  //   auto len = std::stoul(cmd.substr(0, nCurr)); if (len == 0) return;

  //   auto prev = lambda;
  //   lambda = [=]() { for (int32_t i = 0; i < len; i++) { if (i) Command(val); prev(); } };
  // }

  inline int32_t index() { return index(pos.x, pos.y); }
  inline int32_t index(olc::vi2d pos) { return index(pos.x, pos.y); }
  inline int32_t index(int32_t x, int32_t y) { return y * pages.x | x; }
  inline void Index2Pos(int32_t index) { pos.y = index / pages.x; pos.x = index - (pos.y * pages.x); }

  inline int32_t Line2Index(int32_t line) {
    // for (auto& addr : dasm.second) if (addr.second == line) return addr.first;
    // return dasm.second.begin()->first;
    return 0;
  }



private:
  ModeT mode = NORMAL;

  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d pages = olc::vi2d(0x10, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  olc::vi2d vStartAt = olc::vi2d(0, 0);
  const olc::vi2d vStep = olc::vi2d(20, 12);
  const olc::vi2d vOffset = olc::vi2d(44, 12);

  // bool locked = false;
  // std::vector<uint8_t> buffer = { }; 


  // Variables defines animation duration
  // float fStrokeRepeat = 0.f;

  // int32_t LINES_SIZE = 0;
  // DisassembleT dasm;
  // Interpreter::Lexer lexer;
  


protected:
  olc::vi2d pos = olc::vi2d(0, 0);
  olc::vi2d cursor = olc::vi2d(0, 0);

  std::vector<uint8_t> memory;
  Interpreter::Disassembler disassembler;

  static const ::Window::StrokeT<> commands;
};

inline const Window::StrokeT<> Memory::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::W>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::B>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::X>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::E>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::K0>(),[](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::G>(), {
    { Int2Type<olc::Key::G>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  }},
  { Int2Type<olc::Key::SHIFT>(), {
    { Int2Type<olc::Key::G>(),  [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::C>(),  [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::D>(),  [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
    { Int2Type<olc::Key::K4>(), [](void* self, auto... keys) { return static_cast<Memory*>(self)->Process(keys...); } },
  }},
});

};
