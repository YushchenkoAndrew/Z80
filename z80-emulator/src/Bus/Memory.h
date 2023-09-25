#pragma once
#include "Lcd.h"

namespace Bus {

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
template<int32_t TypeT, int32_t SizeT>
class Memory : public Window::Window, public Window::Command, public Device {
public:
  enum { type = TypeT };
  enum ModeT { NORMAL, REPLACE, CHARACTER, DISASSEMBLE };

  Memory(): mode(NORMAL), bus(nullptr) {}
  Memory(ModeT m): mode(m), bus(nullptr) {}
  // Memory(int32_t s): pages(s, 0), bus(nullptr) {}

  Memory(Bus* b): bus(b) {}

  void Disassemble();

  void load(std::vector<uint8_t> code) {
    for (auto& bank : memory) bank = 0x00;

    for (uint32_t i = 0; i < code.size(); i++) memory[i] = code[i];
  }

  inline uint8_t Read(uint32_t addr, bool) { return memory[addr & (uint32_t)(memory.size() - 1)]; }
  inline uint8_t Write(uint32_t addr, uint8_t data, bool) { return (memory[addr & (uint32_t)(memory.size() - 1)] = data); }

  // TODO: Load disassembler here
  void Preinitialize() {}

  // TODO: Maybe add ability to set local state with initialization
  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset.first;

    pages.x = 1 << (int32_t)std::floor(std::log2f((float)size.x / vStep.first.x));
    pages.y = ((size.y - vOffset.first.y * 2) / (vStep.first.y * pages.x)) * pages.x;
    Index2Pos(index());
  }

  inline void Lock() { 
    locked = true;

    switch (mode) {
      case REPLACE: mode = NORMAL;
      case NORMAL: case CHARACTER: case DISASSEMBLE: break;
    }
  }

  inline void Unlock() { locked = false; }


  void Preprocess() {
    switch (mode) {
      case NORMAL:      return Preprocess(Int2Type<NORMAL>());
      case CHARACTER:   return Preprocess(Int2Type<NORMAL>());
      case DISASSEMBLE: return Preprocess(Int2Type<DISASSEMBLE>()); 
      case REPLACE:     return Preprocess(Int2Type<NORMAL>());
    }
  }

  void Process(PixelGameEngine* GameEngine) {
    auto nWheel = GameEngine->GetMouseWheel();
    if (nWheel > 0) MoveTo({ 0, -1 });
    else if (nWheel < 0) MoveTo({ 0, 1 });

    auto mouse = GameEngine->GetMousePos() - (mode == DISASSEMBLE ? vOffset.second : vOffset.first);
    auto nWidth = (mode == CHARACTER ? vStep.first.y : vStep.first.x) * pages.x;

    if ((mode == NORMAL || mode == CHARACTER) && GameEngine->GetMouse(0).bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + nWidth && mouse.y < absolute.y + pages.y * vStep.first.y) {
      MoveTo((mouse - absolute) / olc::vi2d(mode == CHARACTER ? vStep.first.y : vStep.first.x, vStep.first.y) + vStartAt.first - pos);
      Command(Int2Type<Editor::VimT::CMD_gd>());
    } else if (mode == DISASSEMBLE && GameEngine->GetMouse(0).bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x && mouse.y < absolute.y + size.y) {
      auto addr = Line2Index((mouse.y - absolute.y) / vStep.second.y + vStartAt.second.y - 1);
      Index2Pos(addr); cursor.y = dasm.second[addr];
    }

    bUpdated = false;

    switch (mode) {
      case NORMAL:    return Process(Int2Type<NORMAL>(), GameEngine);
      case CHARACTER: return Process(Int2Type<NORMAL>(), GameEngine);
      case DISASSEMBLE: return Process(Int2Type<NORMAL>(), GameEngine); 
      case REPLACE:   return Process(Int2Type<REPLACE>(), GameEngine);
    }
  }


