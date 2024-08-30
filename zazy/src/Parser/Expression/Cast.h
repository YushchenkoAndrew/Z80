#pragma once
#include "Binary.h"

namespace Zazy {

namespace Expr {

class Cast : public Expression {
public:
  Cast(token_t type, expr_t expr):
    type(type), right(expr) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprCast(this);
  }

  void print() { ENCLOSE(printf("(%s)", type->lexeme.c_str()); right->print();) }

public:
  token_t type;
  expr_t right;
};

};
};
