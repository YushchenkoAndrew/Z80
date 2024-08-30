#pragma once
#include "For.h"

namespace Zazy {

namespace Stmt {

class If : public Statement {
public:
  If(expr_t condition, stmt_t then, stmt_t otherwise):
    condition(condition), then(then), otherwise(otherwise) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitStmtIf(this);
  }

  void print() { 
    printf("if ("); condition->print(); printf(") "); then->print(); 
    if (otherwise != nullptr) { printf(" else "); otherwise->print(); }
  }

public:
  expr_t condition;
  stmt_t then;
  stmt_t otherwise;
};

};
};

