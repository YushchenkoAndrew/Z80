#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementNoArgCommand : public Expression {
public:
  StatementNoArgCommand(uint32_t code, Token token): opcode(code), token(token) {}

  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtNoArgCommand(this);
  }

public:
  const uint32_t opcode;
  Token token;
};

};
