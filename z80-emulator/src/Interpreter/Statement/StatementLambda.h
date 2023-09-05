#pragma once
#include "StatementAllocate.h"

namespace Interpreter {

class StatementLambda : public Statement {
public:
  StatementLambda(
    Expression expr,
    std::function<uint32_t(std::vector<uint32_t>& argv)> lambda,
    std::vector<uint32_t> argv = {}
  ): expr({ expr }), lambda(lambda), argv(argv) {}

  StatementLambda(
    std::vector<Expression> expr,
    std::function<uint32_t(std::vector<uint32_t>& argv)> lambda,
    std::vector<uint32_t> argv = {}
  ): expr(expr), lambda(lambda), argv(argv) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_LAMBDA>(), this);
  }

public:
  std::vector<Expression> expr;
  std::vector<uint32_t> argv;
  const std::function<uint32_t(std::vector<uint32_t>& argv)> lambda;
};

};
