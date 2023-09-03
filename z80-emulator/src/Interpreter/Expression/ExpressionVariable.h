#pragma once
#include "ExpressionBinary.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(std::shared_ptr<Token> t):
    token(t) {}
  
  
  template<class T>
  T accept(ExpressionVisitor<T>* visitor) {
    // TODO:
    // return visitor.visitUnary(this);
  }

public:
  std::shared_ptr<Token> token;
};

};

