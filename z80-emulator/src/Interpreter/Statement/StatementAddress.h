#pragma once
#include "Statement.h"


namespace Interpreter {

class StatementAddress : public Statement {
public:
  StatementAddress(std::shared_ptr<Token> t, Expression e):
    label(t), address(e) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_ADDRESS>(), this);
  }

public:
  Expression address;
  std::shared_ptr<Token> label;
};

};
