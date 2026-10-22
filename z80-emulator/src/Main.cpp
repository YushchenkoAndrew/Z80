#pragma once
#include "src/Bus/Cpu.h"
#include "src/Registry.h"
#include <utility>
#include <vector>
#define DEBUG_MODE

#include "include/LuaScript.h"
#include "lib/olcPixelGameEngine.h"
#include "src/Defs.h"
#include "src/Interpreter/Defs.h"
#include "src/Window/Panel.h"
#include <unordered_set>

// Check Memory Leaking
//#define MEM_TRACK
//#include "include/MemTrack.h"

class App : public PixelGameEngine {
private:
  typedef std::pair<uint32_t, Interpreter::RelativeToken> RelativeAddr;

public:
  App(LuaScript lua): lua(std::move(lua)) { sAppName = "Z80 Emulator"; }
  ~App() { if (sync.second != nullptr && sync.second->joinable()) sync.second->join(); }

  bool OnUserCreate() override {
    Registry::Initialize(this, lua);

    Compile(lua.GetOr("SOURCE", ""));
    Registry::W27C512->Load(Registry::Interpreter->memory);

    for (auto f : Registry::Interpreter->files()) Registry::Editor->Open(f);

    const size_t groups = lua.Length("PANELS");
    for (size_t i = 1; i <= groups; ++i) {
      uint32_t index = panels.size();
      std::string group = "PANELS." + std::to_string(i);
      panels.push_back(Panel(this));

      for (const std::string& name : lua.Keys(group)) {
        const std::string p = group + "." + name; 

        auto instance = Registry::GetInstance(name);
        if (!instance) fprintf(stderr, "panel '%s' in group %zu is invalid\n", name.c_str(), i);
        else panels[index].Push(std::tuple(instance, std::pair(olc::vi2d(lua.GetOr(p + ".x", 0), lua.GetOr(p + ".y", 0)), olc::vi2d(lua.GetOr(p + ".width", 0), lua.GetOr(p + ".height", 0)))));
      }
    }

    panels[nPanel].Select(std::pair(olc::vi2d(0, 0), olc::vi2d(ScreenWidth(), ScreenHeight() - vStep.y - vOffset.y)));

    sync = std::pair(true, std::make_unique<std::thread>([&]() { Registry::W27C512->Disassemble(); sync.first = false; }));
    return true;
  }

  template<int32_t T>
  void Process(Int2Type<T>) {
    if (!vKeyEventListeners.size()) return;
    olc::HWButton btn = this->GetKey((olc::Key)T);

    if (btn.bReleased || (!btn.bHeld && vKeyCombinationHeld.count((olc::Key)T))) vKeyCombinationHeld.erase((olc::Key)T);
    if (!(btn.bPressed || (btn.bHeld && fStrokeRepeat >= .4f ? fStrokeRepeat -= .04f, true : false))) return;

    if (fnStrokeAction != nullptr) { fnStrokeAction((olc::Key)T), fnStrokeAction = nullptr; return; }

    // Insert held keys into vector
    for (const auto key : vKeyCombinationHeld) {
      const auto it = std::find(vKeyCombination.begin(), vKeyCombination.end(), key);
      if (it == vKeyCombination.end()) this->vKeyCombination.insert(vKeyCombination.begin(), key);
    }

    if (btn.bPressed) { this->vKeyCombination.push_back((olc::Key)T); fStrokeRepeat = .0f; }
    if (btn.bHeld) vKeyCombinationHeld.insert((olc::Key)T);

    for (const auto listener : vKeyEventListeners) {
      auto [exist, exec] = listener->Executable(this->vKeyCombination);

      if (exec != nullptr) fnStrokeAction = exec(listener);
      if (exist && exec == nullptr) return;
    }

    this->vKeyCombination.clear();
  }

  bool OnUserUpdate(float fElapsedTime) override {
	  Clear(Color::BLACK);
    // AnyType<-1, float>::GetValue() = fElapsedTime;

    foreach<KeyEvent, void>::Process(this);
    if (vKeyCombinationHeld.size()) fStrokeRepeat += fElapsedTime;
    else fStrokeRepeat = .0f;

    Process(); Draw();

    return bExec;
  }