  void Draw(PixelGameEngine* GameEngine) {
    switch (mode) {
      case NORMAL:    Draw(Int2Type<NORMAL>(), GameEngine); break;
      case REPLACE:   Draw(Int2Type<NORMAL>(), GameEngine); break;
      case CHARACTER: Draw(Int2Type<CHARACTER>(), GameEngine); break;
      case DISASSEMBLE: Draw(Int2Type<DISASSEMBLE>(), GameEngine); break;
    }

    auto nWidth = (mode == CHARACTER ? vStep.first.y : vStep.first.x) * pages.x + vOffset.first.x;
    auto nHight = mode == DISASSEMBLE ? size.y : (absolute.y + vOffset.first.y + pages.y * vStep.first.y);

    auto pos = olc::vi2d(absolute.x, nHight);
    GameEngine->FillRect(pos - olc::vi2d(0, 2), { nWidth, 10 }, ~AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());
    GameEngine->DrawString(pos, GetMode(), ~AnyType<Colors::DARK_GREY, ColorT>::GetValue());

    auto cmd = GetCmd();
    pos.x = nWidth - ((int32_t)cmd.size() + 1) * vStep.first.y;
    GameEngine->DrawString(pos, cmd, ~AnyType<Colors::DARK_GREY, ColorT>::GetValue());
  }

private:
  void Draw(Int2Type<NORMAL>, PixelGameEngine* GameEngine) {
    for (int32_t i = 0; i < pages.x; i++) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * vStep.first + olc::vi2d(vOffset.first.x, 0);
      auto color = i == this->pos.x ? AnyType<GREY, ColorT>::GetValue() : AnyType<DARK_GREY, ColorT>::GetValue();

      GameEngine->DrawString(pos, Int2Hex(i), color.val);
    }

    for (int32_t i = 0; i < pages.y; i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * vStep.first + olc::vi2d(0, vOffset.first.y);
      auto color = i + vStartAt.first.y == this->pos.y ? AnyType<GREY, ColorT>::GetValue() : AnyType<DARK_GREY, ColorT>::GetValue();

      GameEngine->DrawString(pos, Int2Hex((i + vStartAt.first.y) * pages.x, 5), color.val);
      
      for (int32_t j = 0; j < pages.x; j++) {
        olc::vi2d pos = absolute + olc::vi2d(j, i) * vStep.first + vOffset.first;
        GameEngine->DrawString(pos, Int2Hex(memory[index(j, i + vStartAt.first.y)]), ~AnyType<WHITE, ColorT>::GetValue());
      }
    }

    bool bSearch = search.first && std::get<0>(search.second).size() && (cmd.front() == '/' || cmd.front() == '?');
    auto pos = bSearch ? std::get<3>(search.second) : this->pos;

