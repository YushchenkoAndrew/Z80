#pragma once
#include "Unary.h"

namespace Zazy {
namespace Expr {

class Var : public Expression {
public:
  Var(token_t t): token(t) {}
  
  
  void* accept(Visitor* visitor) override {
    return visitor->visitExprVar(this);
  }

  void print() { printf("%s", token->lexeme.c_str()); }

public:
  token_t token;
};
};
};

