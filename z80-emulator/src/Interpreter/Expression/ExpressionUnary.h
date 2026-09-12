#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(Token op, std::unique_ptr<Expression> right):
    op(op), right(std::move(right)) {}
  
  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitExprUnary(this);
  }

  Bytes accept(Visitor<Bytes>* visitor) override {
    return visitor->visitExprUnary(this);
  }

public:
  Token op;
  std::unique_ptr<Expression> right;
};

};

