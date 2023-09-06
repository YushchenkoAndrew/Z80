#pragma once
#include "StatementNoArgCommand.h"

namespace Interpreter {

class StatementOneArgCommand : public Statement {
public:
  StatementOneArgCommand(uint32_t o, std::shared_ptr<Expression> a):
    opcode(o), expr(a) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtOneArg(this);
  }

public:
  std::shared_ptr<Expression> expr;
  const uint32_t opcode;
};

};
