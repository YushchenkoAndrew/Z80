#pragma once
#include "Invoke.h"

namespace Zazy {
namespace Expr {

class Literal : public Expression {
public:
  Literal(token_t t): token(t) {}

  void* accept(Visitor* visitor) override {
    return visitor->visitExprLiteral(this);
  }

  void print() { printf("%s", token->lexeme.c_str()); }

public:
  token_t token;
};
};
};