    olc::vi2d cursor = absolute + (pos - vStartAt.first) * vStep.first + vOffset.first;
    GameEngine->FillRect(cursor - olc::vi2d(1, 2), { 18, 12 }, ~AnyType<GREY, ColorT>::GetValue());
    GameEngine->DrawString(cursor, Int2Hex(memory[index(pos)]), ~AnyType<BLACK, ColorT>::GetValue());
  }

  void Draw(Int2Type<CHARACTER>, PixelGameEngine* GameEngine) {
    for (int32_t i = 0; i < pages.x; i++) {
      olc::vi2d pos = absolute + olc::vi2d(i, 0) * olc::vi2d(vStep.first.y, vStep.first.y) + olc::vi2d(vOffset.first.x, 0);
      auto color = i == this->pos.x ? AnyType<GREY, ColorT>::GetValue() : AnyType<DARK_GREY, ColorT>::GetValue();

      GameEngine->DrawString(pos, Int2Hex(i, 1), color.val);
    }

    for (int32_t i = 0; i < pages.y; i++) {
      olc::vi2d pos = absolute + olc::vi2d(0, i) * olc::vi2d(vStep.first.y, vStep.first.y) + olc::vi2d(0, vOffset.first.y);
      auto color = i + vStartAt.first.y == this->pos.y ? AnyType<GREY, ColorT>::GetValue() : AnyType<DARK_GREY, ColorT>::GetValue();

      GameEngine->DrawString(pos, Int2Hex((i + vStartAt.first.y) * pages.x, 5), color.val);
      
      for (int32_t j = 0; j < pages.x; j++) {
        olc::vi2d pos = absolute + olc::vi2d(j, i) * olc::vi2d(vStep.first.y, vStep.first.y) + vOffset.first;
        GameEngine->DrawString(pos, isprint(memory[index(j, i + vStartAt.first.y)]) ? std::string(1, memory[index(j, i + vStartAt.first.y)]) : ".", ~AnyType<WHITE, ColorT>::GetValue());
      }
    }

    bool bSearch = search.first && std::get<0>(search.second).size() && (cmd.front() == '/' || cmd.front() == '?');
    auto pos = bSearch ? std::get<3>(search.second) : this->pos;

    olc::vi2d cursor = absolute + (pos - vStartAt.first) * olc::vi2d(vStep.first.y, vStep.first.y) + vOffset.first;
    GameEngine->FillRect(cursor - olc::vi2d(1, 2), { 12, 12 }, ~AnyType<GREY, ColorT>::GetValue());
    GameEngine->DrawString(cursor, isprint(memory[index(pos)]) ? std::string(1, memory[index(pos)]) : ".", ~AnyType<BLACK, ColorT>::GetValue());
  }

  void Draw(Int2Type<DISASSEMBLE>, PixelGameEngine* GameEngine) {
    const olc::vi2d size = this->size + this->vOffset.first;

    olc::vi2d pos = olc::vi2d(absolute.x + vOffset.second.x, absolute.y + (cursor.y + 1 - vStartAt.second.y) * vStep.second.y + vOffset.second.y);
    GameEngine->FillRect(pos, { size.x, 8 }, ~AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());

    for (auto& token : lexer.tokens) {
      if (vStartAt.second.y >= token->line) continue;
      // printf("%s \n", token->lexeme.c_str());

      olc::vi2d pos = absolute + (olc::vi2d(token->col, token->line) - vStartAt.second) * vStep.second + vOffset.second;

      // TODO: FIX bug with not displaying anything
      // if (pos.x > size.x) continue;
      if (pos.y >= size.y - vStep.second.y) break;

      if (pos.x < size.x && pos.x + token->lexeme.size() * vStep.second.x > size.x) {
        GameEngine->DrawString(pos, token->lexeme.substr(0, (size.x - pos.x) / vStep.second.x), token->color);
      } else GameEngine->DrawString(pos, token->lexeme, token->color);
    }


    pos = (absolute + (cursor - vStartAt.second) * vStep.second + vOffset.second) / vStep.second; 

    std::stringstream ss;
    for (int32_t i = 0, max = size.y / vStep.second.y; i < max; i++) {
      ss.str(""); ss << cursor.y - pos.y + i + 1; auto str = ss.str();
      auto line = olc::vi2d(absolute.x + vOffset.second.x - str.size() * vStep.second.x, (i + 1) * vStep.second.y);

      if (cursor.y - vStartAt.second.y == i) {
        GameEngine->DrawString(line, str, ~AnyType<GREY, ColorT>::GetValue());
      } else GameEngine->DrawString(line, str, ~AnyType<DARK_GREY, ColorT>::GetValue());
    }
  }

  inline void Preprocess(Int2Type<NORMAL>) {
    if (pos.y - vStartAt.first.y >= pages.y && pos.y < (((int32_t)memory.size() - 1) / pages.x)) vStartAt.first.y += pages.y;
    if (pos.y - vStartAt.first.y < 0 && pos.y) vStartAt.first.y -= pages.y;
  }

  inline void Preprocess(Int2Type<DISASSEMBLE>) {
    if (cursor.y - vStartAt.second.y + 3 >  (size.y + vOffset.second.y) / vStep.second.y && cursor.y < LINES_SIZE) vStartAt.second.y++;
    if ((cursor.y - vStartAt.second.y - 1 < 0 && cursor.y) || (!cursor.y && cursor.y != vStartAt.second.y)) vStartAt.second.y--;
  }

