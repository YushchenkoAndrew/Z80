#pragma once
#include "StatementNoArgCommand.h"

namespace Interpreter {

class StatementOneArgCommand : public Statement {
public:
  StatementOneArgCommand(uint32_t o, std::shared_ptr<Token> t, Expression a):
    opcode(o), command(t), arg(a) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  const uint32_t opcode;

  Expression arg;
  std::shared_ptr<Token> command;
};

};
