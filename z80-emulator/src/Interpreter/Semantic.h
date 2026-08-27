#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Expression/ExpressionLiteral.h"
#include "src/Interpreter/Expression/ExpressionBinary.h"
#include "src/Interpreter/Expression/ExpressionUnary.h"
#include "src/Interpreter/Expression/ExpressionBinary.h"
#include "src/Interpreter/Expression/ExpressionLiteral.h"
#include "src/Interpreter/Expression/ExpressionUnary.h"
#include "src/Interpreter/Expression/ExpressionVariable.h"
#include "src/Interpreter/Parser.h"
#include "src/Interpreter/Statement/StatementAddress.h"
#include "src/Interpreter/Statement/StatementAllocate.h"
#include "src/Interpreter/Statement/StatementNoArgCommand.h"
#include "src/Interpreter/Statement/StatementOneArgCommand.h"
#include "src/Interpreter/Statement/StatementRuntimeCommand.h"
#include "src/Interpreter/Statement/StatementVariable.h"
#include "src/Interpreter/Token.h"
#include "src/Utils/Utils.h"
#include <algorithm>
#include <cstdint>
#include <string>

namespace Interpreter {

class Semantic : public Visitor<SemanticToken> {
public:
  Semantic(Parser& p): parser(p) {}

  std::list<std::string> scan(std::string& src) {
    errors.clear();

    const auto& err = parser.scan(src);
    if (err.size()) return err;

    for (auto& stmt : parser.stmt) evaluate(stmt.get());
    return errors;
  }

  SemanticToken visitExprLiteral(ExpressionLiteral* expr) override {
    switch (expr->token.type) {
      case TokenT::NUMBER: {
        const auto [valid, value] = Utils::Str2Uint(expr->token.literal);
        if (!valid) return error(expr->token, "Invalid number format.");
        if (expr->bytes && Utils::SizeOf(value) > expr->bytes) return error(expr->token, "Number byte size exceeded allowed size.");
        return SemanticToken(expr->token, std::max(expr->bytes, Utils::SizeOf(value)), expr->bytes);
      }

      case TokenT::STRING: 
        if (expr->bytes && expr->token.literal.size() > expr->bytes) return error(expr->token, "String byte size exceeded allowed size.");
        return SemanticToken(expr->token, std::max(expr->bytes, (uint32_t)expr->token.literal.size()), expr->bytes);

      default: return error(expr->token, "Unknown literal.");
    }
  }

  SemanticToken visitExprBinary(ExpressionBinary* expr) override {
    auto left = evaluate(expr->left.get());
    auto right = evaluate(expr->right.get());

    if (expr->op.type == TokenT::CONCATENATE) return SemanticToken(expr->op, left.bytes + right.bytes, false);

    if (left.strict && right.bytes > left.bytes) return error(expr->op, "Right side exceeded allowed values size of: " + std::to_string(left.bytes));
    if (right.strict && left.bytes > right.bytes) return error(expr->op, "Left side exceeded allowed values size of: " + std::to_string(right.bytes));

    switch (expr->op.type) {
      case TokenT::BIT_OR: case TokenT::BIT_AND: case TokenT::BIT_XOR:
      case TokenT::PLUS: case TokenT::MINUS: case TokenT::RIGHT_SHIFT: case TokenT::LEFT_SHIFT: 
        return SemanticToken(expr->op, left.strict ? left.bytes : right.strict ? right.bytes : std::max(left.bytes, right.bytes), left.strict || right.strict);
      default: return error(expr->op, "Unknown operation.");
    }
  }

  SemanticToken visitExprUnary(ExpressionUnary* expr) override {
    auto right = evaluate(expr->right.get());
    // if (right.type != SemanticToken::Type::VALUE) return error(expr->op, "Unary value must be a simple value");

    switch (expr->op.type) {
      case TokenT::MINUS: case TokenT::BIT_NOT:
        return SemanticToken(expr->op, right.bytes, right.strict);
      default: return error(expr->op, "Unknown operation.");
    }
  }

  SemanticToken visitExprVariable(ExpressionVariable* expr) override {
    // int32_t size = expr->length != nullptr ? std::stol(expr->length->literal) : expr->size;
    // if (!size && !env.has(expr->token->lexeme)) {
    //   error(expr->token, "Variable byte size need to be explicitly defined.");
    //   return {};
    // }

    // auto bytes = env.get(expr->token->lexeme, size);
    // if (!size || bytes.size() == size) return bytes;

    // error(expr->token, "Variable byte size exceeded allowed size.");
    return {};
  }

  SemanticToken visitStmtAddress(StatementAddress* stmt) override {
    auto right = evaluate(stmt->expr.get());

    // ? I think I need this value to determine the offset .....
    if (right.bytes > 2) return error(stmt->label, "Address byte size exceeded allowed size.");
    return SemanticToken(stmt->label, 0, false);
  }

  SemanticToken visitStmtAllocate(StatementAllocate* stmt) override {
    uint32_t bytes = 0;

    for (auto& it : stmt->data) {
      bytes += (evaluate(it.get()).bytes << (stmt->size - 1));
    }

    return SemanticToken(stmt->token, bytes, false);
  }

  SemanticToken visitStmtNoArgCommand(StatementNoArgCommand* stmt) override {
    return SemanticToken(stmt->token, Utils::SizeOf(stmt->opcode), false);
  }

  SemanticToken visitStmtOneArgCommand(StatementOneArgCommand* stmt) override {
    SemanticToken expr = evaluate(stmt->expr.get());
    return SemanticToken(stmt->token, Utils::SizeOf(stmt->opcode) + expr.bytes, false);
  }

  SemanticToken visitStmtRuntimeCommand(StatementRuntimeCommand* stmt) override {
    uint32_t bytes = 0;
    for (auto& arg : stmt->args) bytes += evaluate(arg.get()).bytes;

    uint32_t opcode = stmt->calc(0, 0);
    if (!opcode) error(stmt->token, "Invalid lambda operation.");

    return SemanticToken(stmt->token, Utils::SizeOf(opcode), false);
  }

  SemanticToken visitStmtVariable(StatementVariable* stmt) override {
    // switch (stmt->type) {
    //   case StatementVariable::TypeT::ADDRESS:
    //     env.define(stmt->label->lexeme, Int2Bytes(env.addr));
    //     return {};


    //   case StatementVariable::TypeT::DEFINITION: 
    //     env.define(stmt->label->lexeme, evaluate(stmt->definition.get()));
    //     return {};
    // }

    // error(nullptr, "Invalid variable.");
    return {};
  }

  SemanticToken visitExprUnknown(Expression* expr) override {
    auto empty = Token();
    return error(empty, "Unknown operation.");
  }

private:
  SemanticToken error(Token& token, std::string message) {
    // errors.push_back("File '" + filename.string() << "' " + "[Ln " + token.line << " Col " << token.col << "] Error at '" << token.lexeme << "': " << message << "\n";
    errors.push_back(std::string("File '") + "TODO: filename" + "' [Ln " + std::to_string(token.line) + " Col " + std::to_string(token.col) + "] Error at '" + token.lexeme + "': " + message + "\n");
    return SemanticToken();
  }

private:
  Parser& parser;
  std::list<std::string> errors;
};

};
