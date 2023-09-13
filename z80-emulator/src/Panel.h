#pragma once
#include "Bus/Rom.h"
#include "Editor/Editor.h"


template <class T>
using WindowT = std::pair<std::shared_ptr<T>, DimensionT>;

class Panel {

public:
  template<typename ...Args>
  Panel(Args ...args) { Init(args...); }

  void Initialize() {
    if (rom.first != nullptr) rom.first->Initialize(rom.second);
    if (editor.first != nullptr) editor.first->Initialize(editor.second);
  }


  void Process(olc::PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Process, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Process, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    if (rom.first != nullptr) rom.first->Process(GameEngine);
    if (editor.first != nullptr) editor.first->Process(GameEngine);
  }

  void Draw(olc::PixelGameEngine* GameEngine) {
    // TODO: For some reason this, is much slower, maybe I'll use it in a future, ha pan not intended
    // std::vector<std::shared_future<void>> vFuture;

    // if (rom.first != nullptr) vFuture.push_back(std::async(&Bus::Rom::Draw, rom.first, GameEngine));
    // if (editor.first != nullptr) vFuture.push_back(std::async(&Editor::Editor::Draw, editor.first, GameEngine)); 

    // for(auto& future : vFuture) future.wait();

    if (rom.first != nullptr) rom.first->Draw(GameEngine);
    if (editor.first != nullptr) editor.first->Draw(GameEngine);
  }

private:
  template<typename T, typename... Args>
  inline void Init(T ref, Args ...args) {  Init(ref); Init(args...); }

  inline void Init() {}
  inline void Init(WindowT<Bus::Rom> r) { rom = r; }
  inline void Init(WindowT<Editor::Editor> e) { editor = e; }

private:
  WindowT<Bus::Rom> rom = { nullptr, {} };
  WindowT<Editor::Editor> editor = { nullptr, {} };
};