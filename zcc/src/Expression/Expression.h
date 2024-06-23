#pragma once
#include "src/Token.h"

namespace Zcc {

typedef std::vector<uint8_t> MemoryT;

class Expression;
class ExpressionLiteral;
class ExpressionBinary;
class ExpressionUnary;
class ExpressionVariable;

class StatementAddress;
class StatementAllocate;
class StatementLambda;
class StatementNoArgCommand;
class StatementOneArgCommand;
class StatementVariable;
class StatementInclude;

class Visitor {
public:
  virtual MemoryT visitExprLiteral(ExpressionLiteral* expr) { return {}; }
  virtual MemoryT visitExprBinary(ExpressionBinary* expr) { return {}; }
  virtual MemoryT visitExprUnary(ExpressionUnary* expr) { return {}; }
  virtual MemoryT visitExprVariable(ExpressionVariable* expr) { return {}; }

  virtual MemoryT visitStmtAddress(StatementAddress* stmt) { return {}; }
  virtual MemoryT visitStmtAllocate(StatementAllocate* stmt) { return {}; }
  virtual MemoryT visitStmtLambda(StatementLambda* stmt) { return {}; }
  virtual MemoryT visitStmtNoArg(StatementNoArgCommand* stmt) { return {}; }
  virtual MemoryT visitStmtOneArg(StatementOneArgCommand* stmt) { return {}; }
  virtual MemoryT visitStmtVariable(StatementVariable* stmt) { return {}; }
  virtual MemoryT visitStmtInclude(StatementInclude * stmt) { return {}; }

  virtual MemoryT visitExprUnknown(Expression* expr) { return {}; }
};

class Expression {
public:

  inline virtual MemoryT accept(Visitor* visitor) {
    return visitor->visitExprUnknown(this);
  }
};

};
