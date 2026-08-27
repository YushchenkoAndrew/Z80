#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementAllocate : public Expression {
public:
  StatementAllocate(Token token, std::list<std::unique_ptr<Expression>> data, uint8_t size):
    token(token), data(std::move(data)), size(size) {}

  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtAllocate(this);
  }

public:
  uint8_t size;
  Token token;
  std::list<std::unique_ptr<Expression>> data;
};

};
