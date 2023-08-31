#pragma once
#include "ExpressionBinary.h"

namespace Interpreter {

class ExpressionGroup : public Expression {
public:
  ExpressionGroup(Expression left, Expression right):
    left(left), right(right) {}
  
  
  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitGroup(this);
  }

public:
  const Expression left;
  const Expression right;
};

};

