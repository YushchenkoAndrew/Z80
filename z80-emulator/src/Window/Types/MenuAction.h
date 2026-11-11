#pragma once

#include "lib/olcPixelGameEngine.h"

namespace Window {

typedef std::function<bool(int32_t)> MenuAction;
typedef std::function<MenuAction(void*)> MenuCallback;

struct MenuT {
  
  // 1. Leaf Constructor (Bakes full compile-time path into the functional call)
  template <typename Func>
  MenuT(Func&& lambda) {
    func = [lambda = std::forward<Func>(lambda)](void* instance) mutable { return lambda(instance); };
  }

  // 2. Branch Constructor (Assembles nested braced structures seamlessly)
  MenuT(std::initializer_list<MenuT> list) {
    for (const auto& item : list) {
        // Deep copy nested elements into our clean type-erased base container maps
        MenuT node;
        node.name = item.name;
        node.func = std::move(item.func);         // Safely transfer the function ownership
        node.children = std::move(item.children); // Safely transfer children maps

        children.push_back(std::move(node));
    }
  }

  // Standard boilerplate for object assignment inside containers
  MenuT() = default;
  MenuT(const MenuT&) = default;
  MenuT(MenuT&&) = default;
  MenuT& operator=(const MenuT&) = default;

  // 3. Entry point builder method
  static MenuT Init(std::initializer_list<MenuT> list) {
    MenuT root;
    root.name = "root";
    for (const auto& item : list) {
      root.children.push_back(item);
    }
    return root;
  }

  std::string name;
  MenuCallback func = nullptr;
  std::vector<MenuT> children; 
};

};