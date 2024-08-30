#pragma once
#include "If.h"

namespace Zazy {

namespace Stmt {

class Return : public Statement {
public:
  Return(expr_t expr):
    expr(expr) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitStmtReturn(this);
  }

  void print() { printf("return "); if (expr != nullptr) expr->print(); }

public:
  expr_t expr;
};

};
};

