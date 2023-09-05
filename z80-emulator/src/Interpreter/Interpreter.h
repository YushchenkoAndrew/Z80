#pragma once
#include "Environment.h"

namespace Interpreter {

class Interpreter : public Visitor<MemoryT> {
public:

  Interpreter(): parser(Parser()) {}
  Interpreter(Parser l): parser(l) {}

  // ~Parser() { reset(); }

  template <int32_t U>
  MemoryT visit(Int2Type<U>, Expression* expr) {
    // TODO:
  }

  MemoryT visit(Int2Type<EXPR_BINARY>, ExpressionBinary* expr) {
    // TODO:
  }

  MemoryT visit(Int2Type<EXPR_LITERAL>, ExpressionLiteral* expr) {
    // TODO:
  }

  MemoryT visit(Int2Type<EXPR_UNARY>, ExpressionUnary* expr) {
    // TODO:
  }

  MemoryT visit(Int2Type<EXPR_VARIABLE>, ExpressionVariable* expr) {
    // TODO:
  }

  MemoryT visit(Int2Type<STMT_ADDRESS>, StatementAddress* stmt) {
    // TODO:
  }

  MemoryT visit(Int2Type<STMT_ALLOCATE>, StatementAllocate* stmt) {
    // TODO:
  }

  MemoryT visit(Int2Type<STMT_LAMBDA>, StatementLambda* stmt) {
    // TODO:
  }

  MemoryT visit(Int2Type<STMT_NO_ARG>, StatementNoArgCommand* stmt) {
    // TODO:
  }

  MemoryT visit(Int2Type<STMT_ONE_ARG>, StatementOneArgCommand* stmt) {
    // TODO:
  }

  MemoryT visit(Int2Type<STMT_VARIABLE>, StatementVariable* stmt) {
    // TODO:
  }

public:
  Environment env;
  Parser parser;
};

};
