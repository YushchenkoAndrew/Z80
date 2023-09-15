#pragma once
#include "Bus/Memory.h"
#include "Editor/Editor.h"


template <class T>
using WindowT = std::tuple<bool, std::shared_ptr<T>, DimensionT>;

class Panel {

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
  }

private:
  template<typename T, typename... Args>
  inline void Init(T ref, Args ...args) {  Init(ref); Init(args...); }

  inline void Init() {}
  inline void Init(WindowT<Bus::Memory> m) { memory = m; }
  inline void Init(WindowT<Window::Lines> l) { lines = l; }
  inline void Init(WindowT<Editor::Editor> e) { editor = e; }

private:
  WindowT<Bus::Memory> memory = { false, nullptr, {} };
  WindowT<Window::Lines> lines = { false, nullptr, {} };
  WindowT<Editor::Editor> editor = { false, nullptr, {} };
};