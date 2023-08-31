#pragma once
#include "Statement.h"

namespace Interpreter {

class StatementLambda : public Statement {
public:
  StatementLambda(Expression expr, std::function<uint16_t(uint16_t)> opcode):
    expr(expr), opcode(opcode) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  const Expression expr;
  const std::function<uint16_t(uint16_t)> opcode;
};

};
