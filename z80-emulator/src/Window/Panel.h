#pragma once

#include "include/Typelist.h"
#include "src/Defs.h"
#include "src/Editor/Vim.h"
#include "src/Utils/Utils.h"
#include "src/Window/Popup.h"
#include "src/Window/Window.h"
#include "src/Window/Event/KeyboardEvent.h"
#include <cstdint>
#include <memory>
#include <utility>

using WindowT = std::tuple<bool, uint8_t, std::shared_ptr<::Window::Window>, ::Window::DimensionT, int32_t>;
using WindowInitT = std::tuple<std::shared_ptr<::Window::Window>, uint8_t, ::Window::DimensionT>;

#define SELECTED(p)  std::get<0>(p)
#define DEPTH(p)     std::get<1>(p)
#define PTR(p)       std::get<2>(p)
#define DIMENSION(p) std::get<3>(p)
#define WINDOW(p)    std::get<4>(p)

#define POS(p) DIMENSION(p).first
#define SIZE(p) DIMENSION(p).second
#define EXIST(p) PTR(p) != nullptr
#define IS_NULL(p) PTR(p) == nullptr
// #define SHOULD_DRAW(flag, p) (!flag || (flag && ZOOMED(p)))
#define IS_INSIDE(rect, pos) (POS(rect).x < pos.x && POS(rect).y < pos.y && pos.x < POS(rect).x + SIZE(rect).x && pos.y < POS(rect).y + SIZE(rect).y)
        
class Panel : public Window::KeyboardEvent, public Window::Window {
  enum DepthT { HIDDEN, NORMAL, ZOOMED = 0xFF };

public:
  Panel(PixelGameEngine* engine): Window::Base(engine), Window::Window(engine, ""), ::Window::KeyboardEvent(engine) {
    auto popup = std::make_shared<::Window::Popup>(engine, "bindings", std::move(std::make_shared<Editor::Vim>(engine)));
    popup->bindings = ::Window::StrokeT<>::Init({
      { Int2Type<olc::Key::ESCAPE>(), [this](void*, auto...) { return this->Process(Int2Type<olc::Key::CTRL>(), Int2Type<olc::Key::SPACE>(), Int2Type<olc::Key::SHIFT>(), Int2Type<olc::Key::OEM_2>()); } }
    });

    Push(WindowInitT(std::move(popup), HIDDEN, std::pair(olc::vi2d(0, 0), olc::vi2d(engine->ScreenWidth() - 24, engine->ScreenHeight() - 40))));
  }

  // void Initialize() {
  //   for (auto& w : windows) if (EXIST(w)) PTR(w)->Initialize();
  // }

  void Select(::Window::DimensionT dimensions) override {
    ::Window::Window::Select(dimensions);
    for (auto& w : windows) if (EXIST(w)) PTR(w)->Select(DIMENSION(w));
  }

  void Process() override {
    const auto mouse = GameEngine->GetMousePos();
    const auto bPressed = GameEngine->GetMouse(0).bPressed;
    std::pair<uint8_t, std::shared_ptr<::Window::Window>> window = std::pair(HIDDEN, nullptr);

    for (auto& w : windows) {
      if (!(EXIST(w))) continue;

      if (IS_INSIDE(w, mouse)) PTR(w)->Preprocess();
      if (bPressed && DEPTH(w)) PTR(w)->Focus(SELECTED(w) = IS_INSIDE(w, mouse));
      if (SELECTED(w) && DEPTH(w) >= window.first) window = std::pair(DEPTH(w), PTR(w));
    }

    if (window.second != nullptr) window.second->Process();
  }

  void Draw() override {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Draw, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Draw, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();
    windows.sort([](const auto& a, const auto& b) { return DEPTH(a) < DEPTH(b); });

    for (auto& w : windows) {
      if (!(EXIST(w) && DEPTH(w))) continue;
      GameEngine->FillRect(PTR(w)->absolute, PTR(w)->absolute + PTR(w)->size, Color::BLACK);
      PTR(w)->Draw();
    }

    // if (mode == NORMAL) return;
    // if (cmd.size() && cmd.back() == 'q' && !search.bEnabled)
    if (!bDrawWindowIndex) return;
    for (auto& w : windows) {
      if (IS_NULL(w) || !SIZE(w).x || !SIZE(w).y) continue;

      const auto& color = SELECTED(w) ? Color::BLUE : Color::RED;
      GameEngine->DrawString(POS(w), std::string(1, '0' + WINDOW(w)), color, 3);
    }
    // if (cmd.size() && cmd.back() == 'w' && !search.bEnabled) Highlight(GameEngine);
  }

  // void Highlight(PixelGameEngine* GameEngine) {
  //   for (auto& w : windows) if (EXIST(w) && SHOULD_DRAW(bFullScreen, w)) PTR(w)->Highlight(GameEngine);
  // }

