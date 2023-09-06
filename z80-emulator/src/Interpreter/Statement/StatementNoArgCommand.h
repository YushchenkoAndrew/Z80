#pragma once
#include "StatementLambda.h"

namespace Interpreter {

class StatementNoArgCommand : public Statement {
public:
  StatementNoArgCommand(uint32_t o): opcode(o) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtNoArg(this);
  }

public:
  const uint32_t opcode;
};

};
