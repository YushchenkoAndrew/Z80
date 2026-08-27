#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionBinary : public Expression {
public:
  ExpressionBinary(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right):
    left(std::move(left)), op(op), right(std::move(right)) {}

  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitExprBinary(this);
  }

public:
  std::unique_ptr<Expression> left;
  std::unique_ptr<Expression> right;
  Token op;
};

};
