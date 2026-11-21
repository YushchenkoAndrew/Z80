#pragma once

#include "include/Typelist.h"
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
    std::list<NodeT> children;
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
  void Reset(NodeT& fs) { for (auto& fs: fs.children) { Reset(fs); fs.children.clear(); } }

  Tree* Open(std::filesystem::path path) { 
    NodeT* fs = &filesystem;
    for (auto& name : path) {
      if (name == "." || name.empty()) continue;

      NodeT* it = nullptr; for (auto& child : fs->children) if (child.name == name) { it = &child; break; }
      if (it == nullptr) fs->children.push_back(NodeT(name, fs));

      // ????
      // children.sort([](const NodeT& a, const NodeT& b) { return a.name.string() < b.name.string(); });
      fs = it == nullptr ? &fs->children.back() : it;
    }
    
    return Select(*fs), this;
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

  void Select(NodeT& file) {
    if (focused != nullptr)  focused  = (focused->focused   = false, nullptr);
    if (selected != nullptr) selected = (selected->selected = false, nullptr);

    selected = focused = (file.focused = file.selected = true, &file);
  }

  inline void Select(::Window::DimensionT dimensions) override { ::Window::Window::Select(dimensions); }

  void Draw() override {
    auto pos = absolute + vOffset;
    DrawNode(filesystem, pos);
  }

  void DrawNode(NodeT& fs, olc::vi2d& pos) {
    for (auto& node : fs.children) {
      std::string prefix = node.children.empty() ? "" : node.opened ? "+" : ">";
      if (node.focused) GameEngine->FillRect(olc::vi2d(absolute.x + vOffset.x, pos.y - 2), olc::vi2d(size.x, vStep.y - 1), Color::VERY_DARK_GREY);
      if (node.selected) GameEngine->FillRect(olc::vi2d(absolute.x + vOffset.x, pos.y - 2), olc::vi2d(size.x, vStep.y - 1), Color::DARK_GREY);

      GameEngine->DrawString(pos, prefix, Color::DARK_GREY);
      GameEngine->DrawString(pos + olc::vi2d(vStep.x + 2, 0), node.name, node.selected ? Color::WHITE : Color::GREY);
      pos.y += vStep.y;

      if (node.children.empty() || !node.opened) continue;
      auto copy = pos + olc::vi2d(vStep.x, 0); DrawNode(node, copy);
      GameEngine->DrawLine(pos + olc::vi2d(2, -2), olc::vi2d(pos.x + 2, copy.y - 4), Color::DARK_GREY); pos.y = copy.y;
    }
  }

  NodeT* Last(NodeT* node) {
    if (node->children.empty()) return node;
    else return Last(&node->children.back());
  };

  // TODO: Think about how to use opened flag
  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) {
    if (focused == nullptr || focused->parent == nullptr || focused->parent->name.empty()) return nullptr;

    auto& siblings = focused->parent->children;
    auto it = std::find_if(siblings.begin(), siblings.end(), [&](const NodeT& node) { return node.name == focused->name; });
    if (it == siblings.end()) return nullptr;

    focused->focused = false;

    if (it == siblings.begin()) focused = focused->parent;
    else focused = Last(&*--it);

    return focused->focused = true, nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) {
    if (focused == nullptr) return nullptr;

    NodeT* node = focused;
    focused->focused = false;

    if (!node->children.empty()) focused = &node->children.front();
    else {
      while (node->parent != nullptr) {
        auto& siblings = node->parent->children;
        auto it = std::find_if(siblings.begin(), siblings.end(), [node](const NodeT& n) { return &n == node; });

        if (it != siblings.end() && ++it != siblings.end()) { focused = &*it; break; }

        node = node->parent;
      }
    }

    return focused->focused = true, nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::H>) {
    if (focused == nullptr) return nullptr;
    return focused->opened = false, nullptr;
  }

  // Fix issue that when file is opend the vim is focused
  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) {
    if (focused == nullptr) return nullptr;
    if (focused->children.empty()) GameEngine->Event(Int2Type<EDITOR_OPEN_CALLBACK>(), Path(focused));

    return focused->opened = true, nullptr;
  }

public:
  inline std::string File() { return Path(selected); }
  const ::Window::StrokeT<>& GetCommands() const override { return Tree::commands; }

private:
  std::filesystem::path Path(const NodeT* node) const {
    std::filesystem::path path = node->name;
    while ((node = node->parent)) path = node->name / path;
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