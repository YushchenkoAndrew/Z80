#pragma once
#include "src/Interpreter/Token.h"

namespace Interpreter {

template<class T>
class ExpressionVisitor;

class Expression {

  template<class T>
  T accept(ExpressionVisitor<T>* visitor);
};

};