  // void Lock() {
  //   for (auto& w : windows) if (EXIST(w)) PTR(w)->Lock();
  // }

  // void Unlock() {
  //   for (auto& w : windows) if (EXIST(w)) PTR(w)->Unlock();
  // }

public:
  // template<int32_t T>
  // inline void Command(Int2Type<T>) {}

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::Z>) { 
    for (auto& w : windows) {
      if (!(EXIST(w) && SELECTED(w))) continue;;
      PTR(w)->Select(DEPTH(w) == ZOOMED ? std::pair(absolute, size) : DIMENSION(w));
      DEPTH(w) = DEPTH(w) == ZOOMED ? NORMAL : ZOOMED; 
    }
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::OEM_2>) { 
    // WindowT<::Window::Window>* bindings = nullptr;
    // for (auto& w : windows) if (EXIST(w) && PTR(w)->windowId == "bindings") { bindings = &w; break; }
    // if (bindings == nullptr) return nullptr;
    auto window = Find("bindings");
    if (window == nullptr) return nullptr;

    // int32_t selected = 0;
    // auto popup = std::dynamic_pointer_cast<>(PTR(*bindings));

    if (SELECTED(*window)) {
      for (auto& w : windows) if (nWindow == WINDOW(w)) PTR(w)->Focus(SELECTED(w) = true);
      PTR(*window)->Focus(SELECTED(*window) = false); DEPTH(*window) = HIDDEN;
      return nullptr;
    }

    // ???????
    // auto GetBindings = [](const char* key) {
    //   Panel::GetConfig().GetTableValue<int32_t>(nullptr, key);
    //   auto list = Panel::GetConfig().GetArray<std::string>(nullptr);
    //   Panel::GetConfig().Pop();

    //   return list;
    // };

    // for (auto& w : windows) if (SELECTED(w)) { bindings = GetBindings(PTR(w)->GetBindings().c_str()); selected = WINDOW(w); }
    std::vector<std::string> commands = this->Combinations(&GetCommands());

    for (auto& w : windows) {
      if (!SELECTED(w)) continue;
      else nWindow = WINDOW(w), PTR(w)->Focus(SELECTED(w) = false);

      const auto combinations = this->Combinations(&PTR(w)->GetCommands());
      commands.insert(commands.end(), combinations.begin(), combinations.end());
    }
    // cmd.push_back('0'); Command(Int2Type<Editor::VimT::CMD_q>()); cmd.pop_back();

    // auto panel = GetBindings("panel");
    // bindings.insert(bindings.end(), panel.begin(), panel.end());

    
    auto popup = Utils::Cast<::Window::Popup>(PTR(*window));
    auto vim = Utils::Cast<Editor::Vim>(popup->window);

    vim->Load(commands); popup->Focus(SELECTED(*window) = true); DEPTH(*window) = 2;
    
    // NOTE: Temp solution
    // Process(Int2Type<olc::Key::CTRL>(), Int2Type<olc::Key::SPACE>(), Int2Type<olc::Key::Z>());

    // TODO: Impl POPUP logic HERE
    return nullptr;
  }

  // inline void Command(Int2Type<Editor::VimT::CMD_w>) {
  //   int32_t digit = this->digit();
  //   for (auto& w : windows) if (EXIST(w)) PTR(w)->SelectHighlight(digit);
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_COLON>) { 
  //   AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<CMD_EXEC_CALLBACK>());
  // }

  ::Window::StrokeAction Process(Int2Type<olc::Key::SPACE>) { 
    GameEngine->Event(Int2Type<NEXT_DEBUG_STEP_CALLBACK>()); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::SPACE>) { 
    GameEngine->Event(Int2Type<NEW_DEBUG_MODE_CALLBACK>()); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::D>) { 
    GameEngine->Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>()); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::K7>) { 
    GameEngine->Event(Int2Type<PROGRAM_EXIT>()); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::Q>) { 
    bDrawWindowIndex = true;

    return [&](olc::Key k, int32_t) {
      int32_t digit = k - olc::Key::K0; bDrawWindowIndex = false;
      for (auto& w : windows) if (EXIST(w)) PTR(w)->Focus(SELECTED(w) = WINDOW(w) == digit);
      return false;
    };
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K1>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 1); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K2>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 2); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K3>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 3); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K4>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 4); 
    return nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K5>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 5); 
    return nullptr;
  }

  // void Process(Int2Type<NORMAL>, PixelGameEngine* GameEngine) {
    // foreach<KeyEvent, Panel>::Process(this);

    // if (!bUpdated) return;
    // else bUpdated = false;
    
    // #ifdef DEBUG_MODE
    // if (mode == COMMAND) printf("Panel: '%s'\n", cmd.c_str());
    // #endif

    // if (search.bEnabled) return GameEngine->Event(Int2Type<CMD_UPDATE_CALLBACK>(), cmd.substr(search.nStartAt));

    // if (nCurr == 0) {
    //   if (cmd.size() > 1 && match<1>({ '^' })) {
    //     if (match<1>({ ' ' })) { mode = COMMAND; return; }
    //     if (peek() == 'r' && GameEngine->GetMode() == PixelGameEngine::DEBUG) {
    //       mode = COMMAND;  GameEngine->Event(Int2Type<DEBUG_RESET_CALLBACK>());
    //     }
    //   }

    //   if (peek() == ' ' && GameEngine->GetMode() == PixelGameEngine::DEBUG) {
    //     mode = COMMAND; 
    //     std::thread p([=]() { GameEngine->Event(Int2Type<NEXT_DEBUG_STEP_CALLBACK>()); }); p.detach();
    //   }

    //   return reset(false);
    // } 

    // if (peekPrev() == 'q' && Utils::IsDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_q>()); return reset(); } 
    // if (peekPrev() == 'w' && peek() - '0' > 0) { phrase(Int2Type<Editor::VimT::CMD_w>()); return reset(); } 

    // // noun
    // if (match<1>({ ' ' })) { phrase(Int2Type<Editor::VimT::CMD_SPACE>()); return reset(); } 
    // if (match<1>({ 'a' })) { phrase(Int2Type<Editor::VimT::CMD_a>()); return reset(); } 
    // if (match<1>({ 'd' })) { phrase(Int2Type<Editor::VimT::CMD_d>()); return reset(); } 
    // if (match<1>({ 'z' })) { phrase(Int2Type<Editor::VimT::CMD_z>()); return reset(); } 
    // if (match<1>({ '?' })) { phrase(Int2Type<Editor::VimT::CMD_QUESTION>()); return reset(); } 
    // if (match<1>({ '&' })) { phrase(Int2Type<Editor::VimT::CMD_AND>()); return reset(); } 
    // if (match<1>({ ':' })) {
    //   mode = COMMAND; search = Window::SearchT(true, 2, -1, true, {});
    //   GameEngine->Event(Int2Type<CMD_UPDATE_CALLBACK>(), cmd.substr(search.nStartAt));
    //   return phrase(Int2Type<Editor::VimT::CMD_COLON>());
    // }

    // if (Utils::IsDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_NUMBER>()); return reset(); } 

    // // verb
    // if (match<2>({ 'q', 'w' })) return;

    // reset(false);
  // }

