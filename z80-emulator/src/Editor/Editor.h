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
  typedef std::pair<std::filesystem::path, Vim> FileT;

  Editor(PixelGameEngine* engine, std::string id): Window::Base(engine), Window::Window(engine, std::move(id)), ::Window::KeyEventListener(engine) {
    bindings = ::Window::StrokeT<>::Init({
      { Int2Type<olc::Key::ESCAPE>(), [](void* self, auto... keys) { return static_cast<Editor*>(self)->menu->Reset(), nullptr; } },
      { Int2Type<olc::Key::SHIFT>(), {
        { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return static_cast<Editor*>(self)->Menu(), nullptr; } },
      }}
    });

    menu = std::make_unique<::Window::Menu>(
      engine,
      ::Window::MenuT::Init({
      { "File", {
        { "Save",     [this](void*) { return Save(this->file), nullptr; } },
        { "Save all", [this](void*) { return Save(), nullptr; } },
      } },
      { "Build", [this](void*) { this->GameEngine->Event(Int2Type<COMPILE_CALLBACK>()); return nullptr; } },
      { "Breakpoints", {
        { "Clear all", [this](void*) { this->GameEngine->Event(Int2Type<RESET_BREAKPOINT_CALLBACK>()); return nullptr; } },
      } },
    }));
  }

  void Reset() { files.clear(); file = ""; }
  Editor* Open(std::filesystem::path path) { 
    if (files.count(path)) return file = path, Select(file), this;

    std::ifstream f(path); std::stringstream buf;
    if (f.is_open()) { buf<< f.rdbuf(); f.close(); }

    files[file = path] = std::make_unique<Vim>(GameEngine); 

    auto context = buf.str();
    return files[path]->Load(buf.str()), this;
  }

  inline void Save() { for (auto& path : files) Save(path.first); }
  inline void Save(std::string path) { std::ofstream f(path); f << files[path]->Text(); f.close(); }

  inline void Focus(bool state) override { ::Window::KeyEventListener::Focus(state); menu->Focus(state); files[file]->Focus(state); }
  inline void Select(std::string path) { const auto offset = olc::vi2d(0, vOffset.y); files[path]->Select({ absolute + offset, size - offset - olc::vi2d(10, 0) }); }
  inline void Select(::Window::DimensionT dimensions) override { ::Window::Window::Select(dimensions); Select(file); }

  const ::Window::StrokeT<>& GetCommands() const override {
    auto it = files.find(file);
    if (it != files.end() && it->second) return it->second->GetCommands();
  }

  void Preprocess() override { menu->Preprocess(); files[file]->Preprocess(); }
  void Process() override {
    const auto mouse = GameEngine->GetMousePos();
    const bool bPressed = GameEngine->GetMouse(0).bPressed;

    if (bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x + vOffset.x && mouse.y < absolute.y + vOffset.y + vStep.y) {
      auto pos = (mouse - absolute) / vStep;

      int32_t acc = 0;
      for (const auto& [path, vim] : files) {
        auto filename = GetTabName(path, 1);
        if (pos.x > acc && pos.x < acc + (int32_t)filename.size() + 1) return file = path, Select(file);
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
  inline void JumpTo(olc::vi2d pos) { files[file]->JumpTo(pos); }
  inline std::string GetTabName(std::filesystem::path path, int32_t offset = 0) {
    auto filename = std::filesystem::path(path).filename().string();
    const int32_t nWidth = (vOffset.x + size.x) / (((int32_t)files.size() + offset) * vStep.x);

    if (nWidth >= filename.size()) return filename;
    return filename.substr(0, nWidth);
  }

  inline std::string File() { return file; }
  inline void Breakpoint(int32_t line) { files[file]->Breakpoint(line); }
  inline void ResetBreakpoints() { files[file]->ResetBreakpoints(); }

private:
  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(24, 16);

  std::filesystem::path file = "";
  std::unordered_map<std::filesystem::path, std::unique_ptr<Vim>> files;
};

};
