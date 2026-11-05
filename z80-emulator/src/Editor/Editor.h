#pragma once

#include "src/Defs.h"
#include "src/Editor/Vim.h"
#include "src/Window/Window.h"

namespace Editor {
#define PATH(e)     e.first
#define VIM(e)      e.second

class Editor : public Window::Window {
public:
  typedef std::pair<std::string, Vim> FileT;

  Editor(PixelGameEngine* engine, std::string id): Window::Base(engine), Window::Window(engine, std::move(id)) {}

  Editor* Open(std::string path) { 
    for (int32_t i = 0; i < files.size(); i++) {
      if (PATH(files[i]) == path) { nFile = i; return this; }
    }

    std::ifstream f(path); std::stringstream buf;
    if (f.is_open()) { buf<< f.rdbuf(); f.close(); }

    files.push_back(std::pair(path, Vim(GameEngine)));

    auto& tab = files.back(); nFile = files.size() - 1;
    auto context = buf.str();

    return VIM(tab).Load(buf.str()), this;
  }

  inline void Save() { for (auto& tab : files) { std::ofstream f(PATH(tab)); f << VIM(tab).Text(); f.close(); } }
  // inline void SaveFile() { std::ofstream f(PATH(files[nFile])); f << VIM(files[nFile]).Text(); f.close(); }

  inline void Focus(bool state) override { VIM(files[nFile]).Focus(state); }
  inline void SelectFile() { VIM(files[nFile]).Select({ absolute, size }); }
  inline void Select(DimensionT dimensions) override {
    this->absolute = dimensions.first; this->size = dimensions.second - vOffset; SelectFile();
  }

  const ::Window::StrokeT<>& GetCommands() const override { return VIM(files[nFile]).GetCommands(); }
    
  void Process() override {
    const auto mouse = GameEngine->GetMousePos();
    const bool bPressed = GameEngine->GetMouse(0).bPressed;

    if (bPressed && mouse.x > absolute.x && mouse.y > absolute.y && mouse.x < absolute.x + size.x + vOffset.x && mouse.y < absolute.y + vOffset.y + vStep.y) {
      auto pos = (mouse - absolute) / vStep;

      for (int32_t i = 0, acc = 0; i < files.size(); i++) {
        auto filename = GetTabName(files[i], 1);
        if (pos.x > acc && pos.x < acc + (int32_t)filename.size() + 1) return nFile = i, SelectFile();
        acc += (int32_t)filename.size() + 1;
      }

      return;
    }

    VIM(files[nFile]).Process();
  }

  // void Highlight(PixelGameEngine* GameEngine) {
  //   Utils::Lock l(mutex);

  //   auto pos = olc::vi2d(absolute.x, absolute.y + vOffset.y + vStep.y);
  //   for (int32_t i = 0; i < tabs.size(); i++) {
  //     auto filename = GetTabName(tabs[i], 1);
  //     auto offset = olc::vi2d((filename.size() + 1) * vStep.x, 14);

  //     pos.x += 2;
  //     const auto& color = nTab == i ? Color::BLUE : Color::RED;
  //     GameEngine->DrawString(pos, std::string(1, '0' + i + 1), color, 3);

  //     pos.x += offset.x;
  //   }
  // }

  void Draw() override {
    VIM(files[nFile]).Draw();

    auto pos = olc::vi2d(absolute.x, absolute.y + vOffset.y - 10);
    for (int32_t i = 0; i < files.size(); i++) {
      auto filename = GetTabName(files[i], 1);
      auto offset = olc::vi2d((filename.size() + 1) * vStep.x, 14);

      pos.x += 2;
      if (nFile == i) {
        GameEngine->FillRect(pos, offset, Color::VERY_DARK_GREY);

        GameEngine->DrawLine(pos + olc::vi2d(0, offset.y), pos + offset - olc::vi2d(1, 0), Color::GREY);
        GameEngine->DrawString(pos + olc::vi2d(4, 4), filename, Color::GREY);
      } else {
        GameEngine->DrawLine(pos + olc::vi2d(0, offset.y), pos + offset - olc::vi2d(1, 0), Color::DARK_GREY);
        GameEngine->DrawString(pos + olc::vi2d(4, 4), filename, Color::DARK_GREY);
      }

      pos.x += offset.x;
    }
  }

public:
  inline void MoveTo(olc::vi2d next) { VIM(files[nFile]).MoveTo(next - VIM(files[nFile]).GetPos()); }
  inline std::string GetTabName(FileT& tab, int32_t offset = 0) {
    auto filename = std::filesystem::path(PATH(tab)).filename().string();
    const int32_t nWidth = (vOffset.x + size.x) / (((int32_t)files.size() + offset) * vStep.x);

    if (nWidth >= filename.size()) return filename;
    return filename.substr(0, nWidth);
  }

  inline std::string File() { return PATH(files[nFile]); }
  inline void Breakpoint(int32_t line) { VIM(files[nFile]).Breakpoint(line); }

private:
  olc::vi2d size = olc::vi2d(0, 0);
  olc::vi2d absolute = olc::vi2d(0, 0);

  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(24, 16);

  uint8_t nFile = 0;
  std::vector<FileT> files;
};

};
