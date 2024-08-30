#pragma once
#include "src/Parser/Expression/Var.h"

namespace Zazy {

namespace Stmt {

class Block : public Statement {
public:
  Block(std::vector<stmt_t> stmt):
    stmt(stmt) {}

  obj_t accept(Visitor* visitor) override {
    return visitor->visitStmtBlock(this);
  }

  void print() { printf("{"); for (auto& s : stmt) s->print(); printf("}"); }

public:
  std::vector<stmt_t> stmt;
};

};
};

