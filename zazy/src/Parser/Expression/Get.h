#pragma once
#include "Cast.h"

namespace Zazy {

namespace Expr {

class Get : public Expression {
public:
  Get(expr_t left, token_t op, expr_t right):
    left(left), operation(op), right(right) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprGet(this);
  }

  void print() { ENCLOSE(left->print(); printf("."); right->print();) }

public:
  expr_t left;
  expr_t right;
  token_t operation;
};

};
};
