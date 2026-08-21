#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionLiteral : public Expression {
public:
  ExpressionLiteral(Token t): token(t) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprLiteral(this);
  }

public:
  Token token;
};

};

