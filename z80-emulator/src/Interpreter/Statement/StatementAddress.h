#pragma once
#include "Statement.h"


namespace Interpreter {

class StatementAddress : public Statement {
public:
  StatementAddress(std::shared_ptr<Token> t, std::shared_ptr<Expression> e):
    label(t), expr(e) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtAddress(this);
  }

public:
  std::shared_ptr<Expression> expr;
  std::shared_ptr<Token> label;
};

};
