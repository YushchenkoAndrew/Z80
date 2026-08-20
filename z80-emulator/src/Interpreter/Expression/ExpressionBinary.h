#pragma once
#include "Expression.h"

namespace Interpreter {

class ExpressionBinary : public Expression {
public:
  ExpressionBinary(std::shared_ptr<Expression> left, Token op, std::shared_ptr<Expression> right):
    left(left), operation(op), right(right) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprBinary(this);
  }

public:
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
  Token operation;
};

};
