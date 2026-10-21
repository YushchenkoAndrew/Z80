#pragma once

#include "src/Interpreter/Token.h"
#include <cstdint>

namespace Interpreter {

typedef std::vector<uint8_t> Bytes;
// typedef std::pair<bool, std::unordered_map<std::string, MemoryT>> VariableT;

class Expression;
class ExpressionLiteral;
class ExpressionBinary;
class ExpressionUnary;
class ExpressionVariable;

class StatementAddress;
class StatementAllocate;
class StatementNoArgCommand;
class StatementOneArgCommand;
class StatementRuntimeCommand;
class StatementVariable;

class SemanticToken {
public:
  SemanticToken() = default;
  SemanticToken(Token token, uint32_t bytes, bool strict, bool sequence = false):
    token(std::move(token)), bytes(bytes), strict(strict), sequence(sequence) {}

public:
  Token token;
  bool strict;
  bool sequence;
  uint32_t bytes;
  uint32_t address;
};


template<typename T>
class Visitor {
public:
  virtual T visitExprLiteral(ExpressionLiteral* expr) = 0;
  virtual T visitExprBinary(ExpressionBinary* expr) = 0;
  virtual T visitExprUnary(ExpressionUnary* expr) = 0;
  virtual T visitExprVariable(ExpressionVariable* expr) = 0;

  virtual T visitStmtAddress(StatementAddress* stmt) = 0;
  virtual T visitStmtAllocate(StatementAllocate* stmt) = 0;
  virtual T visitStmtVariable(StatementVariable* stmt) = 0;
  virtual T visitStmtNoArgCommand(StatementNoArgCommand* stmt) = 0;
  virtual T visitStmtOneArgCommand(StatementOneArgCommand* stmt) = 0;
  virtual T visitStmtRuntimeCommand(StatementRuntimeCommand* stmt) = 0;

  virtual T visitExprUnknown(Expression* expr) = 0;

  T evaluate(Expression* expr);
};

class Expression {
public:
  virtual Bytes         accept(Visitor<Bytes>*         visitor) { return visitor->visitExprUnknown(this); }
  virtual uint32_t      accept(Visitor<uint32_t>*      visitor) { return visitor->visitExprUnknown(this); }
  virtual SemanticToken accept(Visitor<SemanticToken>* visitor) { return visitor->visitExprUnknown(this); }

  SemanticToken semantic;
};

template<>
inline SemanticToken Visitor<SemanticToken>::evaluate(Expression* expr) {
  return expr->semantic = expr->accept(this);
}

template<typename T>
inline T Visitor<T>::evaluate(Expression* expr) {
  return expr->accept(this); 
}
};
