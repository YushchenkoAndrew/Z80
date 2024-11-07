#pragma once
#include "src/Parser/Statement/While.h"

namespace Zazy {

namespace Decl {

class Enum : public Statement {
public:
  Enum(std::vector<stmt_t> vars):
    name(name), vars(vars) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitDeclEnum(this);
  }

  void print() { 
    printf("enum {"); 
    for (int32_t i = 0; i < vars.size(); i++) { if (i) printf(", "); vars[i]->print(); }
    printf(" }");
  }

public:
  token_t name;
  std::vector<stmt_t> vars;
};

};
};