  void Process() {
    panels[nPanel].Process(); 

    const auto mouse = GetMousePos();
    const bool bPressed = GetMouse(0).bPressed;

    const auto nWidth = ScreenWidth();
    const auto nHeight = ScreenHeight() - vOffset.y;

    if (bPressed && mouse.x > 0 && mouse.y > nHeight - vStep.y && mouse.x < nWidth && mouse.y < nHeight) {
      auto pos = mouse / vStep;

      for (int32_t i = 0, x = 2, length = (int32_t)panels[i].GetName().size() + 2; i < panels.size(); length = (int32_t)panels[i].GetName().size() + (int32_t)std::log10(i++) + 2, x += length + 1) {
        if (pos.x >= x && pos.x <= x + length + 1) return Event(Int2Type<PANEL_SELECT_CALLBACK>(), i + 1);
      }
    }
  }

  void Draw() {
    panels[nPanel].Draw(); 

    const auto nWidth = ScreenWidth();
    const auto nHeight = ScreenHeight();

    olc::vi2d pos = olc::vi2d(0, nHeight - vStep.y - vOffset.y);
    FillRect(pos - olc::vi2d(0, 2), olc::vi2d(nWidth, 10), Color::VERY_DARK_GREY);
    DrawString(pos + olc::vi2d(0, vStep.y), keybindings, Color::DARK_GREY);

    const auto& cmd = Command();
    DrawString(pos + olc::vi2d(nWidth - ((int32_t)cmd.size() + 2) * vStep.x, vStep.y), cmd, Color::DARK_GREY);

    auto color = panels[nPanel].IsActive() ? Color::DARK_YELLOW : Color::DARK_GREEN;
    FillRect(pos - olc::vi2d(0, 2), olc::vi2d(vStep.x * 2, vStep.y - 2), color);
    DrawString(pos + olc::vi2d(vStep.x / 2, 0), std::to_string(nPanel + 1), Color::DARK_GREY);
    pos.x += vStep.x * 2;

    // const auto nCmd = cmd.size();
    // for (int32_t i = 0; !nCmd && i < panels.size(); i++) {
    for (int32_t i = 0; i < panels.size(); i++) {
      auto str = std::to_string(i + 1) + " " + panels[i].GetName();
      auto color = i == nPanel ? Color::GREY : Color::DARK_GREY;

      DrawString(pos + olc::vi2d(vStep.x, 0), str, color);
      pos.x += GetTextSize(str).x + vStep.x * 2;
    }

    // if (nCmd) DrawString(pos + olc::vi2d(vStep.x, 0), cmd, Color::GREY);

    std::string name = GetMode() == Bus::Z80::CPU::ModeT::NORMAL ? "NORMAL" : "DEBUG";
    pos.x = nWidth - ((int32_t)name.size() + 2) * vStep.x;
    color = GetMode() == Bus::Z80::CPU::ModeT::NORMAL ? Color::ORANGE : Color::CYAN;

    FillRect(pos - olc::vi2d(vStep.x / 2, 2), GetTextSize(name) + olc::vi2d(vStep.x, 2), color);
    DrawString(pos, name, Color::DARK_GREY);

    if (!sync.first) return;

    const std::string synchronizing = "Synchronizing... ";
    pos.x = nWidth - synchronizing.size() * vStep.x; pos.y += vStep.y;
    DrawString(pos, synchronizing, Color::DARK_GREY);
  }

  uint32_t GetMode() override { return Registry::Z80->GetMode(); }

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

    if (!Registry::Z80->IsRunning()) return;

