#pragma once
#include "Cast.h"

namespace Zcc {

namespace Expr {

class Get : public Expression {
public:
  Get(std::shared_ptr<Expression> left, std::shared_ptr<Token> op, std::shared_ptr<Expression> right):
    left(left), operation(op), right(right) {}

  void accept(Visitor* visitor) override {
    return visitor->visitExprGet(this);
  }

public:
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
  std::shared_ptr<Token> operation;
};

};
};
