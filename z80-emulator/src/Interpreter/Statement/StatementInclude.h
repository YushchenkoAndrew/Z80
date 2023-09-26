#pragma once
#include "StatementAllocate.h"


namespace Interpreter {

class StatementInclude : public Statement {
public:

  StatementInclude(std::shared_ptr<ExpressionLiteral> a): expr(a) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtInclude(this);
  }

public:
  std::shared_ptr<ExpressionLiteral> expr;
};

};
