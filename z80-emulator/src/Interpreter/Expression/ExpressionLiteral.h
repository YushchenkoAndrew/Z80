#pragma once
#include "Expression.h"

namespace Interpreter {

class ExpressionLiteral : public Expression {
public:
  ExpressionLiteral(std::shared_ptr<Token> t, int32_t s): 
    token(t), size(s) {}

  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitLiteral(this);
  }

public:
  std::shared_ptr<Token> token;
  const int32_t size;
};

};

