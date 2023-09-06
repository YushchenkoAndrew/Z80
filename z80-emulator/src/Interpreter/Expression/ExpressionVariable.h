#pragma once
#include "ExpressionUnary.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(std::shared_ptr<Token> t, int32_t s):
    token(t), size(s) {}
  
  
  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprVariable(this);
  }

public:
  std::shared_ptr<Token> token;
  const int32_t size;
};

};

