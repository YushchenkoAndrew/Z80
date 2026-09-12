#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionLiteral : public Expression {
public:
  ExpressionLiteral(Token& t, uint32_t bytes):
    token(std::move(t)), bytes(bytes) {}

  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitExprLiteral(this);
  }

  Bytes accept(Visitor<Bytes>* visitor) override {
    return visitor->visitExprLiteral(this);
  }

public:
  Token token;
  uint32_t bytes;
};

};

