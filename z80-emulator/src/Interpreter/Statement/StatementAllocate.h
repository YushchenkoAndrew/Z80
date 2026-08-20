#pragma once
#include "StatementAddress.h"


namespace Interpreter {

class StatementAllocate : public Statement {
public:
  StatementAllocate(Token t, std::vector<std::shared_ptr<Expression>> d, int32_t s, bool r = false):
    token(t), data(d), size(s), reverse(r) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtAllocate(this);
  }

public:
  int32_t size;
  bool reverse;
  Token token;
  std::vector<std::shared_ptr<Expression>> data;
};

};
