#pragma once

#include "src/Defs.h"
#include "src/Editor/Popup.h"
#include "src/Window/Window.h"
#include "src/Window/Event/KeyboardEvent.h"
#include <memory>

template <class T>
using WindowT = std::tuple<bool, bool, std::shared_ptr<T>, DimensionT, int32_t>;

template <class T>
using WindowInitT = std::tuple<std::shared_ptr<T>, DimensionT>;

#define SELECTED(p)  std::get<0>(p)
#define ZOOMED(p)    std::get<1>(p)
#define PTR(p)       std::get<2>(p)
#define DIMENSION(p) std::get<3>(p)
#define WINDOW(p)    std::get<4>(p)

#define POS(p) DIMENSION(p).first
#define SIZE(p) DIMENSION(p).second
#define EXIST(p) PTR(p) != nullptr
#define IS_NULL(p) PTR(p) == nullptr
#define SHOULD_DRAW(flag, p) (!flag || (flag && ZOOMED(p)))
#define IS_INSIDE(rect, pos) (POS(rect).x < pos.x && POS(rect).y < pos.y && pos.x < POS(rect).x + SIZE(rect).x && pos.y < POS(rect).y + SIZE(rect).y)
        
class Panel : public Window::KeyboardEvent {
  // enum ModeT { NORMAL, COMMAND };

public:
  Panel(PixelGameEngine* engine): Window::Base(engine), ::Window::KeyboardEvent(engine) {}

  // void Initialize() {
  //   for (auto& w : windows) if (EXIST(w)) PTR(w)->Initialize();
  // }

  void Select(DimensionT dimensions) {
    Window::KeyboardEvent::Focus(true);

    this->absolute = dimensions.first; this->size = dimensions.second; bFullScreen = false;
    for (auto& w : windows) if (EXIST(w)) PTR(w)->Select(DIMENSION(w));
  }

  // void Preprocess() {
  //   for (auto& w : windows) if (EXIST(w)) PTR(w)->Preprocess();
  // }

  void Process() {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Process, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Process, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();
    bUpdated = false; bReleased = false;

    // Process(Int2Type<NORMAL>(), GameEngine);
    // if (mode == COMMAND) { if (!cmd.size() && bReleased) mode = NORMAL; return; }

    if (GameEngine->GetMouse(0).bPressed) {
      auto mouse = GameEngine->GetMousePos();
      if (mouse.y - size.y > 0) return;

      for (auto& w : windows) if (!bFullScreen && EXIST(w)) SELECTED(w) = IS_INSIDE(w, mouse);
      for (auto& w : windows) if (EXIST(w)) PTR(w)->Focus(SELECTED(w)); 
    }

    for (auto& w : windows) if (SELECTED(w)) PTR(w)->Process();
  }

