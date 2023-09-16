#pragma once
#include "Bus/Memory.h"
#include "Editor/Editor.h"


template <class T>
using WindowT = std::tuple<bool, std::shared_ptr<T>, DimensionT, int32_t>;

template <class T>
using WindowInitT = std::tuple<bool, std::shared_ptr<T>, DimensionT>;

/**
 * 
 * Grammar:
 *  command     -> <leader> phrase
 * 
 *  phrase      -> (verb DIGIT) | noun | NUMBER
 *  noun        -> '?' | 'z'
 *  verb        -> 'q'
 */
class Panel {
  enum { NORMAL };

public:
  template<typename ...Args>
  Panel(Args ...args) { Init(args...); }

  void Initialize() {
    if (std::get<1>(memory) != nullptr) std::get<1>(memory)->Initialize(std::get<2>(memory));
    if (std::get<1>(lines)  != nullptr) std::get<1>(lines)->Initialize(std::get<2>(lines));
    if (std::get<1>(editor) != nullptr) std::get<1>(editor)->Initialize(std::get<2>(editor));
  }


  void Process(PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Process, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Process, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    Process(Int2Type<NORMAL>(), GameEngine);
    if (cmd.first) { if (!cmd.second.size()) cmd.first = false; return; }

    if (std::get<0>(memory)) std::get<1>(memory)->Process(GameEngine);
    if (std::get<0>(lines))  std::get<1>(lines)->Process(GameEngine);
    if (std::get<0>(editor)) std::get<1>(editor)->Process(GameEngine);
  }

  void Draw(PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Draw, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Draw, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    if (std::get<1>(memory) != nullptr) std::get<1>(memory)->Draw(GameEngine);
    if (std::get<1>(lines)  != nullptr) std::get<1>(lines)->Draw(GameEngine);
    if (std::get<1>(editor) != nullptr) std::get<1>(editor)->Draw(GameEngine);

    if (!cmd.first) return;
    if (cmd.second.back() == 'q') Draw(Int2Type<Editor::VimT::CMD_q>(), GameEngine);
  }

private:
  void Draw(Int2Type<Editor::VimT::CMD_q>, PixelGameEngine* GameEngine) {
    auto index = [&](auto tuple) {
      auto color = std::get<0>(tuple) ? AnyType<BLUE, olc::Pixel>::GetValue() : AnyType<RED, olc::Pixel>::GetValue();
      GameEngine->DrawString(std::get<2>(tuple).first , std::string(1, '0' + std::get<3>(tuple)), color, 4);
    };

    if (std::get<1>(memory) != nullptr) index(memory);
    if (std::get<1>(lines)  != nullptr) index(lines);
    if (std::get<1>(editor) != nullptr) index(editor);
  }

public:
  template<int32_t T>
  inline void Command(Int2Type<T>) {}

  inline void Command(Int2Type<Editor::VimT::CMD_z>) {
    // TODO: Add ability to maximize window
  }

  inline void Command(Int2Type<Editor::VimT::CMD_QUESTION>) {
    // TODO: Add menu property
  }

  inline void Command(Int2Type<Editor::VimT::CMD_NUMBER>) {
    AnyType<-1, PixelGameEngine*>::GetValue()->Event(Int2Type<PANEL_SELECT_CALLBACK>(), digit()); 
  }

  inline void Command(Int2Type<Editor::VimT::CMD_q>) {
    int32_t digit = this->digit();
    if (std::get<1>(memory) != nullptr) std::get<0>(memory) = std::get<3>(memory) == digit;
    if (std::get<1>(lines)  != nullptr) std::get<0>(lines)  = std::get<3>(lines)  == digit;
    if (std::get<1>(editor) != nullptr) std::get<0>(editor) = std::get<3>(editor) == digit;
  }

