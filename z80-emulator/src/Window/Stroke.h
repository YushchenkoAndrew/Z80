#pragma once
#include "Window.h"
#include "include/Typelist.h"
#include "src/Defs.h"
#include <cstdint>
#include <cstdio>
#include <functional>


namespace Window {

// template <olc::Key... CompiledKeys>
// struct StrokeT {
//   // 1. Leaf Constructor: Triggers for [this](auto... keys) { ... }
//   template <int32_t K, typename Func>
//   StrokeT(Int2Type<K>, Func&& lambda): key((olc::Key)K) {
//     func = [lambda = std::forward<Func>(lambda)]() {
//         // Expands the key path into separate sequential Int2Type<int> instances
//         lambda(Int2Type<(int)CompiledKeys>()..., Int2Type<K>());
//     };
//   }

//   // 2. Branch Constructor: Triggers for nested braces { Key, { ... } }
//   template<int32_t K>
//   StrokeT(Int2Type<K>, std::initializer_list<StrokeT<CompiledKeys..., (olc::Key)K>> list): key((olc::Key)K) {
//     for (const auto& item : list) children.insert({ item.key, item });
//   }

//   // Default constructors required for standard map assignments
//   StrokeT() = default;
//   StrokeT(const StrokeT&) = default;
//   StrokeT(StrokeT&&) = default;
//   StrokeT& operator=(const StrokeT&) = default;

//   // 3. Entry point builder method
//   static StrokeT<> Init(std::initializer_list<StrokeT<>> list) {
//     StrokeT<> root;
//     root.key = olc::Key::NONE;

//     for (const auto& item : list) {
//       root.children.insert({item.key, item});
//     }

//     return root;
//   }

//   olc::Key key;
//   std::function<void()> func = nullptr;
//   std::unordered_map<olc::Key, StrokeT<>> children; // Runtime nodes always erase compile types
// };

// template <olc::Key... CompiledKeys>
// struct StrokeT {
//   // 1. Leaf Constructor: Triggers for [this](auto... keys) { ... }
//   template <int32_t K, typename Func>
//   StrokeT(Int2Type<K>, Func&& lambda) : key((olc::Key)K) {
//     func = [lambda = std::forward<Func>(lambda)]() {
//         lambda(Int2Type<(int)CompiledKeys>()..., Int2Type<K>());
//     };
//   }

//   // 2. Branch Constructor: Triggers for nested braces { Key, { ... } }
//   template<int32_t K>
//   StrokeT(Int2Type<K>, std::initializer_list<StrokeT<CompiledKeys..., (olc::Key)K>> list) : key((olc::Key)K) {
//     for (const auto& item : list) {
//       // FIX: Explicit conversion constructor creates a type-erased StrokeT<> from the nested item
//       children.insert(std::pair<const olc::Key, StrokeT<>>{ item.key, StrokeT<>(item) });
//     }
//   }

//   // FIX: Conversion Constructor allowing StrokeT<SomeKeys...> to convert to StrokeT<OtherKeys...> (e.g. StrokeT<>)
//   template <olc::Key... OtherKeys>
//   StrokeT(const StrokeT<OtherKeys...>& other) 
//     : key(other.key), func(other.func), children(other.children) {}

//   // Standard constructors
//   StrokeT() = default;
//   StrokeT(const StrokeT&) = default;
//   StrokeT(StrokeT&&) = default;
//   StrokeT& operator=(const StrokeT&) = default;

//   // 3. Entry point builder method
//   static StrokeT<> Init(std::initializer_list<StrokeT<>> list) {
//     StrokeT<> root;
//     root.key = olc::Key::NONE;

//     for (const auto& item : list) {
//       root.children.insert(std::pair<const olc::Key, StrokeT<>>{item.key, item});
//     }

//     return root;
//   }

//   olc::Key key;
//   std::function<void()> func = nullptr;
//   std::unordered_map<olc::Key, StrokeT<>> children; 
// };

// template <olc::Key... CompiledKeys>
// struct StrokeT {
  
//   // 1. Leaf Constructor
//   template <int32_t K, typename Func>
//   StrokeT(Int2Type<K>, Func&& lambda) : key(static_cast<olc::Key>(K)) {
//     func = [lambda = std::forward<Func>(lambda)]() {
//         // Force evaluation by value (strips lvalue references) by casting to temporary values
//         lambda(Int2Type<static_cast<int>(CompiledKeys)>{}..., Int2Type<K>{});
//     };
//   }

//   // 2. Branch Constructor
//   template<int32_t K>
//   StrokeT(Int2Type<K>, std::initializer_list<StrokeT<CompiledKeys..., static_cast<olc::Key>(K)>> list) 
//       : key(static_cast<olc::Key>(K)) {
//     for (const auto& item : list) {
//         // Explicitly format item to base-type mapping to clear conversion ambiguity
//         StrokeT<> runtimeNode;
//         runtimeNode.key = item.key;
//         runtimeNode.func = item.func;
//         runtimeNode.children = item.children;
//         children.insert({ item.key, runtimeNode });
//     }
//   }

//   // Default constructors
//   StrokeT() = default;
//   StrokeT(const StrokeT&) = default;
//   StrokeT(StrokeT&&) = default;
//   StrokeT& operator=(const StrokeT&) = default;

//   // 3. Entry point builder method
//   static StrokeT<> Init(std::initializer_list<StrokeT<>> list) {
//     StrokeT<> root;
//     root.key = olc::Key::NONE;
//     for (const auto& item : list) {
//       root.children.insert({item.key, item});
//     }
//     return root;
//   }

//   olc::Key key;
//   std::function<void()> func = nullptr;
//   std::unordered_map<olc::Key, StrokeT<>> children; 
// };

typedef std::function<std::function<void(olc::Key)>(void*)> StrokeAction;

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

  //   // 2. Branch Constructor
  // template<int32_t K>
  // StrokeT(Int2Type<K>, std::initializer_list<StrokeT<CompiledKeys..., static_cast<olc::Key>(K)>> list) 
  //     : key(static_cast<olc::Key>(K)) {
  //   for (const auto& item : list) {
  //       // 1. Manually strip the template types by converting to a base StrokeT<> instance
  //       StrokeT<> baseNode;
  //       baseNode.key = item.key;
  //       baseNode.func = item.func;
  //       baseNode.children = item.children;

  //       // 2. Insert the non-templated node cleanly into the map
  //       children.insert({ item.key, std::move(baseNode) });
  //   }
  // }

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
  // std::function<void()> func = nullptr;
  StrokeAction func = nullptr;
  std::unordered_map<olc::Key, StrokeT<>> children; 
};

};