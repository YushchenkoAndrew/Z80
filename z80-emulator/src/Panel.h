#pragma once
#include "Bus/Bus.h"
#include "Editor/Editor.h"


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
    if (EXIST(bus))    PTR(bus)->Preinitialize();
    if (EXIST(eeprom)) PTR(eeprom)->Preinitialize();
    if (EXIST(stack))  PTR(stack)->Preinitialize();
    if (EXIST(lines))  PTR(lines)->Preinitialize();
    if (EXIST(editor)) PTR(editor)->Preinitialize();
  }

  void Initialize(DimensionT dimensions) {
    this->absolute = dimensions.first; this->size = dimensions.second; bFullScreen = false;

    if (EXIST(bus))    PTR(bus)->Initialize(DIMENSION(bus));
    if (EXIST(eeprom)) PTR(eeprom)->Initialize(DIMENSION(eeprom));
    if (EXIST(stack))  PTR(stack)->Initialize(DIMENSION(stack));
    if (EXIST(lines))  PTR(lines)->Initialize(DIMENSION(lines));
    if (EXIST(editor)) PTR(editor)->Initialize(DIMENSION(editor));
  }

  void Preprocess() {
    if (EXIST(bus))    PTR(bus)->Preprocess();
    if (EXIST(eeprom)) PTR(eeprom)->Preprocess();
    if (EXIST(stack))  PTR(stack)->Preprocess();
    if (EXIST(lines))  PTR(lines)->Preprocess();
    if (EXIST(editor)) PTR(editor)->Preprocess();
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

      if (!bFullScreen && EXIST(bus))    SELECTED(bus)    = IS_INSIDE(bus, mouse);
      if (!bFullScreen && EXIST(eeprom)) SELECTED(eeprom) = IS_INSIDE(eeprom, mouse);
      if (!bFullScreen && EXIST(stack))  SELECTED(stack)  = IS_INSIDE(stack, mouse);
      if (!bFullScreen && EXIST(lines))  SELECTED(lines)  = IS_INSIDE(lines, mouse);
      if (!bFullScreen && EXIST(editor)) SELECTED(editor) = IS_INSIDE(editor, mouse);
    }


    if (SELECTED(bus))    PTR(bus)->Process(GameEngine);
    if (SELECTED(eeprom)) PTR(eeprom)->Process(GameEngine);
    if (SELECTED(stack))  PTR(stack)->Process(GameEngine);
    if (SELECTED(lines))  PTR(lines)->Process(GameEngine);
    if (SELECTED(editor)) PTR(editor)->Process(GameEngine);
  }

  void Draw(PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Draw, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Draw, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    if (EXIST(bus)    && SHOULD_DRAW(bFullScreen, bus))    PTR(bus)->Draw(GameEngine);
    if (EXIST(eeprom) && SHOULD_DRAW(bFullScreen, eeprom)) PTR(eeprom)->Draw(GameEngine);
    if (EXIST(stack)  && SHOULD_DRAW(bFullScreen, stack)) PTR(stack)->Draw(GameEngine);
    if (EXIST(lines)  && SHOULD_DRAW(bFullScreen, lines))  PTR(lines)->Draw(GameEngine);
    if (EXIST(editor) && SHOULD_DRAW(bFullScreen, editor)) PTR(editor)->Draw(GameEngine);

    if (mode == NORMAL) return;
    if (cmd.back() == 'q') Draw(Int2Type<Editor::VimT::CMD_q>(), GameEngine);
  }

  void Lock() {
    if (EXIST(bus))    PTR(bus)->Lock();
    if (EXIST(eeprom)) PTR(eeprom)->Lock();
    if (EXIST(stack))  PTR(stack)->Lock();
    if (EXIST(lines))  PTR(lines)->Lock();
    if (EXIST(editor)) PTR(editor)->Lock();
  }

  void Unlock() {
    if (EXIST(bus))    PTR(bus)->Unlock();
    if (EXIST(eeprom)) PTR(eeprom)->Unlock();
    if (EXIST(stack))  PTR(stack)->Unlock();
    if (EXIST(lines))  PTR(lines)->Unlock();
    if (EXIST(editor)) PTR(editor)->Unlock();
  }

private:
  void Draw(Int2Type<Editor::VimT::CMD_q>, PixelGameEngine* GameEngine) {
    auto index = [&](auto tuple) {
      auto color = std::get<0>(tuple) ? AnyType<BLUE, ColorT>::GetValue() : AnyType<RED, ColorT>::GetValue();
      GameEngine->DrawString(POS(tuple), std::string(1, '0' + WINDOW(tuple)), *color, 4);
    };

    if (EXIST(bus))    index(bus);
    if (EXIST(eeprom)) index(eeprom);
    if (EXIST(stack))  index(stack);
    if (EXIST(lines))  index(lines);
    if (EXIST(editor)) index(editor);
  }

