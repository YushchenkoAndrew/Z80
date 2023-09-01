#pragma once
#include "ExpressionLiteral.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(std::shared_ptr<Token> op, Expression right):
    operation(op), right(right) {}
  
  
  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitUnary(this);
  }

public:
  Expression right;
  std::shared_ptr<Token> operation;
};

};

