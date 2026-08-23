#include "Expression.h"

namespace Interpreter {

template<>
inline SemanticToken Visitor<SemanticToken>::evaluate(Expression* expr) {
  return expr->GetSemantic() = expr->accept(this);
}

template<typename T>
inline T Visitor<T>::evaluate(Expression* expr) {
  return expr->accept(this); 
}

};