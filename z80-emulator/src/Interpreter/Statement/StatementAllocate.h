#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementAllocate : public Statement {
public:
  StatementAllocate(Token token, std::vector<Expression> data, int32_t size):
    token(token), data(data), size(size) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtAllocate(this);
  }

public:
  int32_t size;
  Token token;
  std::vector<Expression> data;
};

};
