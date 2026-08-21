#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementVariable : public Statement {
public:
  enum TypeT { ADDRESS, DEFINITION };

  StatementVariable(Token token, Expression expr):
    label(token), definition(expr), type(DEFINITION) {}

  StatementVariable(Token t): label(t), type(ADDRESS) {}

  inline MemoryT accept(Visitor* visitor) override {
    return visitor->visitStmtVariable(this);
  }

public:
  Token label;
  Expression definition;
  const TypeT type;
};

};
