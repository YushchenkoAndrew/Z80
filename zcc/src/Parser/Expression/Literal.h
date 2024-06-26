#pragma once
#include "Invoke.h"

namespace Zcc {
namespace Expr {

class Literal : public Expression {
public:
  Literal(std::shared_ptr<Token> t): 
    token(t) {}

  void* accept(Visitor* visitor) override {
    return visitor->visitExprLiteral(this);
  }

public:
  std::shared_ptr<Token> token;
};
};
};

