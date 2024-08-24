#pragma once
#include "Ternary.h"

namespace Zazy {
namespace Expr {

class Unary : public Expression {
public:
  Unary(token_t op, expr_t right):
    operation(op), right(right) {}
  
  
  void* accept(Visitor* visitor) override {
    return visitor->visitExprUnary(this);
  }

  void print() { ENCLOSE(printf("%s", operation->lexeme.c_str()); right->print();) }

public:
  expr_t right;
  token_t operation;
};
};
};

