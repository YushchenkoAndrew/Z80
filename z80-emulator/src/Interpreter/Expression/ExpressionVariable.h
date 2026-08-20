#pragma once
#include "ExpressionUnary.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(Token t, Token l, int32_t s):
    token(t), length(l), size(s) {}
  
  
  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprVariable(this);
  }

public:
  Token token;
  Token length;
  const int32_t size;
};

};