// private:
//   template<class T>
//   WindowT<Window::Window>& Window(Type2Type<T> type) {
//     std::string id = typeid(T*).name();
//     for (auto& w : windows) if (EXIST(w) && PTR(w)->GetId() == id) return w;

//     static WindowT<Window::Window> empty = std::tuple_cat(std::make_tuple(false, false, nullptr, std::pair(olc::vi2d(), olc::vi2d()), 0));
//     return empty;
//   }

public:
  WindowT* Find(std::string id) {
    for (auto& w : windows) if (EXIST(w) && PTR(w)->windowId == id) return &w;
    return nullptr;
  }

  template<typename T, typename... Args>
  inline void Push(T ref, Args ...args) { Push(ref); Push(args...); }

  inline void Push() {}
  
  inline void Push(WindowInitT b) { windows.push_back(WindowT(false, std::get<1>(b), std::get<0>(b), std::get<2>(b), windows.size() + 1)); }

public:
  // inline bool IsActive() { return mode == COMMAND; }
  // inline bool IsActive() { return false; } // FIXME:?????????
  inline olc::vi2d GetSize() { return olc::vi2d(size.x, size.y); }
  inline std::string GetName() {
    for (const auto& w: windows) if (EXIST(w) && SELECTED(w)) return PTR(w)->windowId;
    return windows.size() ? PTR(windows.front())->windowId : "unknown";
  }

  // static LuaScript& GetConfig() { static LuaScript config; return config; }
  const ::Window::StrokeT<>& GetCommands() const override { return Panel::commands; }

private:
  bool bDrawWindowIndex = false;

  int32_t nWindow = 0;
  std::list<WindowT> windows;
  static const ::Window::StrokeT<> commands;
};


inline const Window::StrokeT<> Panel::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::SPACE>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
  { Int2Type<olc::Key::CTRL>(), {
    { Int2Type<olc::Key::SPACE>(), {
      { Int2Type<olc::Key::K1>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K2>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K3>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K4>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K5>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },

      { Int2Type<olc::Key::SPACE>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::D>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::Q>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::Z>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },

      { Int2Type<olc::Key::SHIFT>(), {
        { Int2Type<olc::Key::K7>(),    [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
        { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      }},
    }}
  }}
});


#undef SELECTED
#undef DISPLAYED
#undef PTR
#undef DIMENSION
#undef WINDOW

#undef POS
#undef SIZE
#undef EXIST
#undef SHOULD_DRAW
#undef IS_INSIDE