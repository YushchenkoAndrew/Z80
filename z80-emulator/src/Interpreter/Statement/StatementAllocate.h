#pragma once
#include "StatementAddress.h"


namespace Interpreter {

class StatementAllocate : public Statement {
public:
  StatementAllocate(std::vector<std::shared_ptr<Expression>> d): data(d) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtAllocate(this);
  }

public:
  std::vector<std::shared_ptr<Expression>> data;
};

};
