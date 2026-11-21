#pragma once

#include "src/Defs.h"
#include "src/Editor/Vim.h"
#include "src/Window/Types/MenuAction.h"
#include "src/Window/Window.h"
#include "src/Window/Menu.h"
#include <memory>
#include <unordered_map>

namespace Editor {
// #define PATH(e)     e.first
// #define VIM(e)      e.second

class Editor : public Window::KeyEventListener, public Window::Window {
public:
  Editor(PixelGameEngine* engine, std::string id): Window::Base(engine), Window::Window(engine, std::move(id)), ::Window::KeyEventListener(engine) {
    menu = std::make_unique<::Window::Menu>(
      engine,
      ::Window::MenuT::Init({
      { "File", {
        { "Save",     [this](void*) { return Save(this->file), Focus(true), nullptr; } },
        { "Save all", [this](void*) { return Save(), Focus(true), nullptr; } },
      } },
      { "Build", [this](void*) { this->GameEngine->Event(Int2Type<COMPILE_CALLBACK>()); return Focus(true), nullptr; } },
      { "Breakpoints", {
        { "Clear all", [this](void*) { this->GameEngine->Event(Int2Type<RESET_BREAKPOINT_CALLBACK>()); return Focus(true), nullptr; } },
      } },
    }));
  }

  void Reset() {
    for (auto& [_, vim] : files) if (vim) vim->Focus(false);
    files.clear(); file = "";
  }

  Editor* Open(std::filesystem::path path) { 
    if (files.count(path)) return Select(path.string()), this;

    std::ifstream f(path); std::stringstream buf;
    if (f.is_open()) { buf<< f.rdbuf(); f.close(); }

    files[path] = std::make_unique<Vim>(GameEngine); 

    auto context = buf.str();
    return files[path]->Load(buf.str()), Select(path.string()), this;
  }

  inline void Save() { for (auto& path : files) Save(path.first); }
  inline void Save(std::string path) { std::ofstream f(path); f << files[path]->Text(); f.close(); }

  inline void Focus(bool state) override { ::Window::KeyEventListener::Focus(state); menu->Focus(false); files[file]->Focus(state); }
  inline void Select(std::string path) {
    auto it = files.find(file);
    if (it != files.end()) it->second->Focus(false);

    files[file = path]->Focus(!menu->bEnabled);

    const auto offset = olc::vi2d(0, vOffset.y);
    files[file]->Select({ absolute + offset, size - offset - olc::vi2d(10, 0) });
  }

  inline void Select(::Window::DimensionT dimensions) override { ::Window::Window::Select(dimensions); Select(file); }

  void Preprocess() override { menu->Preprocess(); files[file]->Preprocess(); }
  void Process() override {
    const auto mouse = GameEngine->GetMousePos();
    const bool bPressed = GameEngine->GetMouse(0).bPressed;

    if (bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x + vOffset.x && mouse.y < absolute.y + vOffset.y + vStep.y) {
      auto pos = (mouse - absolute) / vStep;

      int32_t acc = 0;
      for (const auto& [path, vim] : files) {
        auto filename = GetTabName(path, 1);
        if (pos.x > acc && pos.x < acc + (int32_t)filename.size() + 1) return Select(file);
        acc += (int32_t)filename.size() + 1;
      }

      return;
    }

    if (menu->bEnabled) menu->Process(); 
    else files[file]->Process();
  }

  void Draw() override {
    files[file]->Draw();

    auto pos = olc::vi2d(absolute.x, absolute.y + vOffset.y - 10);
    for (const auto& [path, vim] : files) {
      auto filename = GetTabName(path, 1);
      auto offset = olc::vi2d((filename.size() + 1) * vStep.x, 14);

      pos.x += 2;

      if (file == path) GameEngine->FillRect(pos, offset, Color::VERY_DARK_GREY);
      GameEngine->DrawLine(pos + olc::vi2d(0, offset.y), pos + offset - olc::vi2d(1, 0), file == path ? Color::GREY : Color::DARK_GREY);
      GameEngine->DrawString(pos + olc::vi2d(4, 4), filename, file == path ? Color::GREY : Color::DARK_GREY);

      pos.x += offset.x;
    }

    menu->Draw();
  }


public:
  ::Window::StrokeAction Process(Int2Type<olc::Key::ESCAPE>) {
    return menu->Reset(), menu->Focus(false), files[file]->Focus(true), nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::OEM_2>) {
    return menu->bEnabled = true, menu->Focus(true), files[file]->Focus(false), nullptr;
  }

public:
  inline void JumpTo(olc::vi2d pos) { files[file]->JumpTo(pos); }
  inline std::string GetTabName(std::filesystem::path path, int32_t offset = 0) {
    auto filename = path.filename().string();
    const int32_t nTabs = static_cast<int32_t>(files.size()) + offset;
    const int32_t nWidth = nTabs > 0 && vStep.x > 0 ? (vOffset.x + size.x) / (nTabs * vStep.x) : 0;

    if (nWidth >= filename.size()) return filename;
    return filename.substr(0, nWidth);
  }

  inline std::string File() { return file; }
  inline void Breakpoint(int32_t line) { files[file]->Breakpoint(line); }
  inline void ResetBreakpoints() { files[file]->ResetBreakpoints(); }

  const ::Window::StrokeT<>& GetCommands() const override { return Editor::commands; }

private:
  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(24, 16);

  std::filesystem::path file = "";
  std::unordered_map<std::filesystem::path, std::unique_ptr<Vim>> files;

  static const ::Window::StrokeT<> commands;
};

inline const Window::StrokeT<> Editor::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::ESCAPE>(), [](void* self, auto... keys) { return Self<Editor>(self)->Process(keys...); } },
  { Int2Type<olc::Key::SHIFT>(), {
    { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return Self<Editor>(self)->Process(keys...); } },
  }}
});

};
