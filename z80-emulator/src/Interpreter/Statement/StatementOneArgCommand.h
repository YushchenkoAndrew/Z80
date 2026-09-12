#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementOneArgCommand : public Expression {
public:
  StatementOneArgCommand(uint32_t code, Token token, std::unique_ptr<Expression> expr):
    opcode(code), expr(std::move(expr)), token(token) {}

  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtOneArgCommand(this);
  }

  Bytes accept(Visitor<Bytes>* visitor) override {
    return visitor->visitStmtOneArgCommand(this);
  }

public:
  const uint32_t opcode;
  Token token;
  std::unique_ptr<Expression> expr;
};

};
