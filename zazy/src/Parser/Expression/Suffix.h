#pragma once
#include "Literal.h"

namespace Zazy {
namespace Expr {

class Suffix : public Expression {
public:
  Suffix(expr_t left, token_t op):
    operation(op), left(left) {}
  
  
  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprSuffix(this);
  }

  void print() { ENCLOSE(left->print(); printf("%s", operation->lexeme.c_str());) }

public:
  expr_t left;
  token_t operation;
};
};
};

