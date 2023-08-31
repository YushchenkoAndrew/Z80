#pragma once
#include "Expression.h"

namespace Interpreter {

class ExpressionLiteral : public Expression {
public:
  ExpressionLiteral(std::shared_ptr<Token> token, int32_t s): 
    value(token->literal), type(token->token), size(s) {}

  ExpressionLiteral(std::string val, TokenT type): 
    value(val), type(type), size(0) {}

  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    return visitor.visitLiteral(this);
  }

public:
  const std::string value;
  const TokenT type;
  const int32_t size;
};

};

