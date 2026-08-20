#pragma once
#include "ExpressionBinary.h"

namespace Interpreter {

class ExpressionLiteral : public Expression {
public:
  ExpressionLiteral(Token t, int32_t s): 
    token(t), size(s) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprLiteral(this);
  }

public:
  Token token;
  const int32_t size;
};

};

