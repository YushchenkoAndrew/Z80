#pragma once
#include "Assign.h"

namespace Zcc {

namespace Expr {

class Binary : public Expression {
public:
  Binary(std::shared_ptr<Expression> left, std::shared_ptr<Token> op, std::shared_ptr<Expression> right):
    left(left), operation(op), right(right) {}

  void accept(Visitor* visitor) override {
    return visitor->visitExprBinary(this);
  }

public:
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
  std::shared_ptr<Token> operation;
};

};
};
