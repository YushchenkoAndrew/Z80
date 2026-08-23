#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(Token op, Expression right):
    op(op), right(right) {}
  
  inline SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitExprUnary(this);
  }

public:
  SemanticToken semantic;
  SemanticToken& GetSemantic() override { return semantic; }

public:
  Expression right;
  Token op;
};

};

