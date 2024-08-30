#pragma once
#include "Expr.h"

namespace Zazy {

namespace Stmt {

class For : public Statement {
public:
  For(stmt_t init, expr_t condition, expr_t next, stmt_t body):
    init(init), condition(condition), next(next), body(body) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitStmtFor(this);
  }

  void print() { 
    printf("for ");

    printf("("); if (init != nullptr) init->print();
    printf(";"); if (condition != nullptr) condition->print();
    printf(";"); if (next != nullptr) next->print();
    printf(")"); if (body != nullptr) body->print(); else printf(";");
  }

public:
  stmt_t init;
  expr_t condition;
  expr_t next;
  stmt_t body;
};

};
};

