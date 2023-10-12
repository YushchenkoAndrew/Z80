#include <stdio.h>
#include <fstream>

#define DEBUG_MODE
#include "Panel.h"

// Check Memory Leaking
//#define MEM_TRACK
//#include "include/MemTrack.h"


class App : public PixelGameEngine {
private:
  typedef std::pair<uint32_t, Interpreter::RealtiveToken> RelativeAddr;

public:
  App(LuaScript& config): luaConfig(config) {
    sAppName = "Z80 Emulator";
    AnyType<-1, PixelGameEngine*>::GetValue() = this;
  }

  ~App() { if (offload != nullptr && offload->joinable()) offload->join(); }

  bool OnUserCreate() override {
    // std::ifstream f("../RTC_Test/Test.asm");
    // std::ifstream f(file);
    // std::stringstream buffer;
    // buffer << f.rdbuf();
    // f.close();

    // std::string s = buffer.str();

    // mMinecraft.Init(ScreenHeight(), ScreenWidth(), luaConfig);

    // TODO: Impl terminal
    auto terminal = std::make_shared<Window::Terminal>();

    // Interpreter::Lexer lexer = Interpreter::Lexer(buffer.str());
    // if (bool err = interpreter.Load("assets/SevenSegmentDisplay/Test.asm")) {
    if (bool err = interpreter.Load("assets/TestKeyboard/Test.asm")) {
      for (auto token : interpreter.parser.lexer.tokens) { token->print(); }
      printf("\n");

      for (auto err : interpreter.errors) {
        printf("%s", err.c_str());
      }

      printf("\nHAS AN ERROR %ld\n", interpreter.errors.size());

      terminal->PushOut(interpreter.errors);
    } else terminal->PushOut("OK");

    //  else {
    //   emulator.ROM.load(emulator.interpreter.env.memory);
    // }

    auto editor = std::make_shared<Editor::Editor>();
    for (auto f : interpreter.env.files()) editor->Open(f);
    editor->Open(interpreter.filepath);


    // auto rom = bus->W27C512;
    // auto rom = std::make_shared<Bus::Memory<Bus::EEPROM, 65536>>(8);
    bus->W27C512->load(interpreter.env.memory);

    // bus->hexDisplay->Write(0, 0x79, false); bus->hexDisplay->Write(0, 0x24, false);


    // std::ofstream f("Test.asm");
    // interpreter.env.save("out.bin");
    // f << bus->Disassemble().first << "\n";
    // f.close();
    // emulator.editor.size = {  };


    // // lines->lines = interpreter.errors;
    // lines->lines.push_back("Success !!");

    // auto offset = 210;
    olc::vi2d zero = olc::vi2d(0, 0);
    olc::vi2d size = olc::vi2d(ScreenWidth(), ScreenHeight() - vStep.y);
    olc::vi2d window = olc::vi2d(size.x * 7 / 10, size.y * 9 / 10);

    panels = {
      Panel(
        // TODO: Load windows size from lua
        std::tuple(editor,       std::pair(olc::vi2d(zero.x,   zero.y),              olc::vi2d(window.x,          window.y))),
        std::tuple(bus->W27C512, std::pair(olc::vi2d(window.x, (int)zero.y),         olc::vi2d(size.x - window.x, (int)size.y * 3 / 4))),
        std::tuple(bus->IMS1423, std::pair(olc::vi2d(window.x, (int)size.y * 3 / 4), olc::vi2d(size.x - window.x, (int)size.y / 4))),
        std::tuple(terminal,     std::pair(olc::vi2d(zero.x,   window.y),            olc::vi2d(window.x,           size.y - window.y)))
      ),
      Panel(
        std::tuple(bus,  std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())))
      )
    };

    panels[nPanel].Initialize(std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())));
    // Panel::Panel p = Panel::Panel(std::make_shared<Editor::Editor>(emulator.editor));

    offload = std::make_unique<std::thread>([&]() {
      bus->W27C512->Disassemble(); 
      for (auto& p : panels) p.Preinitialize();
      
      bSyncing.first = false;
    });

    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override {
	  Clear(*AnyType<Colors::BLACK, ColorT>::GetValue());
    AnyType<-1, float>::GetValue() = fElapsedTime;

    panels[nPanel].Preprocess();
    panels[nPanel].Process(this); 
    panels[nPanel].Draw(this); 

    Process();
    Draw();

    // AnyType<-2, float>::GetValue() += fElapsedTime;

    // emulator.ROM.Draw(this);

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
    return bExec;
  }

  void Process() {
    const auto mouse = GetMousePos();
    const bool bPressed = GetMouse(0).bPressed;

    const auto nWidth = ScreenWidth();
    const auto nHeight = ScreenHeight();

    if (bPressed && mouse.x > 0 && mouse.y > nHeight - vStep.y && mouse.x < nWidth && mouse.y < nHeight) {
      auto pos = mouse / vStep;

      for (int32_t i = 1, acc = 0; i <= panels.size(); acc += (int32_t)GetPanelName(i++).size() + 1) {
        if (pos.x > acc && pos.x < acc + (int32_t)GetPanelName(i).size() + 1) return Event(Int2Type<PANEL_SELECT_CALLBACK>(), i);
      }
    }
  }

  void Draw() {
    const auto nWidth = ScreenWidth();
    const auto nHeight = ScreenHeight();

    olc::vi2d pos = olc::vi2d(0, nHeight - vStep.y);
    FillRect(pos - olc::vi2d(0, 2), olc::vi2d(nWidth, 10), *AnyType<Colors::VERY_DARK_GREY, ColorT>::GetValue());

    for (int32_t i = 0; i < panels.size(); i++) {
      auto str = GetPanelName(i + 1);
      auto color = i == nPanel ? AnyType<Colors::GREY, ColorT>::GetValue() : AnyType<Colors::DARK_GREY, ColorT>::GetValue();

      DrawString(pos + olc::vi2d(vStep.x, 0), str, *color);
      pos.x += str.size() * vStep.x + vStep.x;
    }

    std::string name = GetMode() == NORMAL ? "NORMAL" : "DEBUG";
    pos.x = nWidth - ((int32_t)name.size() + 2) * vStep.x;
    DrawString(pos, name, *AnyType<Colors::DARK_GREY, ColorT>::GetValue());

    if (!bSyncing.first) return;
    pos.x -= ((int32_t)bSyncing.second.size() + 2) * vStep.x;
    DrawString(pos, bSyncing.second, *AnyType<Colors::DARK_GREY, ColorT>::GetValue());
  }

  ModeT GetMode() override { return bus->Z80->IsDebug() ? DEBUG : NORMAL; }

  void Event(Int2Type<NEW_DEBUG_MODE_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "NEW_DEBUG_MODE_CALLBACK\n";
    #endif

    Event(Int2Type<ATTACH_DEBUG_MODE_CALLBACK>());
    Event(Int2Type<DEBUG_RESET_CALLBACK>());
  }

  void Event(Int2Type<ATTACH_DEBUG_MODE_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "ATTACH_DEBUG_MODE_CALLBACK\n";
    #endif

    bus->Z80->Debug();
    for (auto& p : panels) p.Lock();
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

    RelativeAddr next = Addr2Token(bus->Z80->Addr());
    if (next.second.second == nullptr) return;

    Panel& p = panels[nPanel];
    if (p.Editor() != nullptr) p.Editor()->Open(next.second.first)->MoveTo(olc::vi2d(next.second.second->col - 1, next.second.second->line - 1));
    if (p.EEPROM() != nullptr) p.EEPROM()->Move2Addr(next.first);
    if (p.Stack() != nullptr)  p.Stack()->Move2Addr(bus->Z80->Stack());
  }

  void Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "DETACH_DEBUG_MODE_CALLBACK\n";
    #endif

    bus->Z80->Normal();
    for (auto& p : panels) p.Unlock();
  }

  void Event(Int2Type<EDITOR_SELECT_CALLBACK>, olc::vi2d cursor) override {
    #ifdef DEBUG_MODE 
    std::cout << "EDITOR_SELECT_CALLBACK [Ln " << cursor.y << ", Col " << cursor.x << "]\n";
    #endif

    if (panels[nPanel].Editor() == nullptr) return;

    const RelativeAddr next = Pos2Token(cursor, panels[nPanel].Editor()->File());
    if (next.second.second == nullptr || panels[nPanel].EEPROM() == nullptr) return;
    panels[nPanel].EEPROM()->Move2Addr(next.first);
  }

  void Event(Int2Type<EDITOR_SELECT_LINE_CALLBACK>, olc::vi2d cursor) override {
    #ifdef DEBUG_MODE 
    std::cout << "EDITOR_SELECT_LINE_CALLBACK [Ln " << cursor.y << ", Col " << cursor.x << "]\n";
    #endif

    if (panels[nPanel].Editor() == nullptr) return;

    const RelativeAddr next = Pos2Token(cursor, panels[nPanel].Editor()->File());
    if (next.second.second == nullptr) return;
    std::cout << next.second.second->line << std::endl;

    bus->Z80->SetBreakpoint((uint16_t)next.first);
    panels[nPanel].Editor()->SelectLine((int32_t)next.second.second->line - 1);
  }

  void Event(Int2Type<MEMORY_SELECT_CALLBACK>, int32_t index) override { 
    #ifdef DEBUG_MODE 
    std::cout << "MEMORY_SELECT_CALLBACK [Addr " <<  std::setfill('0') << std::setw(5) << std::hex << std::uppercase << index << "]\n";
    #endif

    const RelativeAddr next = Addr2Token(index);
    if (next.second.second == nullptr || panels[nPanel].Editor() == nullptr) return;
    panels[nPanel].Editor()->Open(next.second.first)->MoveTo(olc::vi2d(next.second.second->col - 1, next.second.second->line - 1));
  }

  void Event(Int2Type<PANEL_SELECT_CALLBACK>,  int32_t index) override {
    #ifdef DEBUG_MODE 
    std::cout << "PANEL_SELECT_CALLBACK " << index << "\n" ;
    #endif

    if (index - 1 < 0 || index - 1>= panels.size()) return;
    panels[nPanel = index - 1].Initialize(std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight())));
  }

  void Event(Int2Type<PROGRAM_EXIT>) override {
    #ifdef DEBUG_MODE 
    std::cout << "PROGRAM_EXIT\n";
    #endif
    bExec = false;
  }