public:
  template<int32_t T>
  inline void Command(Int2Type<T>) {}

  inline void Command(Int2Type<Editor::VimT::CMD_C>) { mode = mode == NORMAL ? CHARACTER : NORMAL; }
  inline void Command(Int2Type<Editor::VimT::CMD_r>) { mode = REPLACE; Command(Int2Type<Editor::VimT::CMD_x>()); }
  inline void Command(Int2Type<Editor::VimT::CMD_x>) { memory[index()] = 0x00; }
  inline void Command(Int2Type<Editor::VimT::CMD_yy>) { buffer = { memory[index()] }; }
  inline void Command(Int2Type<Editor::VimT::CMD_e>) { Command(Int2Type<Editor::VimT::CMD_b>()); }
  inline void Command(Int2Type<Editor::VimT::CMD_a>) { Command(Int2Type<Editor::VimT::CMD_w>()); Command(Int2Type<Editor::VimT::CMD_i>()); }
  inline void Command(Int2Type<Editor::VimT::CMD_c>) { Command(Int2Type<Editor::VimT::CMD_d>()); Command(Int2Type<Editor::VimT::CMD_i>()); }

  inline void Command(Int2Type<NORMAL>, Int2Type<Editor::VimT::CMD_0>) { pos.x = 0; }
  inline void Command(Int2Type<NORMAL>, Int2Type<Editor::VimT::CMD_DOLLAR>) { pos.x = pages.x - 1; }
  inline void Command(Int2Type<NORMAL>, Int2Type<Editor::VimT::CMD_gg>) { vStartAt.first.y = vStartAt.second.y = pos.y = pos.x = 0; }
  inline void Command(Int2Type<NORMAL>, Int2Type<Editor::VimT::CMD_G>) { pos.x = pages.x - 1; pos.y = ((int32_t)memory.size() - 1) / pages.x; }

  inline void Command(Int2Type<Editor::VimT::CMD_0>) { Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_0>()); UpdateCursor(); }
  inline void Command(Int2Type<Editor::VimT::CMD_DOLLAR>) { Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_DOLLAR>()); UpdateCursor(); }
  inline void Command(Int2Type<Editor::VimT::CMD_gg>) { Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_gg>()); UpdateCursor(); }
  inline void Command(Int2Type<Editor::VimT::CMD_G>) { Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_G>()); UpdateCursor(); }

  inline void Command(Int2Type<Editor::VimT::CMD_D>) {
    if (mode == DISASSEMBLE) mode = NORMAL;
    else { mode = DISASSEMBLE; UpdateCursor();}
  }

  inline void Command(Int2Type<Editor::VimT::CMD_i>) { 
    for (int32_t i = memory.size() - 1, size = index() + 1; i >= size; i--) memory[i] = memory[i - 1];

    search = { false, { "", 2, true, {} } }; 
    Command(Int2Type<Editor::VimT::CMD_r>()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_gd>)    { AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<MEMORY_SELECT_CALLBACK>(), index()); }

  inline void Command(Int2Type<Editor::VimT::CMD_SEMICOLON>) {
    if (!std::get<0>(search.second).size()) return;
    if (!std::get<0>(search.second).size() > 1) return;

    if (std::get<2>(search.second)) Command(Int2Type<Editor::VimT::CMD_f>());
    else Command(Int2Type<Editor::VimT::CMD_F>());
  }

  inline void Command(Int2Type<Editor::VimT::CMD_COMMA>) {
    if (!std::get<0>(search.second).size()) return;
    if (!std::get<0>(search.second).size() > 1) return;

    if (std::get<2>(search.second)) Command(Int2Type<Editor::VimT::CMD_F>());
    else Command(Int2Type<Editor::VimT::CMD_f>());
  }

  inline void Command(Int2Type<Editor::VimT::CMD_f>) { 
    if (!std::get<0>(search.second).size()) return;
    if (!isHex(std::get<0>(search.second).front())) return;
    if (search.first) std::get<2>(search.second) = true;

    uint8_t digit = std::stoul(std::get<0>(search.second), nullptr, 16);
    for (int32_t i = index() + 1; i < memory.size(); i++) {
      if (digit != memory[i]) continue;

      pos.x = i & (pages.x - 1); pos.y = i / pages.x;
      break;
    }
  }

  inline void Command(Int2Type<Editor::VimT::CMD_F>) { 
    if (!std::get<0>(search.second).size()) return;
    if (!isHex(std::get<0>(search.second).front())) return;
    if (search.first) std::get<2>(search.second) = false;

    uint8_t digit = std::stoul(std::get<0>(search.second), nullptr, 16);
    for (int32_t i = index() - 1; i >= 0; i--) {
      if (digit != memory[i]) continue;
      pos.x = i & (pages.x - 1); pos.y = i / pages.x;
    }
  }

  inline void Command(Int2Type<Editor::VimT::CMD_n>) {
    if (!std::get<0>(search.second).size()) return;

    if (std::get<2>(search.second)) Command(Int2Type<Editor::VimT::CMD_SLASH>());
    else Command(Int2Type<Editor::VimT::CMD_QUESTION>());

    pos = std::get<3>(search.second);
  }

  inline void Command(Int2Type<Editor:: VimT::CMD_N>) {
    if (!std::get<0>(search.second).size()) return;

    if (std::get<2>(search.second)) Command(Int2Type<Editor::VimT::CMD_QUESTION>());
    else Command(Int2Type<Editor::VimT::CMD_SLASH>());

    pos = std::get<3>(search.second);
  }

  inline void Command(Int2Type<Editor::VimT::CMD_SLASH>) {
    if (!std::get<0>(search.second).size()) return;
    if (search.first) std::get<2>(search.second) = true;

    std::vector<int32_t> foundAt = {};
    uint8_t digit = std::stoul(std::get<0>(search.second), nullptr, 16);

    for (int32_t i = 0, offset = 0; i < memory.size(); i++) {
      if (memory[i] != digit) continue;

      if (i - index() - 1 < 0) foundAt.push_back(i);
      else foundAt.insert(foundAt.begin() + offset++, i);
    }

    if (!foundAt.size()) return;
    int32_t index = foundAt.front();
    std::get<3>(search.second) = olc::vi2d(index & (pages.x - 1), index / pages.x);
  }

  inline void Command(Int2Type<Editor::VimT::CMD_QUESTION>) {
    if (!std::get<0>(search.second).size()) return;
    if (search.first) std::get<2>(search.second) = false;

    std::vector<int32_t> foundAt = {};
    uint8_t digit = std::stoul(std::get<0>(search.second), nullptr, 16);

    for (int32_t i = 0, offset = 0; i < memory.size(); i++) {
      if (memory[i] != digit) continue;

      if (index() - 1 - i > 0) foundAt.insert(foundAt.begin(), i);
      else foundAt.insert(foundAt.end() - offset++, i);
    }

    if (!foundAt.size()) return;
    int32_t index = foundAt.front();
    std::get<3>(search.second) = olc::vi2d(index & (pages.x - 1), index / pages.x);
  }

  inline void Command(Int2Type<Editor::VimT::CMD_P>) { 
    int32_t offset = buffer.size(); if (!offset) return;

    for (int32_t i = memory.size() - 1, size = index() + offset; i >= size; i--) memory[i] = memory[i - offset];
    for (int32_t i = 0, j = index(); i < offset; i++) memory[j + i] = buffer[i];
  }

  inline void Command(Int2Type<Editor::VimT::CMD_p>) { 
    if (!buffer.size()) return;

    Command(Int2Type<Editor::VimT::CMD_w>()); 
    Command(Int2Type<Editor::VimT::CMD_P>()); 

    for (int32_t i = 0; i < buffer.size() - 1; i++) Command(Int2Type<Editor::VimT::CMD_w>()); 
  }


  inline void Command(Int2Type<Editor::VimT::CMD_dd>) { 
    Command(Int2Type<Editor::VimT::CMD_yy>()); 

    for (int32_t i = index(); i < memory.size() - 1; i++) memory[i] = memory[i + 1];
    memory[memory.size() - 1] = 0x00;
  }

  inline void Command(Int2Type<Editor::VimT::CMD_y>) { 
    auto diff = index(AnyType<-1, olc::vi2d>::GetValue()) - index(); if (diff == 0) return;
    auto size = (int32_t)memory.size(); buffer = {};

    for (int32_t i = 0, j = index(); diff > 0 && i < diff; i++) buffer.push_back(memory[i + j]);
    for (int32_t i = 0, j = index(); diff < 0 && i < std::abs(diff); i++) buffer.push_back(memory[i + j + diff]);

    if (diff < 0) pos = AnyType<-1, olc::vi2d>::GetValue();
  }
  
  inline void Command(Int2Type<Editor::VimT::CMD_d>) { 
    auto diff = index(AnyType<-1, olc::vi2d>::GetValue()) - index(); if (diff == 0) return;
    auto size = (int32_t)memory.size(); buffer = {};

    for (int32_t i = 0, j = index(); diff > 0 && i < diff; i++) buffer.push_back(memory[i + j]);
    for (int32_t i = 0, j = index(); diff < 0 && i < std::abs(diff); i++) buffer.push_back(memory[i + j + diff]);

    for (int32_t i = index(); diff > 0 && i < size - diff; i++) memory[i] = memory[i + diff];
    for (int32_t i = index() + diff; diff < 0 && i < size + diff; i++) memory[i] = memory[i - diff];

    for (int32_t i = 0; i < std::abs(diff); i++) memory[size - 1 - std::abs(diff) + i] = 0x00;
    if (diff < 0) pos = AnyType<-1, olc::vi2d>::GetValue();
  }

  // TODO: Impl undo/redo !!
  inline void Command(Int2Type<Editor::VimT::CMD_u>) { }
  inline void Command(Int2Type<Editor::VimT::CMD_U>) { }

  inline void Command(Int2Type<Editor::VimT::CMD_w>) { 
    if (pos.x < (pages.x - 1)) return Command(Int2Type<Editor::VimT::CMD_l>());
    Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_0>()); Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_j>());
    UpdateCursor();
  }

  inline void Command(Int2Type<Editor::VimT::CMD_b>) { 
    if (pos.x > 0x00) return Command(Int2Type<Editor::VimT::CMD_h>());
    Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_DOLLAR>()); Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_k>());
    UpdateCursor();
  }

  inline void Command(Int2Type<Editor::VimT::CMD_CTRL_u>) {
    for (int32_t i = 0; i < 20; i++) Command(Int2Type<Editor::VimT::CMD_k>());
  }
  
  inline void Command(Int2Type<Editor::VimT::CMD_CTRL_d>) {
    for (int32_t i = 0; i < 20; i++) Command(Int2Type<Editor::VimT::CMD_j>());
  }


  inline void Command(Int2Type<NORMAL>, Int2Type<Editor::VimT::CMD_j>) {
    if (pos.y < ((int32_t)memory.size() - 1) / pages.x) pos.y++;
  }

  inline void Command(Int2Type<Editor::VimT::CMD_j>) {
    switch (mode) {
      case NORMAL:
      case CHARACTER:
        Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_j>());
        break;
    
      case DISASSEMBLE: {
        const int32_t posAt = index();
        int32_t next = memory.size() - 1;

        for (auto& addr : dasm.second) {
          if (addr.first > posAt && addr.first < next) next = addr.first;
        }

        Index2Pos(next); cursor.y = dasm.second[next];
        break;
      }
    }
  }


  inline void Command(Int2Type<NORMAL>, Int2Type<Editor::VimT::CMD_k>) {
    if (pos.y > 0x00) pos.y--;
  }

  inline void Command(Int2Type<Editor::VimT::CMD_k>) {
    switch (mode) {
      case NORMAL:
      case CHARACTER:
        Command(Int2Type<NORMAL>(), Int2Type<Editor::VimT::CMD_k>());
        break;
    
      case DISASSEMBLE: {
        const int32_t posAt = index();
        int32_t next = 0;

        for (auto& addr : dasm.second) {
          if (addr.first < posAt && addr.first > next) next = addr.first;
        }

        Index2Pos(next); cursor.y = dasm.second[next];
        break;
      }
    }
  }

  inline void Command(Int2Type<Editor::VimT::CMD_l>) {
    if (pos.x < (pages.x - 1)) pos.x++;
    UpdateCursor();
  }

  inline void Command(Int2Type<Editor::VimT::CMD_h>) {
    if (pos.x > 0x00) pos.x--;
    UpdateCursor();
  }


