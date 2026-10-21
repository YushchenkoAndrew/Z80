#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Layout.h"
#include "src/Interpreter/Lexer.h"
#include "src/Interpreter/Parser.h"
#include "src/Interpreter/Preprocessor.h"
#include "src/Interpreter/Semantic.h"
#include "src/Utils/Utils.h"
#include <array>
#include <cstdint>

namespace Interpreter {

class Interpreter : public Visitor<Bytes> {
public:
  Interpreter(): parser(lexer), semantic(parser), layout(parser, semantic) {}

  Interpreter& load(std::filesystem::path path) { return (preprocessor.scan(path), *this); }

  std::unordered_set<std::filesystem::path>& files() { return preprocessor.files; }

  std::list<std::string> scan() { return scan(preprocessor.source); }
  std::list<std::string> scan(std::string src) {
    const auto& err = layout.scan(src);
    if (err.size()) return err;

    for (auto& stmt : parser.stmt) evaluate(stmt.get());
    return {};
  }

  Bytes visitExprUnknown(Expression* expr) override { return Bytes(); }

  Bytes visitExprLiteral(ExpressionLiteral* expr) override {
    switch (expr->token.type) {
      case TokenT::NUMBER: {
        auto [_, value] = Utils::Str2Uint(expr->token.literal);
        return this->Int2Bytes(value, expr->semantic.bytes);
      }

      case TokenT::STRING: {
        Bytes bytes;
        for (auto c : expr->token.literal) bytes.push_back(c);
        return bytes;
      }

      default: return Bytes();
    }
  }

  Bytes visitExprBinary(ExpressionBinary* expr) override {
    Bytes left = evaluate(expr->left.get());
    Bytes right = evaluate(expr->right.get());

    switch (expr->op.type) {
      case TokenT::CONCATENATE:
        left.insert(left.end(), right.begin(), right.end());
        return left;

      case TokenT::PLUS:
        return Int2Bytes(Bytes2Int(left) + Bytes2Int(right), expr->semantic.bytes);

      case TokenT::MINUS:
        return Int2Bytes(Bytes2Int(left) + ~Bytes2Int(right) + 1, expr->semantic.bytes);

      case TokenT::BIT_OR:
        return Int2Bytes(Bytes2Int(left) | Bytes2Int(right), expr->semantic.bytes);

      case TokenT::BIT_AND:
        return Int2Bytes(Bytes2Int(left) & Bytes2Int(right), expr->semantic.bytes);

      case TokenT::BIT_XOR:
        return Int2Bytes(Bytes2Int(left) ^ Bytes2Int(right), expr->semantic.bytes);

      case TokenT::RIGHT_SHIFT: 
        return Int2Bytes(Bytes2Int(left) >> Bytes2Int(right), expr->semantic.bytes);

      case TokenT::LEFT_SHIFT: 
        return Int2Bytes(Bytes2Int(left) << Bytes2Int(right), expr->semantic.bytes);

      default: return Bytes();
    }
  }

  Bytes visitExprUnary(ExpressionUnary* expr) override {
    Bytes right = evaluate(expr->right.get());
    auto size = right.size();

    switch (expr->op.type) {
      case TokenT::BIT_NOT:
        for (auto& val : right) val = val ^ 0xFF;
        return right;

      case TokenT::MINUS:
        return Int2Bytes(Utils::TruncateInt(~Bytes2Int(right) + 1, size), size);

      default: return Bytes();
    }
  }

  Bytes visitExprVariable(ExpressionVariable* expr) override {
    if (expr->label.lexeme == "$") return Int2Bytes(address);
    return evaluate(semantic.vars[expr->label.lexeme]);
  }

  Bytes visitStmtAddress(StatementAddress* stmt) override {
    address = Bytes2Int(evaluate(stmt->expr.get()));
    if (memory.size() < address) memory.insert(memory.end(), address - memory.size(), 0);

    return Bytes();
  }

  Bytes visitStmtAllocate(StatementAllocate* stmt) override {
    for (const auto& expr : stmt->data) {
      Bytes bytes = evaluate(expr.get());

      if (!expr->semantic.sequence) {
        if (stmt->size > bytes.size()) bytes.insert(bytes.begin(), stmt->size - bytes.size(), 0);
        std::reverse(bytes.begin(), bytes.end());
      } else if (stmt->size > 1) {
        const uint8_t length = bytes.size();
        for (uint8_t i = 0; i < length; i += stmt->size) bytes.insert(bytes.begin() + i, 0);
      } 

      write(bytes);
    }

    return Bytes();
  }


  Bytes visitStmtNoArgCommand(StatementNoArgCommand* stmt) override {
    return write(Int2Bytes(stmt->opcode)), Bytes();
  }

  Bytes visitStmtOneArgCommand(StatementOneArgCommand* stmt) override {
    Bytes expr = evaluate(stmt->expr.get());
    Bytes opcode = Int2Bytes(stmt->opcode);

    opcode.insert(opcode.end(), expr.rbegin(), expr.rend());
    return write(opcode), Bytes();
  }

  Bytes visitStmtRuntimeCommand(StatementRuntimeCommand* stmt) override {
    std::array<uint32_t, 2> args = { 0, 0 }; uint32_t index = 0;
    for (auto& expr : stmt->args) args[index++] = Bytes2Int(evaluate(expr.get()));

    auto opcode = stmt->calc(args[0], args[1]);
    return write(Int2Bytes(opcode)), Bytes();
  }

  Bytes visitStmtVariable(StatementVariable* stmt) override {
    switch (stmt->type) {
      case StatementVariable::TypeT::ADDRESS:
        return Int2Bytes(stmt->semantic.address);

      case StatementVariable::TypeT::DEFINITION: 
        return evaluate(stmt->definition.get());

      default: return Bytes();
    }
  }

private:
//   inline void lock() { if (vars) vars->first = true; }
//   inline void unlock() { if (vars) { vars->first = false; vars->second.clear(); } }

  inline Bytes Int2Bytes(uint32_t val, int32_t size = 0) {
    if (val == 0x00) return Bytes(size ? size : 1, 0x00);

    Bytes result;
    std::array<uint8_t, 4> bytes = {
      (uint8_t)((val >> 24) & 0xFF),
      (uint8_t)((val >> 16) & 0xFF),
      (uint8_t)((val >>  8) & 0xFF),
      (uint8_t)((val >>  0) & 0xFF),
    };

    if (size) result.insert(result.end(), bytes.end() - size, bytes.end());
    else {
      for (int32_t i = 0; i < bytes.size(); i++) {
        if (bytes[i] == 0x00 && !result.size()) continue;
        result.push_back(bytes[i]);
      }
    } 

    return result;
  }

  inline uint32_t Bytes2Int(Bytes bytes) {
    uint32_t result = 0;

    for (auto& byte: bytes) result = (result << 8) | byte;
    return result;
  }

  inline void write(Bytes bytes) {
    memory.insert(memory.begin() + address, bytes.begin(), bytes.end());
    address += bytes.size();
  }

public:
  Bytes memory;
  uint32_t address = 0x0000;

private:
  Preprocessor preprocessor;
  Lexer lexer;
  Parser parser;
  Semantic semantic;
  Layout layout;
};

};
