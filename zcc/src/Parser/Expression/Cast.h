#pragma once
#include "Binary.h"

namespace Zcc {

namespace Expr {

class Cast : public Expression {
public:
  Cast(std::shared_ptr<Expression> type, std::shared_ptr<Expression> expr):
    type(type), expr(expr) {}

  void accept(Visitor* visitor) override {
    return visitor->visitExprCast(this);
  }

public:
  std::shared_ptr<Expression> type;
  std::shared_ptr<Expression> expr;
};

};
};