private:
  void Process(Int2Type<REPLACE>, PixelGameEngine* GameEngine) {
    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<Editor::KeyEvent, Memory>::Process(this);
  }

  void Process(Int2Type<NORMAL>, PixelGameEngine* GameEngine) {
    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<Editor::KeyEvent, Memory>::Process(this);

    if (!bUpdated) return;
    else bUpdated = false;
    
    #ifdef DEBUG_MODE
    printf("Memory: '%s'\n", cmd.c_str());
    #endif

    if (search.first) {
      // TODO: ADD ability to edit this string
      std::get<0>(search.second).push_back(cmd.back());
      // if (std::get<0>(search.second).size() == std::get<1>(search.second)) 
      return reset(); 
    }

    if (nCurr == 0) {
      if (match<6>({ 'i', 'a', 'C', 'D',  'u', 'U' })) { phrase(peekPrev()); return reset(); };
      if (isDigit(peek()) && peek() != '0') { nCurr++; return; }

      if (cmd.size() > 1 && match<1>({ '^' })) {
        if (peek() == 'd') { phrase(Int2Type<Editor::VimT::CMD_CTRL_d>(), false); return reset(); }
        if (peek() == 'u') { phrase(Int2Type<Editor::VimT::CMD_CTRL_u>(), false); return reset(); }

        error(peek(), "Invalid operation");
      }

    } else {
      uint8_t mask = ((uint8_t)isDigit(peekPrev()) << 1) + (uint8_t)isDigit(peek());

      switch(mask) {
        case 0b11: nCurr++; return;
        case 0b01: reset(false); return;
      }
    }


    if (match<21>({ 'h', 'j', 'k', 'l', 'x', 'r', 'w', 'b', 'e', '0', '$', 'G', '/', '?', 'n', 'N', 'f', 'F', 'r', ',', ';' })) {
      switch (peekPrev()) {
        case 'g': search = { true, { "", 1, true, {} } }; return;
        case 'r': search = { false, { "", 2, true, {} } }; break;
        case 'f': case 'F': search = { true, { "", 1, true, {} } }; break;
        case '/': case '?': search = { true, { "", -1, true, {} } }; break;
      }

      // (noun * number) + verb
      phrase(peekPrev()); number(); verb(peek(nCurr - 2)); 

      return reset();
    }

    // adverb
    if (match<2>({ 'p', 'P' })) { phrase(peekPrev()); number(); return reset(); }
    if (peekPrev() == 'd' && match<1>({ 'd' })) { phrase(Int2Type<Editor::VimT::CMD_dd>(), false); number(); return reset(); } 
    if (peekPrev() == 'y' && match<1>({ 'y' })) { phrase(Int2Type<Editor::VimT::CMD_yy>(), false); number(); return reset(); } 
    if (peekPrev() == 'g' && match<1>({ 'd' })) { phrase(Int2Type<Editor::VimT::CMD_gd>(), false); return reset(); } 
    if (peekPrev() == 'g' && match<1>({ 'g' })) { phrase(Int2Type<Editor::VimT::CMD_gg>(), true, true); number();  verb(peek(nCurr - 3)); return reset(); } 

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
    bSync = bSync || foreach<SyncMemoryCommands, AnyType<-1, int32_t>>::Has();
    
    auto operation = lambda;
    if (locked && foreach<LockedMemoryCommands, AnyType<-1, int32_t>>::Has()) lambda = []() {};
    else lambda = [=]() { operation(); AnyType<-1, int32_t>::GetValue() = c; foreach<MemoryCommands, Memory>::Command(this); };
  }

  template<int32_t T>
  inline void phrase(Int2Type<T> val, bool isSync = true, bool forced = false) {
    bSync = bSync || isSync;

    auto operation = lambda;
    if (locked && !forced) lambda = []() {};
    else lambda = [=]() { operation(); Command(val); };
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

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::UP>,    Int2Type<U>>) { if (AnyType<-1, PixelGameEngine*>::GetValue()->GetKey(olc::Key::UP).bPressed) Command(Int2Type<Editor::VimT::CMD_k>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::DOWN>,  Int2Type<U>>) { if (AnyType<-1, PixelGameEngine*>::GetValue()->GetKey(olc::Key::DOWN).bPressed) Command(Int2Type<Editor::VimT::CMD_j>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::LEFT>,  Int2Type<U>>) { if (AnyType<-1, PixelGameEngine*>::GetValue()->GetKey(olc::Key::LEFT).bPressed) Command(Int2Type<Editor::VimT::CMD_h>()); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::RIGHT>, Int2Type<U>>) { if (AnyType<-1, PixelGameEngine*>::GetValue()->GetKey(olc::Key::RIGHT).bPressed) Command(Int2Type<Editor::VimT::CMD_l>()); }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::DEL).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case DISASSEMBLE: 
      case CHARACTER:
      case NORMAL: reset(false); break;

      case REPLACE: memory[index()] = 0x00; std::get<0>(search.second) = ""; break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::BACK).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case CHARACTER: 
      case DISASSEMBLE: 
      case NORMAL: reset(false); break;

      case REPLACE: 
        memory[index()] = (memory[index()] / pages.x) & 0xF0;
        if (std::get<0>(search.second).size()) std::get<0>(search.second).pop_back();
        break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::ENTER).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case NORMAL: 
      case CHARACTER:
        if (search.first) {
          std::get<1>(search.second) = std::get<0>(search.second).size();
          pos = std::get<3>(search.second); UpdateCursor();
        }

        reset(search.first);
        break;

      case DISASSEMBLE: 
      case REPLACE: mode = NORMAL; break;
    }
  }

  template<int32_t U>
  void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::ESCAPE).bPressed) return;

    bUpdated = true;

    switch (mode) {
      case CHARACTER:
      case DISASSEMBLE:
      case NORMAL: search.first = false; reset(false);
      case REPLACE: mode = NORMAL; break;
    }
  }

  inline void Move2Addr(uint32_t addr) { Index2Pos(addr); UpdateCursor(); }
  inline void MoveTo(olc::vi2d offset) {
    for (int32_t i = 0; i < std::abs(offset.y); i++) {
      if (offset.y > 0) Command(Int2Type<Editor::VimT::CMD_j>());
      else Command(Int2Type<Editor::VimT::CMD_k>());
    }

    for (int32_t i = 0; i < std::abs(offset.x); i++) {
      if (offset.x > 0) Command(Int2Type<Editor::VimT::CMD_l>());
      else Command(Int2Type<Editor::VimT::CMD_h>());
    }
  }

  inline std::string GetMode() {
    switch (mode) {
      case REPLACE: return "-- REPLACE --";
      case DISASSEMBLE: return "-- DISASSEMBLE --";
      case CHARACTER: 
        if (search.first && (cmd.front() == '/' || cmd.front() == '?')) return cmd;
        return "-- CHARACTER --";

      case NORMAL: 
        if (search.first && (cmd.front() == '/' || cmd.front() == '?')) return cmd;
        return "-- NORMAL --";
    }

    return "";
  }

  inline std::string GetCmd() { 
    if (search.first && (cmd.front() == '/' || cmd.front() == '?')) return "";
    return cmd;
  }

