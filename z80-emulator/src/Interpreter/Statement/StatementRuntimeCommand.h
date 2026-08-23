#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"
#include <list>

namespace Interpreter {


class StatementRuntimeCommand : public Statement {
public:
  StatementRuntimeCommand(
    Token token,
    Expression arg,
    std::function<uint32_t(uint32_t arg1, uint32_t arg2)> calc
  ): token(token), args({ arg }), calc(calc) {}

  StatementRuntimeCommand(
    Token token,
    Expression arg1,
    Expression arg2,
    std::function<uint32_t(uint32_t arg1, uint32_t arg2)> calc
  ): token(token), args({ arg1, arg2 }), calc(calc) {}

  inline SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtRuntimeCommand(this);
  }

public:
  SemanticToken semantic;
  SemanticToken& GetSemantic() override { return semantic; }

public:
  Token token;
  std::list<Expression> args;
  const std::function<uint32_t(uint32_t arg1, uint32_t arg2)> calc;
};

};
