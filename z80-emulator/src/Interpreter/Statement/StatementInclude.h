#pragma once

#include "src/Interpreter/Expression/ExpressionLiteral.h"
#include "src/Interpreter/Statement/Statement.h"

namespace Interpreter {

class StatementInclude : public Statement {
public:

  StatementInclude(ExpressionLiteral expr): expr(expr) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtInclude(this);
  }

public:
  ExpressionLiteral expr;
};

};
