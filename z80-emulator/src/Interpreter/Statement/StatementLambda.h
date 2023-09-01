#pragma once
#include "Statement.h"

namespace Interpreter {

class StatementLambda : public Statement {
public:
  StatementLambda(
    Expression expr,
    std::shared_ptr<Token> t,
    std::function<uint32_t(std::vector<uint32_t>& argv)> opcode,
    std::vector<uint32_t> argv = {}
  ): token(t), expr({ expr }), opcode(opcode), argv(argv) {}

  StatementLambda(
    std::vector<Expression> expr,
    std::shared_ptr<Token> t,
    std::function<uint32_t(std::vector<uint32_t>& argv)> opcode,
    std::vector<uint32_t> argv = {}
  ): token(t), expr(expr), opcode(opcode), argv(argv) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  std::vector<Expression> expr;
  std::vector<uint32_t> argv;
  std::shared_ptr<Token> token;
  const std::function<uint32_t(std::vector<uint32_t>& argv)> opcode;
};

};
