#pragma once
#include "Statement.h"


namespace Interpreter {

class StatementAddress : public Statement {
public:
  StatementAddress(Token t, std::shared_ptr<Expression> e):
    label(t), expr(e) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtAddress(this);
  }

public:
  Token label;
  std::shared_ptr<Expression> expr;
};

};
