#pragma once
#include "ExpressionLiteral.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(std::shared_ptr<Token> op, Expression right):
    operation(op->token), right(right) {}
  
  
  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitUnary(this);
  }

public:
  const Expression right;
  const TokenT operation;
};

};

