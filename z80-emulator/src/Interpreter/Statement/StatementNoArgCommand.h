#pragma once

#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementNoArgCommand : public Statement {
public:
  StatementNoArgCommand(uint32_t code, Token token): opcode(code), token(token) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtNoArg(this);
  }

public:
  const uint32_t opcode;
  Token token;
};

};
