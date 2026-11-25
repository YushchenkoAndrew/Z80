#pragma once
#include "src/Parser/AST.h"

namespace Zazy {

struct FuncInfo;

struct Symbol {
  enum Kind { GLOBAL, LOCAL, PARAM, FUNC };

  Kind kind = GLOBAL;
  std::string name;
  Type* type = nullptr;
  std::string label;            // asm name of globals, statics and functions
  int off = 0;                  // LOCAL / PARAM: address is IX+off
  bool isConst = false;         // read only object (ROM when initialised)
  bool hasAddr = false;         // __at(addr)
  uint32_t addr = 0;
  bool isExtern = false;
  bool used = false;
  Decl::Var* var = nullptr;
  FuncInfo* fn = nullptr;
};

struct FuncInfo {
  std::string name;
  std::string label;            // SETUP / MAIN / @name / bound routine
  Decl::Func* decl = nullptr;
  Type* type = nullptr;
  Type* ret = nullptr;
  std::vector<Symbol*> params;
  int frame = 0;                // bytes addressed through IX
  bool defined = false;
  bool used = false;
  bool isStatic = false;
  bool isNaked = false;
  bool isInterrupt = false;
  bool hasCalls = false;
  Symbol* sym = nullptr;
  std::vector<std::string> labels;      // goto labels
};

};
