#pragma once
#include "Unary.h"

namespace Zazy {
namespace Expr {

class Var : public Expression {
public:
  Var(token_t t): name(t) {}
  
  
  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprVar(this);
  }

  void print() { printf("%s", name->lexeme.c_str()); }

public:
  token_t name;
};
};
};

