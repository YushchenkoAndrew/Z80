#pragma once
#include "ExpressionUnary.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(std::shared_ptr<Token> t, std::shared_ptr<Token> l, int32_t s):
    token(t), length(l), size(s) {}
  
  
  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprVariable(this);
  }

public:
  std::shared_ptr<Token> token;
  std::shared_ptr<Token> length;
  const int32_t size;
};

};

