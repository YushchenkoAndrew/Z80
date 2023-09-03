#pragma once
#include "StatementAddress.h"


namespace Interpreter {

class StatementAllocate : public Statement {
public:
  StatementAllocate(std::shared_ptr<Token> t, std::vector<Expression> d):
    token(t), data(d) {}

  template<class T>
  T accept(StatementVisitor<T>* visitor) {
    // TODO
  }

public:
  std::vector<Expression> data;
  std::shared_ptr<Token> token;
};

};
