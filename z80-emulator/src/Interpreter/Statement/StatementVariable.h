#pragma once
#include "StatementOneArgCommand.h"


namespace Interpreter {

class StatementVariable : public Statement {
public:
  enum TypeT { ADDRESS, DEFINITION };

  StatementVariable(std::shared_ptr<Token> t, Expression a):
    label(t), definition(a), type(DEFINITION) {}

  StatementVariable(std::shared_ptr<Token> t): label(t), type(ADDRESS) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_VARIABLE>(), this);
  }

public:
  Expression definition;
  std::shared_ptr<Token> label;
  const TypeT type;
};

};