private:
  inline RelativeAddr Addr2Token(uint32_t addr) {
    RelativeAddr next = std::pair(0, std::pair("", nullptr));

    for (auto& pos : interpreter.env.tokens) {
      if (pos.first <= addr) next = pos;
      else return next;
    }

    return next;
  }

  inline RelativeAddr Pos2Token(olc::vi2d cursor, std::string filename) {
    RelativeAddr next = std::pair(0, std::pair(filename, nullptr));

    for (auto& pos : interpreter.env.tokens) {
      if (next.second.first != pos.second.first) continue;
      if (pos.second.second->line <= cursor.y + 1 && pos.second.second->col <= cursor.x + 1) next = pos;
    }

    return next;
  }

  inline std::string GetPanelName(int32_t i) { return std::to_string(i) + " panel"; }

private:
  const olc::vi2d vOffset = olc::vi2d(0, 0);
  const olc::vi2d vStep = olc::vi2d(8, 12);

  int32_t nPanel = 0;
  std::array<Panel, 2> panels;
  Interpreter::Interpreter interpreter;
  std::shared_ptr<Bus::Bus> bus = std::make_shared<Bus::Bus>();

  std::pair<std::atomic<bool>, const std::string> bSyncing = std::pair(true, "Syncing");
  std::unique_ptr<std::thread> offload = nullptr;

  std::atomic<bool> bExec = true;
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
