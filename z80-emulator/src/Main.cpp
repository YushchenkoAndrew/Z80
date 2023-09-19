#include <stdio.h>
#include <fstream>

#define CMD_PRINT
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
    std::ifstream f("../RTC_Test/Test.asm");
    std::stringstream buffer;
    buffer << f.rdbuf();
    f.close();

    std::ofstream fASM("Test.asm");
    // std::string s = buffer.str();

    // mMinecraft.Init(ScreenHeight(), ScreenWidth(), luaConfig);
    auto editor = std::make_shared<Editor::Editor>();
    editor->temp(buffer.str());


    // Interpreter::Lexer lexer = Interpreter::Lexer(buffer.str());
    Interpreter::Interpreter interpreter = Interpreter::Interpreter();

    bool err = interpreter.scan(buffer.str());
    // // for (auto token : interpreter.parser.lexer.vTokens) { token->print(); }

    // if (err) {
    //   printf("HAS AN ERROR %ld\n", emulator.interpreter.errors.size());

    //   for (auto err : emulator.interpreter.errors) {
    //     printf("%s", err.c_str());
    //   }
    // } else {
    //   emulator.ROM.load(emulator.interpreter.env.memory);
    // }

    auto bus = Bus::Bus();

    auto rom = bus.W27C512;
    // auto rom = std::make_shared<Bus::Memory<Bus::EEPROM, 65536>>(8);
    rom->load(interpreter.env.memory);


    interpreter.env.save("out.bin");
    fASM << bus.Z80->Disassemble() << "\n";
    fASM.close();
    // emulator.editor.size = {  };


    auto lines = std::make_shared<Window::Lines>();
    lines->lines = interpreter.errors;


    auto offset = 200;
    panels = {
      Panel(
        std::tuple(true, editor, std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth() - offset, ScreenHeight())))
        ,
        std::tuple(false, rom,  std::pair(olc::vi2d(ScreenWidth() - offset, 0), olc::vi2d(ScreenWidth(), ScreenHeight())))
        // ,
        // std::tuple(true, lines,  std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())))
      )
    };


    panels[nPanel].Initialize();
    // Panel::Panel p = Panel::Panel(std::make_shared<Editor::Editor>(emulator.editor));

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
	  Clear(~AnyType<Colors::BLACK, ColorT>::GetValue());
    AnyType<-1, float>::GetValue() = fElapsedTime;

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

  void Event(Int2Type<EDITOR_CALLBACK>) override { std::cout << "EDITOR_CALLBACK\n"; }
  void Event(Int2Type<MEMORY_CALLBACK>) override { std::cout << "MEMORY_CALLBACK\n"; }

  void Event(Int2Type<PANEL_SELECT_CALLBACK>,  int32_t index)  override { std::cout << "PANEL_SELECT_CALLBACK " << index << "\n" ; }
  void Event(Int2Type<MEMORY_SELECT_CALLBACK>, int32_t index) override { std::cout << "MEMORY_SELECT_CALLBACK " << index << "\n"; }


private:
  int32_t nPanel = 0;
  std::array<Panel, 1> panels;

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
