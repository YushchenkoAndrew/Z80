#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {


class StatementRuntimeCommand : public Statement {
public:
  StatementRuntimeCommand(
    Token token,
    Expression value,
    std::function<uint32_t(uint32_t arg)> calc
  ): token(token), value(value), calc(calc) {}

  // StatementLambda(
  //   Token t,
  //   std::vector<std::shared_ptr<Expression>> expr,
  //   std::function<uint32_t(std::vector<uint32_t> argv)> lambda,
  //   std::vector<uint32_t> argv = {}
  // ): token(t), expr(expr), lambda(lambda), argv(argv) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtLambda(this);
  }

public:
  // std::vector<uint32_t> argv;
  Token token;
  Expression value;
  // std::vector<Expression> expr;
  const std::function<uint32_t(uint32_t arg)> calc;
};

};