    Registry::Z80->Debug();
    // for (auto& p : panels) p.Lock(); // TODO: ????
  }

  void Event(Int2Type<DEBUG_RESET_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "DEBUG_RESET_CALLBACK\n";
    #endif

    if (!Registry::Z80->IsRunning()) return;

    // Wait until cpu will enter debug mode
    Registry::Z80->Addr(); Registry::Z80->Reset();
  }


  void Event(Int2Type<NEXT_DEBUG_STEP_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "NEXT_DEBUG_STEP_CALLBACK\n";
    #endif

    if (!Registry::Z80->IsRunning()) return;
    if (!Registry::Z80->IsDebug()) return;
    else Registry::Z80->Step();

    RelativeAddr next = Addr2Token(Registry::Z80->Addr());
    if (next.second.second == nullptr) return;

    Registry::Editor->Open(next.second.first)->MoveTo(olc::vi2d(next.second.second->col - 1, next.second.second->line - 1));
    Registry::W27C512->Move2Addr(next.first);
    Registry::IMS1423->Move2Addr(Registry::Z80->Stack());
  }

  void Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "DETACH_DEBUG_MODE_CALLBACK\n";
    #endif

    if (!Registry::Z80->IsRunning()) return;

    Registry::Z80->Normal();
    // for (auto& p : panels) p.Unlock(); // TODO: ???
  }

  void Event(Int2Type<EDITOR_SELECT_CALLBACK>, olc::vi2d cursor) override {
    #ifdef DEBUG_MODE 
    std::cout << "EDITOR_SELECT_CALLBACK [Ln " << cursor.y << ", Col " << cursor.x << "]\n";
    #endif

    const RelativeAddr next = Pos2Token(cursor, Registry::Editor->File());
    if (next.second.second != nullptr) Registry::W27C512->Move2Addr(next.first);
  }

  void Event(Int2Type<EDITOR_SELECT_LINE_CALLBACK>, olc::vi2d cursor) override {
    #ifdef DEBUG_MODE 
    std::cout << "EDITOR_SELECT_LINE_CALLBACK [Ln " << cursor.y << ", Col " << cursor.x << "]\n";
    #endif

    const RelativeAddr next = Pos2Token(cursor, Registry::Editor->File());
    if (next.second.second == nullptr) return;
    std::cout << next.second.second->line << std::endl;

    Registry::Z80->SetBreakpoint((uint16_t)next.first);
    Registry::Editor->SelectLine((int32_t)next.second.second->line - 1);
  }

  void Event(Int2Type<MEMORY_SELECT_CALLBACK>, int32_t index) override { 
    #ifdef DEBUG_MODE 
    std::cout << "MEMORY_SELECT_CALLBACK [Addr " <<  std::setfill('0') << std::setw(5) << std::hex << std::uppercase << index << "]\n";
    #endif

    const RelativeAddr next = Addr2Token(index);
    if (next.second.second == nullptr) return;

    Registry::Editor->Open(next.second.first)->MoveTo(olc::vi2d(next.second.second->col - 1, next.second.second->line - 1));
  }

  void Event(Int2Type<KEYBOARD_EVENT_SUBSCRIBE>,  void* ptr) override {
    #ifdef DEBUG_MODE 
    std::cout << "KEYBOARD_EVENT_SUBSCRIBE\n";
    #endif

    this->vKeyEventListeners.insert(static_cast<Window::KeyboardEvent*>(ptr));
  }

  void Event(Int2Type<KEYBOARD_EVENT_UNSUBSCRIBE>,  void* ptr) override {
    #ifdef DEBUG_MODE 
    std::cout << "KEYBOARD_EVENT_UNSUBSCRIBE\n";
    #endif

    this->vKeyEventListeners.erase(static_cast<Window::KeyboardEvent*>(ptr));
  }

  void Event(Int2Type<PANEL_SELECT_CALLBACK>,  int32_t index) override {
    #ifdef DEBUG_MODE 
    std::cout << "PANEL_SELECT_CALLBACK " << index << "\n" ;
    #endif

    if (index - 1 < 0 || index - 1 >= panels.size()) return;

    auto size = panels[nPanel].GetSize();
    panels[nPanel = index - 1].Select(std::pair(olc::vi2d(0, 0), size));
  }

  void Event(Int2Type<CMD_UPDATE_CALLBACK>, std::string cmd) override {
    #ifdef DEBUG_MODE 
    std::cout << "CMD_UPDATE_CALLBACK '" << cmd << "'" << std::endl;
    #endif

    // nCurr = 0; this->cmd = cmd;
  }

  void Event(Int2Type<CMD_EXEC_CALLBACK>) override {
    #ifdef DEBUG_MODE 
    std::cout << "CMD_EXEC_CALLBACK\n";
    #endif

    // advance(); // Consume ':'

    // Cmd2Action(); nCurr = 0; cmd = "";
  }

  void Event(Int2Type<PROGRAM_EXIT>) override {
    #ifdef DEBUG_MODE 
    std::cout << "PROGRAM_EXIT\n";
    #endif
    bExec = false;
  }

