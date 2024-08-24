#pragma once
#include "Group.h"

namespace Zazy {
namespace Expr {

class Invoke : public Expression {
public:
  Invoke(expr_t left, std::vector<expr_t> args):
    left(left), args(args) {}
  
  
  void* accept(Visitor* visitor) override {
    return visitor->visitExprInvoke(this);
  }

  // void print() { ENCLOSE(left->print(); printf("%s", type->lexeme.c_str()); ) }

public:
  expr_t left;
  std::vector<expr_t> args;
};
};
};