  void Process(Int2Type<NORMAL>, PixelGameEngine* GameEngine) {
    AnyType<-1, PixelGameEngine*>::GetValue() = GameEngine;
    foreach<Editor::KeyEvent, Panel>::Process(this);

    if (!bUpdated || !cmd.first) return;
    else bUpdated = false;
    
    #ifdef CMD_PRINT
    printf("Panel: %s\n", cmd.second.c_str());
    #endif

    if (nCurr == 0) { nCurr++; return; }

    // noun
    if (match<1>({ 'z' })) { phrase(Int2Type<Editor::VimT::CMD_z>()); return reset(); } 
    if (match<1>({ '?' })) { phrase(Int2Type<Editor::VimT::CMD_QUESTION>()); return reset(); } 

    if (peekPrev() == 'q' && isDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_q>()); return reset(); } 
    if (isDigit(peek())) { phrase(Int2Type<Editor::VimT::CMD_NUMBER>()); return reset(); } 

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

    cmd.second = ""; nStart = nCurr = 0; lambda = []() {}; 
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

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::DEL>, Int2Type<U>>)    { EscapeStrokeHandler(olc::Key::DEL); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::BACK>, Int2Type<U>>)   { EscapeStrokeHandler(olc::Key::BACK); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ENTER>, Int2Type<U>>)  { EscapeStrokeHandler(olc::Key::ENTER); }
  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::ESCAPE>, Int2Type<U>>) { EscapeStrokeHandler(olc::Key::ENTER); }

  template<int32_t U> void Process(TypeList<Int2Type<olc::Key::SPACE>, Int2Type<U>>) { 
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(olc::Key::SPACE).bPressed) return;

    const bool isCtrl = GameEngine->GetKey(olc::Key::CTRL).bHeld;
    if (!isCtrl && !cmd.first) return;

    cmd.first = bUpdated = true; cmd.second += " ";
  }

private:
  inline void EscapeStrokeHandler(olc::Key key) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(key).bPressed || !cmd.first) return;

    bUpdated = true; reset(false);
  }

  inline void BasicStrokeHandler(olc::Key key, const char lower, const char upper) {
    auto GameEngine = AnyType<-1, PixelGameEngine*>::GetValue();
    if (!GameEngine->GetKey(key).bPressed || !cmd.first) return;

    const bool toUpper = GameEngine->GetKey(olc::Key::SHIFT).bHeld;
    const char c = toUpper ? upper : lower;

    bUpdated = true; cmd.second += std::string(1, c); 
  }


  inline const char peek() { return cmd.second[nCurr]; }
  inline const char peek(int32_t nCurr) { return cmd.second[nCurr]; }
  inline const char peekPrev() { return nCurr == 0 ? '\0' : cmd.second[nCurr - 1]; }
  inline bool check(const char c) { return peek() == c; }
  inline bool isDigit(const char &c) { return c >= '0' && c <= '9'; }
  inline bool isAlpha(const char &c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
  inline int32_t digit() { return peek() - '0'; }

  template<int32_t T>
  bool match(std::array<const char, T> str) {
    for (auto& c : str) {
      if (!check(c)) continue;
      nCurr++; return true;
    }

    return false;
  }

private:
  template<typename T, typename... Args>
  inline void Init(T ref, Args ...args) {  Init(ref); Init(args...); }

  inline void Init() {}
  inline void Init(WindowInitT<Bus::Memory> m) { memory = std::tuple_cat(m, std::make_tuple(++nWindows)); }
  inline void Init(WindowInitT<Window::Lines> l) { lines = std::tuple_cat(l, std::make_tuple(++nWindows)); }
  inline void Init(WindowInitT<Editor::Editor> e) { editor = std::tuple_cat(e, std::make_tuple(++nWindows)); }

private:
  bool bUpdated = false;

  int32_t nStart = 0; // index of the cmd, which is pointing to first char in the lexeme
  int32_t nCurr = 0; // index of the cmd, which is pointing to the curr char

  std::function<void(void)> lambda = []() {};
  std::pair<bool, std::string> cmd = { false, "" };

  int32_t nWindows = 0;
  WindowT<Bus::Memory> memory = { false, nullptr, {}, 0 };
  WindowT<Window::Lines> lines = { false, nullptr, {}, 0 };
  WindowT<Editor::Editor> editor = { false, nullptr, {}, 0 };
};