private:
  inline void Cmd2Action() {
    // auto editor = panels[nPanel].Find(Type2Type<Editor::Editor>());
    // if (editor == nullptr) return;

    // switch (advance()) {
    //   case 'w': 
    //     if (match('a')) editor->Save();
    //     else editor->SaveFile();

    //     return;

    //   case 'e':
    //     advance(); // Consume ' '
    //     if (isAtEnd()) return;

    //     editor->Open(cmd.substr(nCurr));
    //     return;

    //   case 'c': {
    //     Bus::Z80::Lock l(this);

    //     Compile(interpreter.filepath);
    //     interpreter.env.save(std::filesystem::path(interpreter.filepath).replace_extension("bin"));

    //     for (auto f : interpreter.env.files()) editor->Open(f);
    //     editor->Open(interpreter.filepath);

    //     return;
    //   }

    //   case 'm': {
    //     Bus::Z80::Lock l(this);
    //     bus->W27C512->Load(interpreter.env.memory);
    //     bus->W27C512->Disassemble(); 

    //     return;
    //   }

    //   case 'l':  {
    //     Bus::Z80::Lock l(this);
    //     Compile(editor->File());
    //     return;
    //   }
    // }
  }


  inline RelativeAddr Addr2Token(uint32_t addr) {
    RelativeAddr next = std::pair(0, std::pair("", nullptr));

    // TODO:
    // for (auto& pos : interpreter.env.tokens) {
    //   if (pos.first <= addr) next = pos;
    //   else return next;
    // }

    return next;
  }

  inline RelativeAddr Pos2Token(olc::vi2d cursor, std::string filename) {
    RelativeAddr next = std::pair(0, std::pair(filename, nullptr));

    // TODO:
    // for (auto& pos : interpreter.env.tokens) {
    //   if (next.second.first != pos.second.first) continue;
    //   if (pos.second.second->line <= cursor.y + 1 && pos.second.second->col <= cursor.x + 1) next = pos;
    // }

    return next;
  }

  // inline bool isAtEnd() { return nCurr >= cmd.size(); }
  // inline const char advance() { return isAtEnd() ? '\0': cmd[nCurr++]; }
  // inline const char peek() { return isAtEnd() ? '\0' : cmd[nCurr]; }
  // inline bool match(char c) { if (peek() == c) { advance(); return true; } return false; }

  bool Compile(std::string filepath) {
    if (!filepath.size()) return false;

    const auto& errors = Registry::Interpreter->load(filepath).scan();
    if (!errors.size()) return Registry::Interpreter->save(std::filesystem::path(filepath).replace_extension("bin"));

    #ifdef DEBUG_MODE 
    // for (auto token : interpreter.parser.lexer.tokens) { token.print(); }
    // printf("\n");

    for (auto& msg : errors) {
      printf("%s", msg.c_str());
    }

    // printf("\nHAS AN ERROR %ld\n", interpreter.errors.size());
    #endif

    return false;
  }

  std::string Command() {
    std::string result = "";
    for (size_t i = 0; i < vKeyCombination.size(); i++, result += "+") {
      result += Window::KeyboardEvent::GetKeycode(vKeyCombination[i]);
    }

    return result;
  }

private:
  const olc::vi2d vOffset = olc::vi2d(0, 12);
  const olc::vi2d vStep = olc::vi2d(8, 12);

  LuaScript lua;
  std::vector<Panel> panels;

  int32_t nPanel = 0;
  float fStrokeRepeat = .0f;
  std::atomic<bool> bExec = true;

  std::vector<olc::Key> vKeyCombination;
  std::unordered_set<olc::Key> vKeyCombinationHeld;
  std::unordered_set<Window::KeyboardEvent*> vKeyEventListeners;
  std::function<void(olc::Key)> fnStrokeAction = nullptr;

  std::pair<std::atomic<bool>, std::unique_ptr<std::thread>> sync = std::pair(false, nullptr);
  static inline const std::string keybindings = "Ctrl-Space ?    Open/Close list of key bindings";
};

int main() {
  Defs::Init(); Interpreter::Defs::Init(); Bus::Defs::Init();  // Initialize types

  LuaScript lua;
  if (!lua.DoFile("src/lua/Config.lua")) return 0;
  // if (!lua.Has("Config")) return 0;

  // if (!Panel::GetConfig().Init("src/lua/Bindings.lua")) return 0;
  // if (!Panel::GetConfig().GetTable("Bindings")) return 0;

  // TODO: Create defs to load colors from lua
  // Defs::Load(&luaConfig);

  const int32_t width  = lua.GetOr("WIDTH", 700);
  const int32_t height  = lua.GetOr("HIGHT", 412);
  const int32_t px = lua.GetOr("PIXELS", 2);

  App app(std::move(lua));
  if (app.Construct(width, height, px, px)) app.Start();
  
  return 0;
}
