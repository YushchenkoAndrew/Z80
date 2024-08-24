#pragma once
#include "src/Parser/Defs.h"

namespace Zazy {

namespace Expr {

class Array : public Expression {
public:
  Array(std::vector<expr_t> args):
    args(args) {}

  void* accept(Visitor* visitor) override {
    return visitor->visitExprArray(this);
  }

  // void print() { ENCLOSE(printf("%s", operation->lexeme.c_str()); right->print();) }

public:
  std::vector<expr_t> args;
};

};
};
