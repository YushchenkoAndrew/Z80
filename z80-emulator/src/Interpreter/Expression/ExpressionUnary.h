#pragma once
#include "ExpressionLiteral.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(TokenT op, Expression right):
    operation(op), right(right) {}
  
  
  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitUnary(this);
  }

public:
  const Expression right;
  const TokenT operation;
};

};

