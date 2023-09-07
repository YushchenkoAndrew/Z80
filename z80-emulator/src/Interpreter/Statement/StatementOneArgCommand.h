#pragma once
#include "StatementNoArgCommand.h"

namespace Interpreter {

class StatementOneArgCommand : public Statement {
public:
  StatementOneArgCommand(uint32_t o, std::shared_ptr<Token> t, std::shared_ptr<Expression> a):
    opcode(o), expr(a), token(t) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtOneArg(this);
  }

public:
  const uint32_t opcode;
  std::shared_ptr<Token> token;
  std::shared_ptr<Expression> expr;
};

};
