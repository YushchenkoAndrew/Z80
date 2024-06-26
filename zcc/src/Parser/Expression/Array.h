#pragma once
#include "src/Parser/Defs.h"

namespace Zcc {

namespace Expr {

class Array : public Expression {
public:
  Array(std::vector<std::shared_ptr<Expression>> args):
    args(args) {}

  void* accept(Visitor* visitor) override {
    return visitor->visitExprArray(this);
  }

public:
  std::vector<std::shared_ptr<Expression>> args;
};

};
};
