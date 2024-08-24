#pragma once
#include "src/Lexer/Lexer.h"



namespace Zazy {

namespace Expr {
  #define ENCLOSE(...) printf("("); __VA_ARGS__ printf(")");

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
  class Unary;
  class Var;
};

class Expression;

typedef std::shared_ptr<Expression> expr_t;

class Visitor {
public:
  virtual void* visitExprArray(Expr::Array* expr) { return nullptr; }
  virtual void* visitExprAssign(Expr::Assign* expr) { return nullptr; }
  virtual void* visitExprBinary(Expr::Binary* expr) { return nullptr; }
  virtual void* visitExprCast(Expr::Cast* expr) { return nullptr; }
  virtual void* visitExprGet(Expr::Get* expr) { return nullptr; }
  virtual void* visitExprGroup(Expr::Group* expr) { return nullptr; }
  virtual void* visitExprInvoke(Expr::Invoke* expr) { return nullptr; }
  virtual void* visitExprLiteral(Expr::Literal* expr) { return nullptr; }
  virtual void* visitExprSuffix(Expr::Suffix* expr) { return nullptr; }
  virtual void* visitExprTernary(Expr::Ternary* expr) { return nullptr; }
  virtual void* visitExprUnary(Expr::Unary* expr) { return nullptr; }
  virtual void* visitExprVar(Expr::Var* expr) { return nullptr; }


  // virtual MemoryT visitStmtAddress(StatementAddress* stmt) { return {}; }
  // virtual MemoryT visitStmtAllocate(StatementAllocate* stmt) { return {}; }
  // virtual MemoryT visitStmtLambda(StatementLambda* stmt) { return {}; }
  // virtual MemoryT visitStmtNoArg(StatementNoArgCommand* stmt) { return {}; }
  // virtual MemoryT visitStmtOneArg(StatementOneArgCommand* stmt) { return {}; }
  // virtual MemoryT visitStmtVariable(StatementVariable* stmt) { return {}; }
  // virtual MemoryT visitStmtInclude(StatementInclude * stmt) { return {}; }

  virtual void* visitExprUnknown(Expression* expr) { return nullptr; }
};

class Expression {
public:
  virtual void* accept(Visitor* visitor) {
    return visitor->visitExprUnknown(this);
  }
  
  virtual void print() {}
};

};