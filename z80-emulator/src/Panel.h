#pragma once
#include "Bus/Bus.h"
#include "Editor/Popup.h"


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
#define SHOULD_DRAW(flag, p) (!flag || (flag && ZOOMED(p)))
#define IS_INSIDE(rect, pos) (POS(rect).x < pos.x && POS(rect).y < pos.y && pos.x < POS(rect).x + SIZE(rect).x && pos.y < POS(rect).y + SIZE(rect).y)
        

/**
 * 
 * Grammar:
 *  command     -> <leader> phrase
 * 
 *  phrase      -> (verb DIGIT) | noun | NUMBER
 *  noun        -> '?' | 'z'
 *  verb        -> 'q'
 */
class Panel : public Window::Command {
  enum ModeT { NORMAL, COMMAND };

public:
  template<typename ...Args>
  Panel(Args ...args) { Init(args...); }


  void Preinitialize() {
    for (auto& w : windows) if (EXIST(w)) PTR(w)->Preinitialize();
  }

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second; bFullScreen = false;
    for (auto& w : windows) if (EXIST(w)) PTR(w)->Initialize(DIMENSION(w));
  }

  void Preprocess() {
    for (auto& w : windows) if (EXIST(w)) PTR(w)->Preprocess();
  }

  void Process(PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Process, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Process, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();
    bUpdated = false; bReleased = false;

    Process(Int2Type<NORMAL>(), GameEngine);
    if (mode == COMMAND) { if (!cmd.size() && bReleased) mode = NORMAL; return; }

    if (GameEngine->GetMouse(0).bPressed) {
      auto mouse = GameEngine->GetMousePos();
      if (mouse.y - size.y > 0) return;

      for (auto& w : windows) if (!bFullScreen && EXIST(w)) SELECTED(w) = IS_INSIDE(w, mouse);
    }

    for (auto& w : windows) if (SELECTED(w)) PTR(w)->Process(GameEngine);
  }

  void Draw(PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Draw, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Draw, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    for (auto& w : windows) if (EXIST(w) && SHOULD_DRAW(bFullScreen, w)) PTR(w)->Draw(GameEngine);

    if (mode == NORMAL) return;
    if (cmd.back() == 'q' && !search.bEnabled) Draw(Int2Type<Editor::VimT::CMD_q>(), GameEngine);
    if (cmd.back() == 'w' && !search.bEnabled) Highlight(GameEngine);
  }

  void Highlight(PixelGameEngine* GameEngine) {
    for (auto& w : windows) if (EXIST(w) && SHOULD_DRAW(bFullScreen, w)) PTR(w)->Highlight(GameEngine);
  }

  void Lock() {
    for (auto& w : windows) if (EXIST(w)) PTR(w)->Lock();
  }

  void Unlock() {
    for (auto& w : windows) if (EXIST(w)) PTR(w)->Unlock();
  }

private:
  void Draw(Int2Type<Editor::VimT::CMD_q>, PixelGameEngine* GameEngine) {
    auto index = [&](auto tuple) {
      auto color = SELECTED(tuple) ? AnyType<BLUE, ColorT>::GetValue() : AnyType<RED, ColorT>::GetValue();
      GameEngine->DrawString(POS(tuple), std::string(1, '0' + WINDOW(tuple)), *color, 4);
    };

    for (auto& w : windows) if (EXIST(w)) index(w);
  }

