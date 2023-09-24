#include <stdio.h>
#include <fstream>

#define DEBUG_MODE
#include "Panel.h"

// Check Memory Leaking
//#define MEM_TRACK
//#include "include/MemTrack.h"


class App : public PixelGameEngine {
public:
  App(LuaScript& config): luaConfig(config) {
    sAppName = "Z80 Emulator";
  }

  bool OnUserCreate() override {
    // std::ifstream f("../RTC_Test/Test.asm");
    std::ifstream f("assets/SevenSegmentDisplay/Test.asm");
    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();

    std::ofstream fASM("Test.asm");
    // std::string s = buffer.str();

    // mMinecraft.Init(ScreenHeight(), ScreenWidth(), luaConfig);
    auto editor = std::make_shared<Editor::Editor>();
    editor->temp(buffer.str());


    // Interpreter::Lexer lexer = Interpreter::Lexer(buffer.str());
    if (bool err = interpreter.scan(buffer.str())) {
      for (auto token : interpreter.parser.lexer.tokens) { token->print(); }
      printf("\n");

      for (auto err : interpreter.errors) {
        printf("%s", err.c_str());
      }

      printf("\nHAS AN ERROR %ld\n", interpreter.errors.size());
    }

    //  else {
    //   emulator.ROM.load(emulator.interpreter.env.memory);
    // }


    // auto rom = bus->W27C512;
    // auto rom = std::make_shared<Bus::Memory<Bus::EEPROM, 65536>>(8);
    bus->W27C512->load(interpreter.env.memory);
    bus->W27C512->Disassemble();

    // bus->hexDisplay->Write(0, 0x79, false); bus->hexDisplay->Write(0, 0x24, false);


    interpreter.env.save("out.bin");
    fASM << bus->Disassemble().first << "\n";
    fASM.close();
    // emulator.editor.size = {  };


    auto lines = std::make_shared<Window::Lines>();
    lines->lines = interpreter.errors;


    auto offset = 210;
    panels = {
      Panel(
        // TODO: Load panes from lua
        std::tuple(editor, std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth() - offset, ScreenHeight())))
        ,
        std::tuple(bus->W27C512,  std::pair(olc::vi2d(ScreenWidth() - offset, 0), olc::vi2d(offset, ScreenHeight())))
        // ,
        // TODO: Display errors
        // std::tuple(true, lines,  std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())))
      ),
      Panel(
        std::tuple(bus,  std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())))
      )
    };

    for (auto& p : panels) p.Preinitialize();
    panels[nPanel].Initialize(std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())));
    // Panel::Panel p = Panel::Panel(std::make_shared<Editor::Editor>(emulator.editor));

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
	  Clear(~AnyType<Colors::BLACK, ColorT>::GetValue());
    AnyType<-1, float>::GetValue() = fElapsedTime;

    panels[nPanel].Preprocess();

    // AnyType<-2, float>::GetValue() += fElapsedTime;
    panels[nPanel].Process(this);


    // emulator.ROM.Draw(this);
    panels[nPanel].Draw(this);


    // TODO: Text editor
    // std::ifstream t("../RTC_Test/Test.asm");
    // std::stringstream buffer;
    // buffer << t.rdbuf();

    // // mMinecraft.Update(*this, fElapsedTime);
    // Interpreter::Lexer lexer = Interpreter::Lexer(buffer.str());
    // if (lexer.scan()) printf("%s\n", lexer.error().c_str());


    // // this->DrawString(10, 10, buffer.str());
    // for (auto t : lexer.vDst) {
    //   this->DrawString(t->col * 8, t->line * 12, t->lexeme, t->color);
    // }


    // // mMinecraft.Draw(*this, fElapsedTime);
    // // return mMinecraft.IsFinished();
    return true;
  }

  ModeT GetMode() override { return bus->Z80->IsDebug() ? DEBUG : NORMAL; }

  void Event(Int2Type<ENTER_DEBUG_MODE_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "ENTER_DEBUG_MODE_CALLBACK\n";
    #endif

    bus->Z80->Debug();

    for (auto& p : panels) p.Lock();
    Event(Int2Type<DEBUG_RESET_CALLBACK>());
  }

  void Event(Int2Type<DEBUG_RESET_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "DEBUG_RESET_CALLBACK\n";
    #endif

    bus->Z80->Reset();
  }


  void Event(Int2Type<NEXT_DEBUG_STEP_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "NEXT_DEBUG_STEP_CALLBACK\n";
    #endif

    if (!bus->Z80->IsDebug()) return;
    else bus->Z80->Step();

    const uint32_t addr = bus->Z80->Addr();
    std::pair<uint32_t, std::shared_ptr<Interpreter::Token>> next = std::pair(0, nullptr);

    for (auto& pos : interpreter.env.tokens) {
      if (
        pos.first <= addr && (
          next.second == nullptr || 
          (pos.second->line >= next.second->line && pos.second->col >= next.second->col)
      )) next = pos;
    }

    if (next.second == nullptr) return;

    Panel& p = panels[nPanel];
    if (p.Editor() != nullptr) p.Editor()->MoveTo(olc::vi2d(next.second->col - 1, next.second->line - 1));
    if (p.Memory() != nullptr) p.Memory()->Move2Addr(next.first);
  }

  void Event(Int2Type<EXIT_DEBUG_MODE_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "EXIT_DEBUG_MODE_CALLBACK\n";
    #endif

    bus->Z80->Normal();
    for (auto& p : panels) p.Unlock();
  }

  void Event(Int2Type<EDITOR_SELECT_CALLBACK>, olc::vi2d cursor) override {
    #ifdef DEBUG_MODE 
    std::cout << "EDITOR_SELECT_CALLBACK [Ln " << cursor.y << ", Col " << cursor.x << "]\n";
    #endif

    std::pair<uint32_t, std::shared_ptr<Interpreter::Token>> next = std::pair(0, nullptr);

    for (auto& pos : interpreter.env.tokens) {
      if (
        pos.second->line <= cursor.y + 1 && pos.second->col <= cursor.x + 1 && (
          next.second == nullptr || 
          (pos.second->line >= next.second->line && pos.second->col >= next.second->col)
      )) next = pos;
    }

    if (next.second == nullptr || panels[nPanel].Memory() == nullptr) return;
    panels[nPanel].Memory()->Move2Addr(next.first);
  }

  void Event(Int2Type<MEMORY_SELECT_CALLBACK>, int32_t index) override { 
    #ifdef DEBUG_MODE 
    std::cout << "MEMORY_SELECT_CALLBACK [Addr " <<  std::setfill('0') << std::setw(5) << std::hex << std::uppercase << index << "]\n";
    #endif

    std::pair<uint32_t, std::shared_ptr<Interpreter::Token>> next = std::pair(0, nullptr);

    for (auto& pos : interpreter.env.tokens) {
      if (
        pos.first <= index && (
          next.second == nullptr || 
          (pos.second->line >= next.second->line && pos.second->col >= next.second->col)
      )) next = pos;
    }

    if (next.second == nullptr || panels[nPanel].Editor() == nullptr) return;
    panels[nPanel].Editor()->MoveTo(olc::vi2d(next.second->col - 1, next.second->line - 1));
  }

  void Event(Int2Type<PANEL_SELECT_CALLBACK>,  int32_t index) override {
    #ifdef DEBUG_MODE 
    std::cout << "PANEL_SELECT_CALLBACK " << index << "\n" ;
    #endif

    if (index - 1 < 0 || index - 1>= panels.size()) return;
    panels[nPanel = index - 1].Initialize(std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())));
  }


private:
  int32_t nPanel = 0;
  std::array<Panel, 2> panels;
  Interpreter::Interpreter interpreter;
  std::shared_ptr<Bus::Bus> bus = std::make_shared<Bus::Bus>();

  LuaScript& luaConfig;
};

int main() {
  Defs::Init(); Interpreter::Defs::Init(); Bus::Defs::Init();  // Initialize types

  LuaScript luaConfig;
  if (!luaConfig.Init("src/lua/Config.lua")) return 0;
  if (!luaConfig.GetTable("Init")) return 0;

  // TODO: Create defs to load colors from lua
  // Defs::Load(&luaConfig);

  const int32_t nScreenWidth = luaConfig.GetTableValue<int32_t>(nullptr, "nScreenWidth");
  const int32_t nScreenHeight = luaConfig.GetTableValue<int32_t>(nullptr, "nScreenHeight");
  const int32_t nPixel = luaConfig.GetTableValue<int32_t>(nullptr, "nPixel");

  App app(luaConfig);
  if (app.Construct(nScreenWidth, nScreenHeight, nPixel, nPixel)) app.Start();
  
  return 0;
}
