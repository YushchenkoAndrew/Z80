#pragma once
#include "ExpressionUnary.h"

namespace Interpreter {

class ExpressionBinary : public Expression {
public:
  ExpressionBinary(Expression left, TokenT op, Expression righ):
    left(left), operation(op), right(righ) {}

  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitBinary(this);
  }

public:
  const Expression left;
  const Expression right;
  const TokenT operation;
};

};
