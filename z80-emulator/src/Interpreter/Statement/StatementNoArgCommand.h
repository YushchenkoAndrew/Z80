#pragma once
#include "Statement.h"

namespace Interpreter {

class StatementNoArgCommand : public Statement {
public:
  StatementNoArgCommand(uint16_t o, TokenT t): opcode(o), type(t) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  const uint16_t opcode;
  const TokenT type;
};

};
