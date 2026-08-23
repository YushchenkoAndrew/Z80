#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementAllocate : public Statement {
public:
  StatementAllocate(Token token, std::list<Expression> data, uint8_t size):
    token(token), data(data), size(size) {}

  inline SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtAllocate(this);
  }

public:
  SemanticToken semantic;
  SemanticToken& GetSemantic() override { return semantic; }

public:
  uint8_t size;
  Token token;
  std::list<Expression> data;
};

};
