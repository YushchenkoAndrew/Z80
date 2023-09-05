#pragma once
#include "StatementLambda.h"

namespace Interpreter {

class StatementNoArgCommand : public Statement {
public:
  StatementNoArgCommand(uint32_t o): opcode(o) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_NO_ARG>(), this);
  }

public:
  const uint32_t opcode;
};

};
