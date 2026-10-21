#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionUnary : public Expression {
public:
  ExpressionUnary(Token op, std::unique_ptr<Expression> right):
    op(op), right(std::move(right)) {}
  
  Bytes         accept(Visitor<Bytes>*         visitor) override { return visitor->visitExprUnary(this); }
  uint32_t      accept(Visitor<uint32_t>*      visitor) override { return visitor->visitExprUnary(this); }
  SemanticToken accept(Visitor<SemanticToken>* visitor) override { return visitor->visitExprUnary(this); }

public:
  Token op;
  std::unique_ptr<Expression> right;
};

};

