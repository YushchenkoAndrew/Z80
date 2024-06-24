#pragma once
#include "Literal.h"

namespace Zcc {
namespace Expr {

class Suffix : public Expression {
public:
  Suffix(std::shared_ptr<Expression> left, std::shared_ptr<Token> op):
    operation(op), left(left) {}
  
  
  void accept(Visitor* visitor) override {
    return visitor->visitExprUnary(this);
  }

public:
  std::shared_ptr<Expression> left;
  std::shared_ptr<Token> operation;
};
};
};

