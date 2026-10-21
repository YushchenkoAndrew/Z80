#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Token.h"

namespace Interpreter {

class StatementVariable : public Expression {
public:
  enum TypeT { ADDRESS, DEFINITION };

  StatementVariable(Token token, std::unique_ptr<Expression> expr):
    label(token), definition(std::move(expr)), type(DEFINITION) {}

  StatementVariable(Token t): label(t), type(ADDRESS) {}

  Bytes         accept(Visitor<Bytes>*         visitor) override { return visitor->visitStmtVariable(this); }
  uint32_t      accept(Visitor<uint32_t>*      visitor) override { return visitor->visitStmtVariable(this); }
  SemanticToken accept(Visitor<SemanticToken>* visitor) override { return visitor->visitStmtVariable(this); }

public:
  Token label;
  std::unique_ptr<Expression> definition;
  const TypeT type;
};

};
