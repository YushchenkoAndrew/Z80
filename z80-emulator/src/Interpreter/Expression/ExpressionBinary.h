#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionBinary : public Expression {
public:
  ExpressionBinary(Expression left, Token op, Expression right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  inline SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitExprBinary(this);
  }

public:
  SemanticToken semantic;
  SemanticToken& GetSemantic() override { return semantic; }

public:
  Expression left;
  Expression right;
  Token op;
};

};
