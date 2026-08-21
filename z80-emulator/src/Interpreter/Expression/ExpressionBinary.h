#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionBinary : public Expression {
public:
  ExpressionBinary(Expression left, Token op, Expression right):
    left(left), operation(op), right(right) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprBinary(this);
  }

public:
  Expression left;
  Expression right;
  Token operation;
};

};
