#pragma once
#include "StatementOneArgCommand.h"


namespace Interpreter {

class StatementVariable : public Statement {
public:
  enum TypeT { ADDRESS, DEFINITION };

  StatementVariable(Token t, std::shared_ptr<Expression> a):
    label(t), definition(a), type(DEFINITION) {}

  StatementVariable(Token t): label(t), type(ADDRESS) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtVariable(this);
  }

public:
  Token label;
  std::shared_ptr<Expression> definition;
  const TypeT type;
};

};
