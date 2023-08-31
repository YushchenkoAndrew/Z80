#pragma once
#include "ExpressionGroup.h"


namespace Interpreter {

template<class T>
class ExpressionVisitor {

public:
  T visitLiteral(ExpressionLiteral* expr);
  T visitBinary(ExpressionBinary* expr);
  T visitUnary(ExpressionUnary* expr);
  T visitGroup(ExpressionGroup* expr);
  // T visitList(expr: ConditionListExpression);
  // T visitVariable(expr: ConditionVariableExpression);
};

};