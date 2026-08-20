#pragma once
#include "ExpressionLiteral.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(Token op, std::shared_ptr<Expression> right):
    operation(op), right(right) {}
  
  
  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprUnary(this);
  }

public:
  std::shared_ptr<Expression> right;
  Token operation;
};

};

