#pragma once
#include "Block.h"

namespace Zazy {

namespace Stmt {

class Expr : public Statement {
public:
  Expr(expr_t expr):
    expr(expr) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitStmtExpr(this);
  }

  void print() { expr->print(); }

public:
  expr_t expr;
};

};
};

