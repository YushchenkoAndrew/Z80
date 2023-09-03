#pragma once
#include "StatementOneArgCommand.h"


namespace Interpreter {

class StatementVariable : public Statement {
  enum TypeT { ADDRESS, DEFINITION };

public:
  StatementVariable(std::shared_ptr<Token> t, Expression a):
    label(t), definition(a), type(DEFINITION) {}

  StatementVariable(std::shared_ptr<Token> t): label(t), type(ADDRESS) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  Expression definition;
  std::shared_ptr<Token> label;
  const TypeT type;
};

};
