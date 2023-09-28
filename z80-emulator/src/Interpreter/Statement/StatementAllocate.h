#pragma once
#include "StatementAddress.h"


namespace Interpreter {

class StatementAllocate : public Statement {
public:
  StatementAllocate(std::shared_ptr<Token> t, std::vector<std::shared_ptr<Expression>> d, int32_t s):
    token(t), data(d), size(s) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtAllocate(this);
  }

public:
  int32_t size;
  std::shared_ptr<Token> token;
  std::vector<std::shared_ptr<Expression>> data;
};

};
