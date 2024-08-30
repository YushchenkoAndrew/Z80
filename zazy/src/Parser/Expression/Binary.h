#pragma once
#include "Assign.h"

namespace Zazy {

namespace Expr {

class Binary : public Expression {
public:
  Binary(expr_t left, token_t op, expr_t right):
    left(left), operation(op), right(right) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprBinary(this);
  }

  void print() { ENCLOSE(left->print(); printf(" %s ", operation->lexeme.c_str()); right->print();) }

public:
  expr_t left;
  expr_t right;
  token_t operation;
};

};
};
