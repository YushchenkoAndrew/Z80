#pragma once
#include "src/Lexer/Lexer.h"



namespace Zcc {

namespace Expr {
  class Array;
  class Assign;
  class Binary;
  class Cast;
  class Get;
  class Group;
  class Invoke;
  class Literal;
  class Suffix;
  class Ternary;
  class Type;
  class Unary;
  class Var;
};


class Expression;

class Visitor {
public:
  virtual void visitExprArray(Expr::Array* expr) {}
  virtual void visitExprAssign(Expr::Assign* expr) {}
  virtual void visitExprBinary(Expr::Binary* expr) {}
  virtual void visitExprCast(Expr::Cast* expr) {}
  virtual void visitExprGet(Expr::Get* expr) {}
  virtual void visitExprGroup(Expr::Group* expr) {}
  virtual void visitExprInvoke(Expr::Invoke* expr) {}
  virtual void visitExprLiteral(Expr::Literal* expr) {}
  virtual void visitExprSuffix(Expr::Suffix* expr) {}
  virtual void visitExprTernary(Expr::Ternary* expr) {}
  virtual void visitExprType(Expr::Type* expr) {}
  virtual void visitExprUnary(Expr::Unary* expr) {}
  virtual void visitExprVar(Expr::Var* expr) {}


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