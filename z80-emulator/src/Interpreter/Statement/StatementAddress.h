#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementAddress : public Expression {
public:
  StatementAddress(Token label, std::unique_ptr<Expression> expr):
    label(label), expr(std::move(expr)) {}

  SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtAddress(this);
  }

public:
  Token label;
  std::unique_ptr<Expression> expr;
};

};