public:
  template<int32_t T>
  inline void Command(Int2Type<T>) {}

  inline void Command(Int2Type<Editor::VimT::CMD_z>) {
    if (SELECTED(bus))    { bFullScreen = (ZOOMED(bus)    ^= true); PTR(bus)->Initialize(   bFullScreen ? std::pair(absolute, size) : DIMENSION(bus)); }
    if (SELECTED(eeprom)) { bFullScreen = (ZOOMED(eeprom) ^= true); PTR(eeprom)->Initialize(bFullScreen ? std::pair(absolute, size) : DIMENSION(eeprom)); }
    if (SELECTED(stack))  { bFullScreen = (ZOOMED(stack) ^= true);  PTR(stack)->Initialize( bFullScreen ? std::pair(absolute, size) : DIMENSION(stack)); }
    if (SELECTED(lines))  { bFullScreen = (ZOOMED(lines)  ^= true); PTR(lines)->Initialize( bFullScreen ? std::pair(absolute, size) : DIMENSION(lines)); }
    if (SELECTED(editor)) { bFullScreen = (ZOOMED(editor) ^= true); PTR(editor)->Initialize(bFullScreen ? std::pair(absolute, size) : DIMENSION(editor)); }
  }

  inline void Command(Int2Type<Editor::VimT::CMD_QUESTION>) {
    // TODO: Add menu property
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

  inline void Command(Int2Type<Editor::VimT::CMD_NUMBER>) {
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<PANEL_SELECT_CALLBACK>(), digit()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_q>) {
    int32_t digit = this->digit();
    if (EXIST(bus))    SELECTED(bus)    = WINDOW(bus)    == digit;
    if (EXIST(eeprom)) SELECTED(eeprom) = WINDOW(eeprom) == digit;
    if (EXIST(stack))  SELECTED(stack)  = WINDOW(stack)  == digit;
    if (EXIST(lines))  SELECTED(lines)  = WINDOW(lines)  == digit;
    if (EXIST(editor)) SELECTED(editor) = WINDOW(editor) == digit;
  }

  void Process(Int2Type<NORMAL>, PixelGameEngine* GameEngine) {
    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<Editor::KeyEvent, Panel>::Process(this);

    if (!bUpdated) return;
    else bUpdated = false;
    
    #ifdef DEBUG_MODE
    if (mode == COMMAND) printf("Panel: '%s'\n", cmd.c_str());
    #endif

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

    // noun
    if (match<1>({ ' ' })) { phrase(Int2Type<Editor::VimT::CMD_SPACE>()); return reset(); } 
    if (match<1>({ 'a' })) { phrase(Int2Type<Editor::VimT::CMD_a>()); return reset(); } 
    if (match<1>({ 'd' })) { phrase(Int2Type<Editor::VimT::CMD_d>()); return reset(); } 
    if (match<1>({ 'z' })) { phrase(Int2Type<Editor::VimT::CMD_z>()); return reset(); } 
    if (match<1>({ '?' })) { phrase(Int2Type<Editor::VimT::CMD_QUESTION>()); return reset(); } 

    if (peekPrev() == 'q' && Utils::IsDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_q>()); return reset(); } 
    if (Utils::IsDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_NUMBER>()); return reset(); } 

    // verb
    if (match<1>({ 'q' })) return;

    reset(false);
  }

  template<int32_t T>
  inline void phrase(Int2Type<T> val) {
    auto operation = lambda;
    lambda = [=]() { operation(); Command(val); };
  }

  inline void reset(bool exec = true) {
    if (exec) lambda();

    cmd = ""; nStart = nCurr = 0; lambda = []() {}; 
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
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>)   { EscapeStrokeHandler(olc::Key::BACK); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>)  { EscapeStrokeHandler(olc::Key::ENTER); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) { EscapeStrokeHandler(olc::Key::ENTER); }

private:
  inline void EscapeStrokeHandler(olc::Key key) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(key).bPressed) return;

    bUpdated = true; reset(false);
  }

public:
  std::shared_ptr<Bus::Bus>& Bus() { return PTR(bus); }
  std::shared_ptr<Window::Lines>& Lines() { return PTR(lines); }
  std::shared_ptr<Editor::Editor>& Editor() { return PTR(editor); }
  std::shared_ptr<Bus::Bus::W27C512_T>& EEPROM() { return PTR(eeprom); }
  std::shared_ptr<Bus::Bus::IMS1423_T>& Stack() { return PTR(stack); }

private:
  template<typename T, typename... Args>
  inline void Init(T ref, Args ...args) {  Init(ref); Init(args...); }

  inline void Init() {}
  
  template<int32_t T, int32_t U>
  inline void Init(WindowInitT<Bus::Memory<T, U>> m) { Init(Int2Type<T>(), m); }
  
  template<int32_t T>
  inline void Init(Int2Type<Bus::W27C512>, WindowInitT<Bus::Memory<Bus::W27C512, T>> m) { eeprom = std::tuple_cat(std::make_tuple(nWindows == 1, false), m, std::make_tuple(++nWindows)); }

  template<int32_t T>
  inline void Init(Int2Type<Bus::IMS1423>, WindowInitT<Bus::Memory<Bus::IMS1423, T>> m) { stack = std::tuple_cat(std::make_tuple(nWindows == 1, false), m, std::make_tuple(++nWindows)); }

  inline void Init(WindowInitT<Bus::Bus> b) { bus = std::tuple_cat(std::make_tuple(nWindows == 1, false), b, std::make_tuple(++nWindows)); }
  inline void Init(WindowInitT<Window::Lines> l) { lines = std::tuple_cat(std::make_tuple(nWindows == 1, false), l, std::make_tuple(++nWindows)); }
  inline void Init(WindowInitT<Editor::Editor> e) { editor = std::tuple_cat(std::make_tuple(nWindows == 1, false), e, std::make_tuple(++nWindows)); }

private:
  ModeT mode = NORMAL;
  int32_t nWindows = 0;
  bool bFullScreen = false;

  olc::vi2d absolute = olc::vi2d(0, 0);
  olc::vi2d size = olc::vi2d(0, 0);

  WindowT<Bus::Bus> bus = { false, false, nullptr, {}, 0 };
  WindowT<Window::Lines> lines = { false, false, nullptr, {}, 0 };
  WindowT<Editor::Editor> editor = { false, false, nullptr, {}, 0 };

  WindowT<Bus::Bus::W27C512_T> eeprom = { false, false, nullptr, {}, 0 };
  WindowT<Bus::Bus::IMS1423_T> stack = { false, false, nullptr, {}, 0 };
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