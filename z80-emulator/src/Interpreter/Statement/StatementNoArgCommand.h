#pragma once

#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementNoArgCommand : public Statement {
public:
  StatementNoArgCommand(uint32_t code, Token token): opcode(code), token(token) {}

  inline SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtNoArgCommand(this);
  }

public:
  SemanticToken semantic;
  SemanticToken& GetSemantic() override { return semantic; }

public:
  const uint32_t opcode;
  Token token;
};

};
