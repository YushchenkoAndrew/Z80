#pragma once
#include "StatementLambda.h"

namespace Interpreter {

class StatementNoArgCommand : public Statement {
public:
  StatementNoArgCommand(uint16_t o, std::shared_ptr<Token> t): opcode(o), type(t->token) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  const uint16_t opcode;
  const TokenT type;
};

};
