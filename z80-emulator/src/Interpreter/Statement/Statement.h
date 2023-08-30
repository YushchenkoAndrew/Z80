#pragma once
#include "src/Interpreter/Expression/ExpressionVisitor.h"

namespace Interpreter {

template<class T>
class StatementVisitor;

class Statement {

  template<class T>
  T accept(StatementVisitor<T>* visitor);
};

};
