#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementNoArgCommand : public Expression {
public:
  StatementNoArgCommand(uint32_t code, Token token): opcode(code), token(token) {}

  Bytes         accept(Visitor<Bytes>* visitor)         override { return visitor->visitStmtNoArgCommand(this); }
  uint32_t      accept(Visitor<uint32_t>* visitor)      override { return visitor->visitStmtNoArgCommand(this); }
  SemanticToken accept(Visitor<SemanticToken>* visitor) override { return visitor->visitStmtNoArgCommand(this); }

public:
  const uint32_t opcode;
  Token token;
};

};
