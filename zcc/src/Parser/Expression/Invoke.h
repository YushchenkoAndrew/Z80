#pragma once
#include "Group.h"

namespace Zcc {
namespace Expr {

class Invoke : public Expression {
public:
  Invoke(std::shared_ptr<Expression> left, std::vector<std::shared_ptr<Expression>> args):
    left(left), args(args) {}
  
  
  void accept(Visitor* visitor) override {
    return visitor->visitExprInvoke(this);
  }

public:
  std::shared_ptr<Expression> left;
  std::vector<std::shared_ptr<Expression>> args;
};
};
};

