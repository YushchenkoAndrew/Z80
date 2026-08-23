#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Statement/Statement.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementAddress : public Statement {
public:
  StatementAddress(Token label, Expression expr):
    label(label), expr(expr) {}

  inline SemanticToken accept(Visitor<SemanticToken>* visitor) override {
    return visitor->visitStmtAddress(this);
  }

public:
  SemanticToken semantic;
  SemanticToken& GetSemantic() override { return semantic; }

public:
  Token label;
  Expression expr;
};

};
