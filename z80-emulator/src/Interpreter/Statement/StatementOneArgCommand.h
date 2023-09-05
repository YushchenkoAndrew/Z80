#pragma once
#include "StatementNoArgCommand.h"

namespace Interpreter {

class StatementOneArgCommand : public Statement {
public:
  StatementOneArgCommand(uint32_t o, Expression a):
    opcode(o), expr(a) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_ONE_ARG>(), this);
  }

public:
  Expression expr;
  const uint32_t opcode;
};

};
