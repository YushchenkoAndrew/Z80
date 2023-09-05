#pragma once
#include "ExpressionLiteral.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(std::shared_ptr<Token> op, Expression right):
    operation(op), right(right) {}
  
  
  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<EXPR_UNARY>(), this);
  }

public:
  Expression right;
  std::shared_ptr<Token> operation;
};

};

