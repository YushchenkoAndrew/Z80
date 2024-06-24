#pragma once
#include "Unary.h"

namespace Zcc {
namespace Expr {

class Var : public Expression {
public:
  Var(std::shared_ptr<Token> t):
    token(t) {}
  
  
  inline void accept(Visitor* visitor) override {
    return visitor->visitExprVariable(this);
  }

public:
  std::shared_ptr<Token> token;
};
};
};

