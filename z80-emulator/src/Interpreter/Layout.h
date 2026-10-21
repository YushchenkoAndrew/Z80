#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Parser.h"
#include "src/Interpreter/Semantic.h"
#include "src/Utils/Utils.h"

namespace Interpreter {

class Layout : public Visitor<uint32_t> {
public:
  Layout(Parser& p, Semantic& s): parser(p), semantic(s) {}

  std::list<std::string> scan(std::string& src) {
    const auto& err = semantic.scan(src);
    if (err.size()) return err;

    for (auto& stmt : parser.stmt) evaluate(stmt.get());
    return {};
  }

  uint32_t visitExprUnknown(Expression* expr) override { return 0; }

  uint32_t visitExprLiteral(ExpressionLiteral* expr) override {
    switch (expr->token.type) {
      case TokenT::NUMBER: return Utils::Str2Uint(expr->token.literal).second;
      case TokenT::STRING: return 0;
      default:             return 0;
    }
  }

  uint32_t visitExprBinary(ExpressionBinary* expr) override {
    uint32_t left = evaluate(expr->left.get());
    uint32_t right = evaluate(expr->right.get());

    switch (expr->op.type) {
      case TokenT::PLUS:        return left + right;
      case TokenT::MINUS:       return left - right;
      case TokenT::BIT_OR:      return left | right;
      case TokenT::BIT_AND:     return left & right;
      case TokenT::BIT_XOR:     return left ^ right;
      case TokenT::RIGHT_SHIFT: return left >> right;
      case TokenT::LEFT_SHIFT:  return left << right;
      case TokenT::CONCATENATE: return 0;
      default:                  return 0;
    }
  }

  uint32_t visitExprUnary(ExpressionUnary* expr) override {
    uint32_t right = evaluate(expr->right.get());

    switch (expr->op.type) {
      case TokenT::BIT_NOT: return ~right;
      case TokenT::MINUS:   return -right;
      default:              return 0;
    }
  }

  uint32_t visitExprVariable(ExpressionVariable* expr) override {
    if (expr->label.lexeme == "$") return address;

    const auto ref = semantic.vars[expr->label.lexeme];
    return ref->type == StatementVariable::TypeT::ADDRESS ? 0 : evaluate(ref);
  }

  uint32_t visitStmtAddress(StatementAddress* stmt) override {
    return address = evaluate(stmt->expr.get()), 0;
  }

  uint32_t visitStmtAllocate(StatementAllocate* stmt)             override { return address += stmt->semantic.bytes, 0; }
  uint32_t visitStmtNoArgCommand(StatementNoArgCommand* stmt)     override { return address += stmt->semantic.bytes, 0; }
  uint32_t visitStmtOneArgCommand(StatementOneArgCommand* stmt)   override { return address += stmt->semantic.bytes, 0; }
  uint32_t visitStmtRuntimeCommand(StatementRuntimeCommand* stmt) override { return address += stmt->semantic.bytes, 0; }

  uint32_t visitStmtVariable(StatementVariable* stmt) override {
    switch (stmt->type) {
      case StatementVariable::TypeT::ADDRESS:
        return stmt->semantic.address = address;

      case StatementVariable::TypeT::DEFINITION: 
        return evaluate(stmt->definition.get());

      default: return 0;
    }
  }

private:
  Parser& parser;
  Semantic& semantic;

  uint32_t address = 0x0000;
};

};
