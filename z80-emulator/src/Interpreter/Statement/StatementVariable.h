#pragma once
#include "StatementOneArgCommand.h"


namespace Interpreter {

class StatementVariable : public Statement {
public:
  StatementVariable(std::shared_ptr<Token> t, Expression a):
    label(t), definition(a) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  Expression definition;
  std::shared_ptr<Token> label;
};

};
