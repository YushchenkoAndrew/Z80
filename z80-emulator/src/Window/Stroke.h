#pragma once

#include "include/Typelist.h"
#include "lib/olcPixelGameEngine.h"

namespace Window {

typedef std::function<void(olc::Key)> StrokeAction;
typedef std::function<StrokeAction(void*)> StrokeCallback;

template <olc::Key... CompiledKeys>
struct StrokeT {
  
  // 1. Leaf Constructor (Bakes full compile-time path into the functional call)
  template <int32_t K, typename Func>
  StrokeT(Int2Type<K>, Func&& lambda) : key(static_cast<olc::Key>(K)) {
    func = [lambda = std::forward<Func>(lambda)](void* instance) {
        // Expands parameters as rvalues alongside the execution instance
        return lambda(instance, Int2Type<static_cast<int>(CompiledKeys)>{}..., Int2Type<K>{});
    };
  }

  // 2. Branch Constructor (Assembles nested braced structures seamlessly)
  template<int32_t K>
  StrokeT(Int2Type<K>, std::initializer_list<StrokeT<CompiledKeys..., static_cast<olc::Key>(K)>> list) 
      : key(static_cast<olc::Key>(K)) {
    for (const auto& item : list) {
        // Deep copy nested elements into our clean type-erased base container maps
        StrokeT<> runtimeNode;
        runtimeNode.key = item.key;
        runtimeNode.func = std::move(item.func);         // Safely transfer the function ownership
        runtimeNode.children = std::move(item.children); // Safely transfer children maps

        children.insert({ item.key, std::move(runtimeNode) });
    }
  }

  // Standard boilerplate for object assignment inside containers
  StrokeT() = default;
  StrokeT(const StrokeT&) = default;
  StrokeT(StrokeT&&) = default;
  StrokeT& operator=(const StrokeT&) = default;

  // 3. Entry point builder method
  static StrokeT<> Init(std::initializer_list<StrokeT<>> list) {
    StrokeT<> root;
    root.key = olc::Key::NONE;
    for (const auto& item : list) {
      root.children.insert({item.key, item});
    }
    return root;
  }

  olc::Key key;
  StrokeCallback func = nullptr;
  std::unordered_map<olc::Key, StrokeT<>> children; 
};

};