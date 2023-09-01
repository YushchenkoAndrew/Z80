#pragma once
#include "StatementLambda.h"

namespace Interpreter {

class StatementNoArgCommand : public Statement {
public:
  StatementNoArgCommand(uint32_t o, std::shared_ptr<Token> t): opcode(o), command(t) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  const uint32_t opcode;
  std::shared_ptr<Token> command;
};

};
