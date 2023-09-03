#pragma once
#include "Statement.h"


namespace Interpreter {

class StatementAddress : public Statement {
public:
  StatementAddress(std::shared_ptr<Token> t, Expression e):
    label(t), address(e) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  Expression address;
  std::shared_ptr<Token> label;
};

};
