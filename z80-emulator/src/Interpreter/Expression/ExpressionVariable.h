#pragma once
#include "ExpressionUnary.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(std::shared_ptr<Token> t):
    token(t) {}
  
  
  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<EXPR_VARIABLE>(), this);
  }

public:
  std::shared_ptr<Token> token;
};

};

