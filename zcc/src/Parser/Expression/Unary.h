#pragma once
#include "Suffix.h"

namespace Zcc {
namespace Expr {

class Unary : public Expression {
public:
  Unary(std::shared_ptr<Token> op, std::shared_ptr<Expression> right):
    operation(op), right(right) {}
  
  
  void accept(Visitor* visitor) override {
    return visitor->visitExprUnary(this);
  }

public:
  std::shared_ptr<Expression> right;
  std::shared_ptr<Token> operation;
};
};
};