  void Draw() {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Draw, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Draw, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    for (auto& w : windows) if (EXIST(w) && SHOULD_DRAW(bFullScreen, w)) PTR(w)->Draw();

    // if (mode == NORMAL) return;
    // if (cmd.size() && cmd.back() == 'q' && !search.bEnabled)
    if (bDrawWindowIndex) {
      for (auto& w : windows) {
        if (IS_NULL(w) || !SIZE(w).x || !SIZE(w).y) continue;

        const auto& color = SELECTED(w) ? Color::BLUE : Color::RED;
        GameEngine->DrawString(POS(w), std::string(1, '0' + WINDOW(w)), color, 3);
      }
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

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::Z>) { 
    for (auto& w : windows) if (SELECTED(w)) { bFullScreen = (ZOOMED(w) ^= true); PTR(w)->Select(bFullScreen ? std::pair(absolute, size) : DIMENSION(w)); }
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::OEM_2>) { 
    WindowT<Window::Window>* bindings = nullptr;
    for (auto& w : windows) if (EXIST(w) && PTR(w)->windowId == "bindings") { bindings = &w; break; }

    if (bindings == nullptr) return nullptr;


    int32_t selected = 0;
    std::vector<std::string> commands = this->Combinations(&GetCommands());
    auto vim = std::dynamic_pointer_cast<Editor::Popup>(PTR(*bindings));

    if (SELECTED(*bindings)) {
      vim->Load(commands);
      Process(Int2Type<olc::Key::CTRL>(), Int2Type<olc::Key::SPACE>(), Int2Type<olc::Key::Z>());

      SELECTED(*bindings) = false;
      // cmd.push_back(popup->window + '0'); Command(Int2Type<Editor::VimT::CMD_q>()); cmd.pop_back();
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
    for (auto& w : windows) {
      if (!SELECTED(w)) continue;
      auto combinations = this->Combinations(&PTR(w)->GetCommands());
      commands.insert(commands.end(), combinations.begin(), combinations.end());

      selected = WINDOW(w);
    }
    // cmd.push_back('0'); Command(Int2Type<Editor::VimT::CMD_q>()); cmd.pop_back();

    // auto panel = GetBindings("panel");
    // bindings.insert(bindings.end(), panel.begin(), panel.end());


    vim->Load(commands); vim->window = selected; SELECTED(*bindings) = true;
    Process(Int2Type<olc::Key::CTRL>(), Int2Type<olc::Key::SPACE>(), Int2Type<olc::Key::Z>());

    // TODO: Impl POPUP logic HERE
    // for (auto& t : bindings) printf("%s\n", t.c_str());
    return nullptr;
  }

  // inline void Command(Int2Type<Editor::VimT::CMD_w>) {
  //   int32_t digit = this->digit();
  //   for (auto& w : windows) if (EXIST(w)) PTR(w)->SelectHighlight(digit);
  // }

  // inline void Command(Int2Type<Editor::VimT::CMD_COLON>) { 
  //   AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<CMD_EXEC_CALLBACK>());
  // }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::A>) { 
    GameEngine->Event(Int2Type<ATTACH_DEBUG_MODE_CALLBACK>()); 
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::C>) { 
    GameEngine->Event(Int2Type<NEW_DEBUG_MODE_CALLBACK>()); 
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::D>) { 
    GameEngine->Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>()); 
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::K7>) { 
    GameEngine->Event(Int2Type<PROGRAM_EXIT>()); 
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::Q>) { 
    bDrawWindowIndex = true;

    return [&](olc::Key k) {
      int32_t digit = k - olc::Key::K0; bDrawWindowIndex = false;
      for (auto& w : windows) if (EXIST(w)) SELECTED(w) = WINDOW(w) == digit;
      for (auto& w : windows) if (EXIST(w)) PTR(w)->Focus(SELECTED(w)); 
    };
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K1>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 1); 
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K2>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 2); 
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K3>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 3); 
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K4>) { 
    GameEngine->Event(Int2Type<PANEL_SELECT_CALLBACK>(), 4); 
    return nullptr;
  }

  Window::StrokeAction Process(Int2Type<olc::Key::CTRL>, Int2Type<olc::Key::SPACE>, Int2Type<olc::Key::K5>) { 
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
  // template<class T>
  // std::shared_ptr<T> Find(Type2Type<T> type) { return std::dynamic_pointer_cast<T>(PTR(Window(type))); }

// private:
  template<typename T, typename... Args>
  inline void Push(T ref, Args ...args) { Push(ref); Push(args...); }

  inline void Push() {}
  
  template<class T>
  inline void Push(WindowInitT<T> b) { windows.push_back(std::tuple_cat(std::make_tuple(nWindows == 1, false), b, std::make_tuple(++nWindows))); }

public:
  // inline bool IsActive() { return mode == COMMAND; }
  inline bool IsActive() { return false; } // FIXME:?????????
  inline olc::vi2d GetSize() { return olc::vi2d(size.x, size.y); }
  inline std::string GetName() {
    for (const auto& w: windows) if (EXIST(w) && SELECTED(w)) return PTR(w)->windowId;
    return windows.size() ? PTR(windows.front())->windowId : "unknown";
  }

  // static LuaScript& GetConfig() { static LuaScript config; return config; }
  const Window::StrokeT<>& GetCommands() const override { return Panel::commands; }

private:
  // ModeT mode = NORMAL;
  int32_t nWindows = 0;
  bool bFullScreen = false;
  bool bDrawWindowIndex = false;

  olc::vi2d absolute = olc::vi2d(0, 0);
  olc::vi2d size = olc::vi2d(0, 0);

  std::list<WindowT<Window::Window>> windows;
  static const Window::StrokeT<> commands;
};


inline const Window::StrokeT<> Panel::commands = Window::StrokeT<>::Init({
  { Int2Type<olc::Key::CTRL>(), {
    { Int2Type<olc::Key::SPACE>(), {
      { Int2Type<olc::Key::K1>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K2>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K3>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K4>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::K5>(), [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },

      { Int2Type<olc::Key::A>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
      { Int2Type<olc::Key::C>(),  [](void* self, auto... keys) { return static_cast<Panel*>(self)->Process(keys...); } },
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
#undef ZOOMED
#undef PTR
#undef DIMENSION
#undef WINDOW

#undef POS
#undef SIZE
#undef EXIST
#undef SHOULD_DRAW
#undef IS_INSIDE