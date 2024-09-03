#pragma once
#include "Suffix.h"

namespace Zazy {
namespace Expr {

class Ternary : public Expression {
public:
  Ternary(expr_t condition, expr_t then, expr_t otherwise):
    condition(condition), then(then), otherwise(otherwise) {}
  
  
  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprTernary(this);
  }

  void print() { ENCLOSE(condition->print(); printf(" ? "); then->print(); printf(" : "); otherwise->print();); }

public:
  expr_t condition;
  expr_t then;
  expr_t otherwise;
};
};
};

