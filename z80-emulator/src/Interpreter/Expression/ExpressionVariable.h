#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class ExpressionVariable : public Expression {
public:
  ExpressionVariable(Token t, Token l, uint32_t bytes):
    label(t), length(l), bytes(bytes) {}
  
  Bytes         accept(Visitor<Bytes>*         visitor) override { return visitor->visitExprVariable(this); }
  uint32_t      accept(Visitor<uint32_t>*      visitor) override { return visitor->visitExprVariable(this); }
  SemanticToken accept(Visitor<SemanticToken>* visitor) override { return visitor->visitExprVariable(this); }

public:
  Token label;
  Token length;
  uint32_t bytes;
};

};

