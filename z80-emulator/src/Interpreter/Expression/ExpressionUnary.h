#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(Token op, Expression right):
    operation(op), right(right) {}
  
  
  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprUnary(this);
  }

public:
  Expression right;
  Token operation;
};

};

