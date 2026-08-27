#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(Token t, Token l, uint8_t bytes):
    token(t), length(l), bytes(bytes) {}
  
  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitExprVariable(this);
  }

public:
  Token token;
  Token length;
  uint8_t bytes;
};

};

