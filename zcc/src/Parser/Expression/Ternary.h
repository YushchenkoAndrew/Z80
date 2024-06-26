#pragma once
#include "Suffix.h"

namespace Zcc {
namespace Expr {

class Ternary : public Expression {
public:
  Ternary(std::shared_ptr<Expression> condition, std::shared_ptr<Expression> left, std::shared_ptr<Expression> right):
    condition(condition), left(left), right() {}
  
  
  void* accept(Visitor* visitor) override {
    return visitor->visitExprTernary(this);
  }

public:
  std::shared_ptr<Expression> condition;
  std::shared_ptr<Expression> left;
  std::shared_ptr<Expression> right;
  // std::shared_ptr<Token> operation;
};
};
};

