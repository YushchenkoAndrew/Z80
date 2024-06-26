#pragma once
#include "Get.h"

namespace Zcc {
namespace Expr {

class Group : public Expression {
public:
  Group(std::shared_ptr<Expression> expr):
    expr(expr) {}
  
  
  void accept(Visitor* visitor) override {
    return visitor->visitExprGroup(this);
  }

public:
  std::shared_ptr<Expression> expr;
};
};
};