public:
  template<int32_t T>
  inline void Command(Int2Type<T>) {}

  inline void Command(Int2Type<Editor::VimT::CMD_z>) {
    for (auto& w : windows) if (SELECTED(w)) { bFullScreen = (ZOOMED(w) ^= true); PTR(w)->Initialize(bFullScreen ? std::pair(absolute, size) : DIMENSION(w)); }
  }

  inline void Command(Int2Type<Editor::VimT::CMD_QUESTION>) {
    auto popup = Find(Type2Type<Editor::Popup>());
    auto& window = Window(Type2Type<Editor::Popup>());
    if (!(EXIST(window))) return;

    int32_t selected = 0;
    std::vector<std::string> bindings;

    if (SELECTED(window)) {
      popup->Load(bindings);
      Command(Int2Type<Editor::VimT::CMD_z>());

      SELECTED(window) = false;
      cmd.push_back(popup->window + '0'); Command(Int2Type<Editor::VimT::CMD_q>()); cmd.pop_back();
      return;
    }

    auto GetBindings = [](const char* key) {
      Panel::GetConfig().GetTableValue<int32_t>(nullptr, key);
      auto list = Panel::GetConfig().GetArray<std::string>(nullptr);
      Panel::GetConfig().Pop();

      return list;
    };

    for (auto& w : windows) if (SELECTED(w)) { bindings = GetBindings(PTR(w)->GetBindings().c_str()); selected = WINDOW(w); }
    cmd.push_back('0'); Command(Int2Type<Editor::VimT::CMD_q>()); cmd.pop_back();

    auto panel = GetBindings("panel");
    bindings.insert(bindings.end(), panel.begin(), panel.end());


    popup->Load(bindings); popup->window = selected; SELECTED(window) = true;
    Command(Int2Type<Editor::VimT::CMD_z>());

    // TODO: Impl POPUP logic HERE
    // for (auto& t : bindings) printf("%s\n", t.c_str());
  }

  inline void Command(Int2Type<Editor::VimT::CMD_SPACE>) {
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<NEW_DEBUG_MODE_CALLBACK>()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_a>) {
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<ATTACH_DEBUG_MODE_CALLBACK>()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_d>) {
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<DETACH_DEBUG_MODE_CALLBACK>()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_COLON>) { 
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<CMD_EXEC_CALLBACK>());
  }

  inline void Command(Int2Type<Editor::VimT::CMD_NUMBER>) {
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<PANEL_SELECT_CALLBACK>(), digit()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_AND>) {
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<PROGRAM_EXIT>()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_w>) {
    int32_t digit = this->digit();
    for (auto& w : windows) if (EXIST(w)) PTR(w)->SelectHighlight(digit);
  }

  inline void Command(Int2Type<Editor::VimT::CMD_q>) {
    int32_t digit = peek() - '0';
    for (auto& w : windows) if (EXIST(w)) SELECTED(w) = WINDOW(w) == digit;
  }

  void Process(Int2Type<NORMAL>, PixelGameEngine* GameEngine) {
    foreach<KeyEvent, Panel>::Process(this);

    if (!bUpdated) return;
    else bUpdated = false;
    
    #ifdef DEBUG_MODE
    if (mode == COMMAND) printf("Panel: '%s'\n", cmd.c_str());
    #endif

    if (search.bEnabled) return GameEngine->Event(Int2Type<CMD_UPDATE_CALLBACK>(), cmd.substr(search.nStartAt));

    if (nCurr == 0) {
      if (cmd.size() > 1 && match<1>({ '^' })) {
        if (match<1>({ ' ' })) { mode = COMMAND; return; }
        if (peek() == 'r' && GameEngine->GetMode() == PixelGameEngine::DEBUG) {
          mode = COMMAND;  GameEngine->Event(Int2Type<DEBUG_RESET_CALLBACK>());
        }
      }

      if (peek() == ' ' && GameEngine->GetMode() == PixelGameEngine::DEBUG) {
        mode = COMMAND; 
        std::thread p([=]() { GameEngine->Event(Int2Type<NEXT_DEBUG_STEP_CALLBACK>()); }); p.detach();
      }

      return reset(false);
    } 

    if (peekPrev() == 'q' && Utils::IsDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_q>()); return reset(); } 
    if (peekPrev() == 'w' && peek() - '0' > 0) { phrase(Int2Type<Editor::VimT::CMD_w>()); return reset(); } 

    // noun
    if (match<1>({ ' ' })) { phrase(Int2Type<Editor::VimT::CMD_SPACE>()); return reset(); } 
    if (match<1>({ 'a' })) { phrase(Int2Type<Editor::VimT::CMD_a>()); return reset(); } 
    if (match<1>({ 'd' })) { phrase(Int2Type<Editor::VimT::CMD_d>()); return reset(); } 
    if (match<1>({ 'z' })) { phrase(Int2Type<Editor::VimT::CMD_z>()); return reset(); } 
    if (match<1>({ '?' })) { phrase(Int2Type<Editor::VimT::CMD_QUESTION>()); return reset(); } 
    if (match<1>({ '&' })) { phrase(Int2Type<Editor::VimT::CMD_AND>()); return reset(); } 
    if (match<1>({ ':' })) {
      mode = COMMAND; search = Window::SearchT(true, 2, -1, true, {});
      GameEngine->Event(Int2Type<CMD_UPDATE_CALLBACK>(), cmd.substr(search.nStartAt));
      return phrase(Int2Type<Editor::VimT::CMD_COLON>());
    }

    if (Utils::IsDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_NUMBER>()); return reset(); } 

    // verb
    if (match<2>({ 'q', 'w' })) return;

    reset(false);
  }

  template<int32_t T>
  inline void phrase(Int2Type<T> val) {
    auto operation = lambda;
    lambda = [=]() { operation(); Command(val); };
  }

  inline void reset(bool exec = true) {
    if (exec) lambda();

    cmd = ""; nStart = nCurr = 0; search.bEnabled = false; lambda = []() {}; 
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<CMD_UPDATE_CALLBACK>(), cmd);
  }


