#pragma once
#include "Expression.h"

namespace Interpreter {

class ExpressionBinary : public Expression {
public:
  ExpressionBinary(Expression left, std::shared_ptr<Token> op, Expression righ):
    left(left), operation(op), right(righ) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<EXPR_BINARY>(), this);
  }

public:
  Expression left;
  Expression right;
  std::shared_ptr<Token> operation;
};

};
