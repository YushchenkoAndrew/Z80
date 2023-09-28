#pragma once
#include "StatementInclude.h"

namespace Interpreter {

class StatementLambda : public Statement {
public:
  StatementLambda(
    std::shared_ptr<Token> t,
    std::shared_ptr<Expression> expr,
    std::function<uint32_t(std::vector<uint32_t> argv)> lambda,
    std::vector<uint32_t> argv = {}
  ): token(t), expr({ expr }), lambda(lambda), argv(argv) {}

  StatementLambda(
    std::shared_ptr<Token> t,
    std::vector<std::shared_ptr<Expression>> expr,
    std::function<uint32_t(std::vector<uint32_t> argv)> lambda,
    std::vector<uint32_t> argv = {}
  ): token(t), expr(expr), lambda(lambda), argv(argv) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtLambda(this);
  }

public:
  std::vector<uint32_t> argv;
  std::shared_ptr<Token> token;
  std::vector<std::shared_ptr<Expression>> expr;
  const std::function<uint32_t(std::vector<uint32_t> argv)> lambda;
};

};
