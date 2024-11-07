#pragma once
#include "Return.h"

namespace Zazy {

namespace Stmt {

class While : public Statement {
public:
  While(expr_t condition, stmt_t body):
    condition(condition), body(body) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitStmtWhile(this);
  }

  void print() { 
    printf("while ("); condition->print();
    if (body != nullptr) body->print(); else printf(";");
  }

public:
  expr_t condition;
  stmt_t body;
};

};
};

