#pragma once
#include "src/Interpreter/Token.h"

namespace Interpreter {


template<class T>
class Visitor;

class Expression {

  template<class T>
  inline T accept(Visitor<T>* visitor) {
    return visitor.visit(Int2Type<EXPR_UNKNOWN>(), this);
  }
};


template<class T>
class Visitor {
public:

  template <int32_t U>
  T visit(Int2Type<U>, Expression* expr);
};
};
