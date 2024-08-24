#pragma once
#include "Array.h"

namespace Zazy {

namespace Expr {

class Assign : public Expression {
public:
  Assign(expr_t left, expr_t right):
    left(left), right(right) {}

  void* accept(Visitor* visitor) override {
    return visitor->visitExprAssign(this);
  }

  void print() { ENCLOSE(left->print(); printf(" = "); right->print();) }

public:
  expr_t left;
  expr_t right;
};

};
};
