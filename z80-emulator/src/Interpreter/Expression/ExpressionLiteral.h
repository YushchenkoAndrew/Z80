#pragma once
#include "Expression.h"

namespace Interpreter {

class ExpressionLiteral : public Expression {
public:
  ExpressionLiteral(Token token): value(token.literal), type(token.token) {}

  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitLiteral(this);
  }

public:
  const std::string value;
  const TokenT type;
};

};

