#pragma once
#include "src/Utils/Utils.h"

namespace Zazy {

struct Type;

struct Member {
  std::string name;
  Type* type;
  uint32_t offset;
};

struct StructDef {
  std::string tag;
  std::vector<Member> members;
  uint32_t size = 0;
  bool complete = false;

  const Member* find(const std::string& name) const {
    for (auto& m : members) if (m.name == name) return &m;
    return nullptr;
  }
};

/**
 * Only the types the Z80 is happy with: void, 8/16 bit integers, pointers, arrays, structs.
 * Structs are packed (no padding), pointers are 16 bit.
 */
struct Type {
  enum Kind { VOID, U8, I8, U16, I16, PTR, ARRAY, STRUCT, FUNC };

  Kind kind = VOID;
  Type* base = nullptr;          // PTR: pointee, ARRAY: element, FUNC: return type
  uint32_t count = 0;            // ARRAY: length
  StructDef* sdef = nullptr;     // STRUCT
  bool isConst = false;          // const-qualified (for PTR: pointee is const, e.g. const uint8_t*)
  std::vector<Type*> params;     // FUNC

  bool isVoid()    const { return kind == VOID; }
  bool isInt()     const { return kind == U8 || kind == I8 || kind == U16 || kind == I16; }
  bool isPtr()     const { return kind == PTR; }
  bool isArray()   const { return kind == ARRAY; }
  bool isStruct()  const { return kind == STRUCT; }
  bool isFunc()    const { return kind == FUNC; }
  bool isScalar()  const { return isInt() || isPtr(); }
  bool isSigned()  const { return kind == I8 || kind == I16; }

  uint32_t size() const {
    switch (kind) {
      case U8: case I8: return 1;
      case U16: case I16: case PTR: return 2;
      case ARRAY: return count * base->size();
      case STRUCT: return sdef->size;
      default: return 0;
    }
  }

  // width of a scalar value in bytes (pointers are words)
  uint32_t width() const { return size() == 1 ? 1 : 2; }
};

namespace Types {

inline std::deque<Type>& pool() { static std::deque<Type> p; return p; }
inline std::deque<StructDef>& structs() { static std::deque<StructDef> p; return p; }

inline Type* make(Type::Kind k) { pool().push_back(Type()); pool().back().kind = k; return &pool().back(); }
inline Type* prim(Type::Kind k) {
  static Type* cache[16] = {};
  if (!cache[k]) cache[k] = make(k);
  return cache[k];
}

inline Type* Void() { return prim(Type::VOID); }
inline Type* U8()   { return prim(Type::U8); }
inline Type* I8()   { return prim(Type::I8); }
inline Type* U16()  { return prim(Type::U16); }
inline Type* I16()  { return prim(Type::I16); }

inline Type* ptr(Type* to, bool constPointee = false) {
  Type* t = make(Type::PTR); t->base = to; t->isConst = constPointee; return t;
}
inline Type* array(Type* of, uint32_t n) {
  Type* t = make(Type::ARRAY); t->base = of; t->count = n; return t;
}
inline Type* func(Type* ret, std::vector<Type*> params) {
  Type* t = make(Type::FUNC); t->base = ret; t->params = std::move(params); return t;
}
inline Type* structType(StructDef* d) {
  Type* t = make(Type::STRUCT); t->sdef = d; return t;
}
inline StructDef* newStruct(const std::string& tag) {
  structs().push_back(StructDef()); structs().back().tag = tag; return &structs().back();
}

// const-qualified copy (the pooled primitives are shared, never modify them)
inline Type* constOf(Type* t) {
  if (t->isConst) return t;
  Type* c = make(t->kind); *c = *t; c->isConst = true; return c;
}

// is the object itself read only (a const pointer *to* data is not)
inline bool readOnly(const Type* t) {
  if (t->kind == Type::PTR) return false;
  if (t->kind == Type::ARRAY) return readOnly(t->base);
  return t->isConst;
}

inline Type* intOf(uint32_t bytes, bool isSigned) {
  if (bytes == 1) return isSigned ? I8() : U8();
  return isSigned ? I16() : U16();
}

inline bool same(const Type* a, const Type* b) {
  if (a == b) return true;
  if (!a || !b || a->kind != b->kind) return false;

  switch (a->kind) {
    case Type::PTR:    return same(a->base, b->base);
    case Type::ARRAY:  return a->count == b->count && same(a->base, b->base);
    case Type::STRUCT: return a->sdef == b->sdef;
    case Type::FUNC: {
      if (!same(a->base, b->base) || a->params.size() != b->params.size()) return false;
      for (size_t i = 0; i < a->params.size(); i++) if (!same(a->params[i], b->params[i])) return false;
      return true;
    }
    default: return true;
  }
}

inline std::string str(const Type* t) {
  if (!t) return "?";

  switch (t->kind) {
    case Type::VOID: return "void";
    case Type::U8:   return "uint8_t";
    case Type::I8:   return "int8_t";
    case Type::U16:  return "uint16_t";
    case Type::I16:  return "int16_t";
    case Type::PTR:  return std::string(t->isConst ? "const " : "") + str(t->base) + "*";
    case Type::ARRAY: return str(t->base) + "[" + (t->count ? std::to_string(t->count) : "") + "]";
    case Type::STRUCT: return "struct " + t->sdef->tag;
    case Type::FUNC: {
      std::string s = str(t->base) + "(";
      for (size_t i = 0; i < t->params.size(); i++) s += (i ? ", " : "") + str(t->params[i]);
      return s + ")";
    }
  }

  return "?";
}

};
};
