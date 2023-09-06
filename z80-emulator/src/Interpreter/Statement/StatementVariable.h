#pragma once
#include "StatementOneArgCommand.h"


namespace Interpreter {

class StatementVariable : public Statement {
public:
  enum TypeT { ADDRESS, DEFINITION };

  StatementVariable(std::shared_ptr<Token> t, std::shared_ptr<Expression> a):
    label(t), definition(a), type(DEFINITION) {}

  StatementVariable(std::shared_ptr<Token> t): label(t), type(ADDRESS) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtVariable(this);
  }

public:
  std::shared_ptr<Expression> definition;
  std::shared_ptr<Token> label;
  const TypeT type;
};

};
