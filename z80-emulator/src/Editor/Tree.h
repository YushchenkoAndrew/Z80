#pragma once

#include "include/Typelist.h"
#include "src/Defs.h"
#include "src/Window/KeyEventListener.h"
#include "src/Window/Menu.h"
#include "src/Window/Window.h"
#include <cstdint>

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
    auto src = std::filesystem::path(GameEngine->lua.GetOr("SOURCE", ""));

    menu = std::make_unique<::Window::Menu>(
      engine,
      ::Window::MenuT::Init({
      { "Open", {
        { "File",     { MenuPopulate(*src.begin()) } },
        { "File all", [this, src](void*) { for (const auto& e: std::filesystem::recursive_directory_iterator(*src.begin())) if (e.is_regular_file()) Open(e.path()); return nullptr; } },
      } },
      { "Reload", [this](void*) { this->GameEngine->Event(Int2Type<COMPILE_CALLBACK>(), ""); return nullptr; } },
      // { "Breakpoints", {
      //   { "Clear all", [this](void*) { this->GameEngine->Event(Int2Type<RESET_BREAKPOINT_CALLBACK>()); return Focus(true), nullptr; } },
      // } },
    }));
  }

  ::Window::MenuT MenuPopulate(std::filesystem::path path) {
    ::Window::MenuT root(path.filename().string(), {});

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
      if (entry.is_directory()) {
        auto node = MenuPopulate(entry.path());
        if (!node.children.empty()) root.children.push_back(std::move(node));
      }

      if (entry.is_regular_file()) {
        auto file = entry.path();
        root.children.push_back(::Window::MenuT(entry.path().filename().string(), [this, file](void*) -> ::Window::MenuAction { GameEngine->Event(Int2Type<EDITOR_OPEN_CALLBACK>(), file); return nullptr; }));
      } 
    }

    std::sort(root.children.begin(), root.children.end(), [](const ::Window::MenuT& a, const ::Window::MenuT& b) { return a.name < b.name; });

    return root;
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
    
    return Focus(Select(fs)), this;
  }

  bool IsMouseOver() const {
    auto mouse = GameEngine->GetMousePos();
    const auto pos = absolute + vOffset;

    if (mouse.x < pos.x || mouse.y < pos.y || mouse.x >= pos.x + size.x || mouse.y >= pos.y + size.y) return false;
    return true;
  }

  void Preprocess() override {
    menu->Preprocess();

    if (!IsMouseOver()) return;

    // TODO: Make it better ??
    const auto mouse = GameEngine->GetMousePos();
    auto depth = 0; auto limit = std::max((mouse.y - absolute.y - vOffset.y) / vStep.y, 0) + 1;
    if (auto item = View(&filesystem, limit, depth)) Focus(item);
  }

  void Process() override {
    menu->Process();

    const auto bPressed = GameEngine->GetMouse(0).bPressed;
    if (IsMouseOver() && bPressed && focused && focused->children.empty()) {
      GameEngine->Event(Int2Type<EDITOR_OPEN_CALLBACK>(), Path(Select(focused)));
    }
  }

  NodeT* Focus(NodeT* file) {
    if (focused != nullptr)  focused  = (focused->focused = false, nullptr);
    return focused = (file->focused = true, file);
  }

  inline void Focus(bool state) override { ::Window::KeyEventListener::Focus(state); menu->Focus(false); }

  NodeT* Select(NodeT* file) {
    if (selected != nullptr) selected = (selected->selected = false, nullptr);
    return selected = (file->selected = true, file);
  }

  inline void Select(::Window::DimensionT dimensions) override { ::Window::Window::Select(dimensions); }

  void Draw() override {
    auto pos = absolute + vOffset;
    DrawNode(filesystem, pos); menu->Draw();

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

public:
  ::Window::StrokeAction Process(Int2Type<olc::Key::ESCAPE>) {
    return menu->Reset(), menu->Focus(false), nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::SHIFT>, Int2Type<olc::Key::OEM_2>) {
    return menu->bEnabled = true, menu->Focus(true), nullptr;
  }

  // TODO: Think about how to use opened flag
  ::Window::StrokeAction Process(Int2Type<olc::Key::K>) {
    if (menu->bEnabled || focused == nullptr || focused->parent == nullptr || focused->parent->name.empty()) return nullptr;

    auto& siblings = focused->parent->children;
    auto it = std::find_if(siblings.begin(), siblings.end(), [&](const NodeT& node) { return node.name == focused->name; });
    if (it == siblings.end()) return nullptr;

    focused->focused = false;

    if (it == siblings.begin()) focused = focused->parent;
    else focused = Last(&*--it);

    return focused->focused = true, nullptr;
  }

  ::Window::StrokeAction Process(Int2Type<olc::Key::J>) {
    if (menu->bEnabled || focused == nullptr) return nullptr;

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
    if (menu->bEnabled || focused == nullptr) return nullptr;
    return focused->opened = false, nullptr;
  }

  // Fix issue that when file is opend the vim is focused
  // Fix me when the file is changed the Editor has a Segmentation error on menu open through Shift+?
  ::Window::StrokeAction Process(Int2Type<olc::Key::L>) {
    if (menu->bEnabled || focused == nullptr) return nullptr;
    if (focused->children.empty()) GameEngine->Event(Int2Type<EDITOR_OPEN_CALLBACK>(), Path(Select(focused)));

    return  focused->opened = true, nullptr;
  }

public:
  inline std::string File() { return Path(selected); }
  const ::Window::StrokeT<>& GetCommands() const override { return Tree::commands; }

private:
  NodeT* View(NodeT* fs, int32_t limit, int32_t& depth) { if (limit == depth) return fs;

    for (auto& node : fs->children) {
      if (auto n = View(&node, limit, ++depth)) return n;
    }

    return nullptr;
  }

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
  { Int2Type<olc::Key::ESCAPE>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  { Int2Type<olc::Key::SHIFT>(), {
    { Int2Type<olc::Key::OEM_2>(), [](void* self, auto... keys) { return Self<Tree>(self)->Process(keys...); } },
  }}
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