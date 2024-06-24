#pragma once
#include "src/Lexer/Lexer.h"



namespace Zcc {

namespace Expr {
  class Literal;
  class Binary;
  class Unary;
  class Var;
};


class Expression;

class Visitor {
public:
  virtual void visitExprLiteral(Expr::Literal* expr) {}
  virtual void visitExprBinary(Expr::Binary* expr) {}
  virtual void visitExprUnary(Expr::Unary* expr) {}
  virtual void visitExprVariable(Expr::Var* expr) {}

  // virtual MemoryT visitStmtAddress(StatementAddress* stmt) { return {}; }
  // virtual MemoryT visitStmtAllocate(StatementAllocate* stmt) { return {}; }
  // virtual MemoryT visitStmtLambda(StatementLambda* stmt) { return {}; }
  // virtual MemoryT visitStmtNoArg(StatementNoArgCommand* stmt) { return {}; }
  // virtual MemoryT visitStmtOneArg(StatementOneArgCommand* stmt) { return {}; }
  // virtual MemoryT visitStmtVariable(StatementVariable* stmt) { return {}; }
  // virtual MemoryT visitStmtInclude(StatementInclude * stmt) { return {}; }

  virtual void visitExprUnknown(Expression* expr) {}
};

class Expression {
public:
  virtual void accept(Visitor* visitor) {
    return visitor->visitExprUnknown(this);
  }
};
};