public:
  template<int32_t T, int32_t U>
  void Process(TypeList<Int2Type<T>, Int2Type<U>>) {
    const char c = static_cast<char>(+U);
    BasicStrokeHandler(static_cast<olc::Key>(+T), c, toupper(c));
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

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>)    { EscapeStrokeHandler(olc::Key::DEL); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) { EscapeStrokeHandler(olc::Key::ESCAPE); }

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>)   {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::BACK).bPressed) return;

    
    bUpdated = true; 
    switch (mode) {
      case NORMAL: return;
      case COMMAND:
        if (!search.bEnabled) return reset(false);
        if (cmd.size()) cmd.pop_back();
        if (cmd.size()) break;

        mode = NORMAL; reset(false);
        break;
    }
  }

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>)  {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::ENTER).bPressed) return;
    
    bUpdated = true; mode = NORMAL; reset(search.bEnabled);
  }

private:
  inline void EscapeStrokeHandler(olc::Key key) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(key).bPressed) return;

    bUpdated = true; mode = NORMAL; reset(false);
  }

  template<class T>
  WindowT<Window::Window>& Window(Type2Type<T> type) {
    std::string id = typeid(T*).name();
    for (auto& w : windows) if (EXIST(w) && PTR(w)->GetId() == id) return w;

    static WindowT<Window::Window> empty = std::tuple_cat(std::make_tuple(false, false, nullptr, std::pair(olc::vi2d(), olc::vi2d()), 0));
    return empty;
  }

public:
  template<class T>
  std::shared_ptr<T> Find(Type2Type<T> type) { return std::dynamic_pointer_cast<T>(PTR(Window(type))); }

private:
  template<typename T, typename... Args>
  inline void Init(T ref, Args ...args) {  Init(ref); Init(args...); }

  inline void Init() {}
  
  template<class T>
  inline void Init(WindowInitT<T> b) { windows.push_back(std::tuple_cat(std::make_tuple(nWindows == 1, false), b, std::make_tuple(++nWindows))); }

public:
  inline bool IsActive() { return mode == COMMAND; }
  inline olc::vi2d GetSize() { return olc::vi2d(size.x, size.y); }

  inline std::string GetName() { 
    for (auto& w : windows) if (SELECTED(w)) return PTR(w)->GetName();
    return "";
  }

  static LuaScript& GetConfig() {
    static LuaScript config;
		return config;
  }

private:
  ModeT mode = NORMAL;
  int32_t nWindows = 0;
  bool bFullScreen = false;

  olc::vi2d absolute = olc::vi2d(0, 0);
  olc::vi2d size = olc::vi2d(0, 0);

  std::list<WindowT<Window::Window>> windows;
};


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