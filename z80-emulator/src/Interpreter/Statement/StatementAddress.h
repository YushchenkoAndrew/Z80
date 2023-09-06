#pragma once
#include "Statement.h"


namespace Interpreter {

class StatementAddress : public Statement {
public:
  StatementAddress(std::shared_ptr<Token> t, Expression e):
    label(t), expr(e) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_ADDRESS>(), this);
  }

public:
  Expression expr;
  std::shared_ptr<Token> label;
};

};
