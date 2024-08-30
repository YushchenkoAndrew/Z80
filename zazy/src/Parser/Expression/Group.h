#pragma once
#include "Get.h"

namespace Zazy {
namespace Expr {

class Group : public Expression {
public:
  Group(expr_t expr):
    expr(expr) {}
  
  
  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprGroup(this);
  }

  void print() { ENCLOSE(expr->print();) }

public:
  expr_t expr;
};
};
};