private:
  inline void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    bool bPressed = GameEngine->GetKey(key).bPressed;

    if (GameEngine->GetKey(key).bReleased) fStrokeRepeat = .0f;
    if (GameEngine->GetKey(key).bHeld) {
      fStrokeRepeat += AnyType<-1, float>::GetValue();
      if (fStrokeRepeat >= 0.3f) { fStrokeRepeat = .2f; bPressed = true; }
    }

    if (!bPressed) return;
    const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
    const bool isCtrl = GameEngine->GetKey(olc::Key::CTRL).bHeld;
    const char c = toUpper ? upper : lower;

    bUpdated = true;

    switch (mode) {
      case DISASSEMBLE: cmd += isCtrl ? "^" : "";
      case CHARACTER: case NORMAL: cmd += std::string(1, c); break;

      case REPLACE: {
        if (!isHex(c)) { memory[index()] = c & 0xFF; mode = NORMAL; break; }

        memory[index()] = ((memory[index()] & 0x0F) * pages.x) | Hex2Int(c);

        std::get<0>(search.second).push_back(c);
        if (std::get<0>(search.second).size() == std::get<1>(search.second)) mode = NORMAL;

        break;
      }
    }
  }

  void UpdateCursor() {
    const int32_t posAt = index(); cursor.y = 0;

    for (auto& addr : dasm.second) {
      if (addr.first <= posAt && addr.first >= cursor.y) cursor.y = addr.first;
    }

    cursor.y = dasm.second[cursor.y];
  }

  template<int32_t T = Editor::VimT::CMD_NONE>
  void number(Int2Type<T> val = Int2Type<T>()) {
    int32_t nCurr = 0; while (isDigit(peek(nCurr))) nCurr++;
    if (nCurr == 0) return;

    auto len = std::stoul(cmd.substr(0, nCurr)); if (len == 0) return;

    auto prev = lambda;
    lambda = [=]() { for (int32_t i = 0; i < len; i++) { if (i) Command(val); prev(); } };
  }

  inline int32_t index() { return index(pos.x, pos.y); }
  inline int32_t index(olc::vi2d pos) { return index(pos.x, pos.y); }
  inline int32_t index(int32_t x, int32_t y) { return (y * pages.x) | x; }
  inline void Index2Pos(int32_t index) { pos.y = index / pages.x; pos.x = index - (pos.y * pages.x); }

  inline int32_t Line2Index(int32_t line) {
    for (auto& addr : dasm.second) if (addr.second == line) return addr.first;
    return dasm.second.begin()->first;
  }

  inline uint8_t Hex2Int(const char c) { 
    if (isDigit(c)) return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 0x0A;
    if (c >= 'A' && c <= 'F') return c - 'A' + 0x0A;
    return 0;
  }

  inline std::string Int2Hex(int32_t i, int32_t width = 2) { 
    std::stringstream ss; ss << std::setfill('0') << std::setw(width) << std::hex << std::uppercase << +i;
    return ss.str();
  }


private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d pages = olc::vi2d(0x10, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  std::pair<olc::vi2d, olc::vi2d> vStartAt = std::pair(olc::vi2d(0, 0), olc::vi2d(0, 0));
  const std::pair<olc::vi2d, olc::vi2d> vStep = std::pair(olc::vi2d(20, 12), olc::vi2d(8, 12));
  const std::pair<olc::vi2d, olc::vi2d> vOffset = std::pair(olc::vi2d(44, 12), olc::vi2d(24, 0));

  ModeT mode = NORMAL;
  bool locked = false;

  std::vector<uint8_t> buffer = { }; 
  

  // Variables defines animation duration
  float fStrokeRepeat = 0.f;

  olc::vi2d pos = olc::vi2d(0, 0);
  olc::vi2d cursor = olc::vi2d(0, 0);

  std::array<uint8_t, SizeT> memory;

  Bus* bus;

  int32_t LINES_SIZE = 0;
  DisassembleT dasm;
  Interpreter::Lexer lexer;
};

};
