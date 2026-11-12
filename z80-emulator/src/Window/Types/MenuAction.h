#pragma once

#include "lib/olcPixelGameEngine.h"

namespace Window {

typedef std::function<bool(int32_t)> MenuAction;
typedef std::function<MenuAction(void*)> MenuCallback;

struct MenuT {

  MenuT(std::string name, MenuCallback callback): name(std::move(name)), func(std::move(callback)) {}
  MenuT(std::string name, std::initializer_list<MenuT> list): name(std::move(name)), children(list) {}
  
  MenuT() = default;
  MenuT(const MenuT&) = default;
  MenuT(MenuT&&) noexcept = default;
  MenuT& operator=(const MenuT&) = default;
  MenuT& operator=(MenuT&&) noexcept = default;

  static MenuT Init(std::initializer_list<MenuT> list) {
    return MenuT("root", list);
  }

  std::string name;
  MenuCallback func = nullptr;
  std::vector<MenuT> children;
};

};