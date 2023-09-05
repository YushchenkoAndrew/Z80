#pragma once
#include "ExpressionBinary.h"

namespace Interpreter {

class ExpressionLiteral : public Expression {
public:
  ExpressionLiteral(std::shared_ptr<Token> t, int32_t s): 
    token(t), size(s) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<EXPR_LITERAL>(), this);
  }

public:
  std::shared_ptr<Token> token;
  const int32_t size;
};

};

