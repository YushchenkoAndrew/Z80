#pragma once
#include "Array.h"

namespace Zcc {

namespace Expr {

class Assign : public Expression {
public:
  Assign(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right):
    left(left), right(right) {}

  void accept(Visitor* visitor) override {
    return visitor->visitExprAssign(this);
  }

public:
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
};

};
};
