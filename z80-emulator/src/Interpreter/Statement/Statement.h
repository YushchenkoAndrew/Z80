#pragma once
#include "src/Interpreter/Expression/ExpressionVariable.h"

namespace Interpreter {

class Statement : public Expression {

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<STMT_UNKNOWN>(), this);
  }
};

};
