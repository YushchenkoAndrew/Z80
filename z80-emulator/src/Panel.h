#pragma once
#include "Bus/Rom.h"
#include "Editor/Editor.h"


template <class T>
using WindowT = std::tuple<bool, std::shared_ptr<T>, DimensionT>;

class Panel {

public:
  template<typename ...Args>
  Panel(Args ...args) { Init(args...); }

  void Initialize() {
    if (std::get<1>(rom) != nullptr) std::get<1>(rom)->Initialize(std::get<2>(rom));
    if (std::get<1>(editor) != nullptr) std::get<1>(editor)->Initialize(std::get<2>(editor));
  }


  void Process(olc::PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Process, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Process, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    if (std::get<0>(rom)) std::get<1>(rom)->Process(GameEngine);
    if (std::get<0>(editor)) std::get<1>(editor)->Process(GameEngine);
  }

  void Draw(olc::PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Draw, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Draw, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    if (std::get<1>(rom) != nullptr) std::get<1>(rom)->Draw(GameEngine);
    if (std::get<1>(editor) != nullptr) std::get<1>(editor)->Draw(GameEngine);
  }

private:
  template<typename T, typename... Args>
  inline void Init(T ref, Args ...args) {  Init(ref); Init(args...); }

  inline void Init() {}
  inline void Init(WindowT<Bus::Rom> r) { rom = r; }
  inline void Init(WindowT<Editor::Editor> e) { editor = e; }

private:
  WindowT<Bus::Rom> rom = { false, nullptr, {} };
  WindowT<Editor::Editor> editor = { false, nullptr, {} };
};