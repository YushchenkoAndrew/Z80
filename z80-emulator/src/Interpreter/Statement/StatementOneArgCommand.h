#pragma once
#include "StatementNoArgCommand.h"

namespace Interpreter {

class StatementOneArgCommand : public Statement {
public:
  StatementOneArgCommand(uint16_t o, std::shared_ptr<Token> t, Expression a):
    opcode(o), type(t->token), arg0(a) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  const uint16_t opcode;
  const TokenT type;
  const Expression arg0;
};

};
