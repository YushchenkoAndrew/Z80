#pragma once
#include "Func.h"

namespace Zazy {

namespace Decl {

class Var : public Statement {
public:
  Var(token_t type, token_t name, expr_t value):
    type(type), name(name), value(value) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitDeclVar(this);
  }

  void print() { 
    printf("%s %s", type->lexeme.c_str(), name->lexeme.c_str());
    if (value != nullptr) { printf(" = "); value->print(); }
  }

public:
  token_t type;
  token_t name;
  expr_t value;
};

};
};

