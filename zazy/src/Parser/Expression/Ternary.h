#pragma once
#include "Suffix.h"

namespace Zazy {
namespace Expr {

class Ternary : public Expression {
public:
  Ternary(expr_t condition, expr_t left, expr_t right):
    condition(condition), left(left), right(right) {}
  
  
  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprTernary(this);
  }

  void print() { ENCLOSE(condition->print(); printf(" ? "); left->print(); printf(" : "); right->print();); }

public:
  expr_t condition;
  expr_t left;
  expr_t right;
};
};
};

