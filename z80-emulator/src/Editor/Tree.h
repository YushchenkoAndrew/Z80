#pragma once

#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Window.h"

namespace Editor {

class Tree : public Window::KeyEventListener, public Window::Window {
  struct NodeT {
     NodeT(): name(""), parent(nullptr) {}
     NodeT(std::filesystem::path name, NodeT* parent): name(name), parent(parent) {}

    bool opened = true;
    bool focused = false;
    bool selected = false;

    NodeT* parent;
    std::filesystem::path name;
    std::map<std::filesystem::path, NodeT> children;
  };

public:
  Tree(PixelGameEngine* engine, std::string id): Window::Base(engine), Window::Window(engine, std::move(id)), ::Window::KeyEventListener(engine) {
    // menu = std::make_unique<::Window::Menu>(
    //   engine,
    //   ::Window::MenuT::Init({
    //   { "File", {
    //     { "Save",     [this](void*) { return Save(this->file), Focus(true), nullptr; } },
    //     { "Save all", [this](void*) { return Save(), Focus(true), nullptr; } },
    //   } },
    //   { "Build", [this](void*) { this->GameEngine->Event(Int2Type<COMPILE_CALLBACK>()); return Focus(true), nullptr; } },
    //   { "Breakpoints", {
    //     { "Clear all", [this](void*) { this->GameEngine->Event(Int2Type<RESET_BREAKPOINT_CALLBACK>()); return Focus(true), nullptr; } },
    //   } },
    // }));
  }

  void Reset() { Reset(filesystem); }
  void Reset(NodeT& fs) { for (auto& [_, fs]: fs.children) { Reset(fs); fs.children.clear(); } }

  Tree* Open(std::filesystem::path path) { 
    NodeT* fs = &filesystem;
    for (auto& name : path) {
      if (name == "." || name.empty()) continue;
      if (!fs->children.count(name)) fs->children.emplace(name, NodeT(name, fs));

      fs = &fs->children[name];
    }
    
    return Select(fs->parent), this;
  }

  bool IsMouseOver() const {
    auto mouse = GameEngine->GetMousePos();

    // ?? need to know the size of children recursively so could figure out the lines available
    // // const auto item = View(path.size());
    // if (!item.items || !item.rows) return false;
    // if (mouse.x < item.position.x || mouse.y < item.position.y || mouse.x >= item.position.x + item.size.x || mouse.y >= item.position.y + item.size.y) return false;

    return false;
  }

  void Preprocess() override {
    if (!IsMouseOver()) return;

    const auto mouse = GameEngine->GetMousePos();
    // const auto item = View(path.size());

    // TODO: Make the line focused somehow ???
    // line = (mouse.y - item.position.y) / vStep.y;
  }

  void Select(NodeT* file) {
    if (focused != nullptr) focused->focused = false;
    if (selected != nullptr) selected = focused = (selected->selected = false, nullptr);
    if (file != nullptr) selected = focused = (file->focused = file->selected = true, file);
  }

  inline void Select(::Window::DimensionT dimensions) override { ::Window::Window::Select(dimensions); }

  void Draw() override {
    auto pos = absolute + vOffset;
    DrawNode(filesystem, pos);
  }

  void DrawNode(NodeT& fs, olc::vi2d& pos) {
    for (auto& [name, node] : fs.children) {
      std::string prefix = node.children.empty() ? "" : node.opened ? "+" : ">";
      if (fs.focused) GameEngine->FillRect(olc::vi2d(absolute.x + vOffset.x, pos.y - 2), olc::vi2d(size.x, vStep.y - 1), Color::VERY_DARK_GREY);
      if (fs.selected) GameEngine->FillRect(olc::vi2d(absolute.x + vOffset.x, pos.y - 2), olc::vi2d(size.x, vStep.y - 1), Color::DARK_GREY);

      GameEngine->DrawString(pos, prefix, Color::DARK_GREY);
      GameEngine->DrawString(pos + olc::vi2d(vStep.x + 2, 0), name, fs.selected ? Color::WHITE : Color::GREY);
      pos.y += vStep.y;

      if (node.children.empty()) continue;
      auto copy = pos + olc::vi2d(vStep.x, 0); DrawNode(node, copy);
      GameEngine->DrawLine(pos + olc::vi2d(2, -2), olc::vi2d(pos.x + 2, copy.y - 4), Color::DARK_GREY); pos.y = copy.y;
    }
  }

  // ? Think about how to impl this ????
  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) { return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) { return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::H>) { return nullptr; }
  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) { return nullptr; }

public:
  inline std::string File() { return Path(selected); }
  const ::Window::StrokeT<>& GetCommands() const override { return Tree::commands; }

private:
  std::filesystem::path Path(const NodeT* node) {
    if (selected == nullptr) return "";

    std::filesystem::path path;
    while (node && node != &filesystem) {
      path = node->name / path;
      node = node->parent;
    }

    return path;
  }

private:
  const olc::vi2d vStep = olc::vi2d(8, 12);
  const olc::vi2d vOffset = olc::vi2d(6, 20);

  NodeT filesystem;
  NodeT* focused = nullptr;
  NodeT* selected = nullptr;

  static const ::Window::StrokeT<> commands;
};

inline const Window::StrokeT<> Tree::commands = ::Window::StrokeT<>::Init({
  { Int2Type<olc::Key::K>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  { Int2Type<olc::Key::J>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  { Int2Type<olc::Key::H>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  { Int2Type<olc::Key::L>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  // { Int2Type<olc::Key::G>(), {
  //   { Int2Type<olc::Key::G>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  // }},
  // { Int2Type<olc::Key::SHIFT>(), {
  //   { Int2Type<olc::Key::G>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  // }},
  // { Int2Type<olc::Key::CTRL>(), {
  //   { Int2Type<olc::Key::A>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  // }},
});

};