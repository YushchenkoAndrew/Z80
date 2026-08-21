#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementOneArgCommand : public Statement {
public:
  StatementOneArgCommand(uint32_t code, Token token, Expression expr):
    opcode(code), expr(expr), token(token) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtOneArg(this);
  }

public:
  const uint32_t opcode;
  Token token;
  Expression expr;
};

};
