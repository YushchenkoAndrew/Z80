#pragma once
#include "Group.h"

namespace Zazy {
namespace Expr {

class Invoke : public Expression {
public:
  Invoke(expr_t left, std::vector<expr_t> args):
    left(left), args(args) {}
  
  
  obj_t accept(Visitor* visitor) override {
    return visitor->visitExprInvoke(this);
  }

  void print() { 
    ENCLOSE(
      left->print(); printf("("); 

      for (uint32_t i = 0; i < args.size(); i++) { 
        args[i]->print(); 
        if (i + 1 != args.size()) printf(", "); 
      }

      printf(")");
    )
  }

public:
  expr_t left;
  std::vector<expr_t> args;
};
};
};

