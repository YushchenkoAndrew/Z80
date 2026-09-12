#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"
#include <list>

namespace Interpreter {


class StatementRuntimeCommand : public Expression {
public:
  StatementRuntimeCommand(
    Token token,
    std::unique_ptr<Expression> arg,
    std::function<uint32_t(uint32_t arg1, uint32_t arg2)> calc
  ): token(token), calc(calc) { args.push_back(std::move(arg)); }

  StatementRuntimeCommand(
    Token token,
    std::unique_ptr<Expression> arg1,
    std::unique_ptr<Expression> arg2,
    std::function<uint32_t(uint32_t arg1, uint32_t arg2)> calc
  ): token(token), calc(calc) { args.push_back(std::move(arg1)); args.push_back(std::move(arg2)); }

  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtRuntimeCommand(this);
  }

  Bytes accept(Visitor<Bytes>* visitor) override {
    return visitor->visitStmtRuntimeCommand(this);
  }

public:
  Token token;
  std::list<std::unique_ptr<Expression>> args;
  const std::function<uint32_t(uint32_t arg1, uint32_t arg2)> calc;
};

};
