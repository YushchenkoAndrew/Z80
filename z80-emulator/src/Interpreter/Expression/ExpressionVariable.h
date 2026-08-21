#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(Token t, Token l):
    token(t), length(l) {}
  
  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitExprVariable(this);
  }

public:
  Token token;
  Token length;
};

};

