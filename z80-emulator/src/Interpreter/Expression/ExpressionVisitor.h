#pragma once
#include "ExpressionBinary.h"


namespace Interpreter {

template<class T>
class ExpressionVisitor {

public:
  T visitLiteral(ExpressionLiteral* expr);
  T visitBinary(ExpressionBinary* expr);
  T visitUnary(ExpressionUnary* expr);
  // T visitGroup(expr: ConditionGroupExpression);
  // T visitList(expr: ConditionListExpression);
  // T visitVariable(expr: ConditionVariableExpression);
};

};