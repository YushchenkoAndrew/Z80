#pragma once
#include "StatementAddress.h"


namespace Interpreter {

class StatementAllocate : public Statement {
public:
  StatementAllocate(std::vector<Expression> d): data(d) {}

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_ALLOCATE>(), this);
  }

public:
  std::vector<Expression> data;
};

};
