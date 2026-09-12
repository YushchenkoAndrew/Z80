#pragma once

#include "src/Interpreter/Expression/Expression.h"
#include "src/Interpreter/Lexer.h"
#include "src/Interpreter/Parser.h"
#include "src/Interpreter/Preprocessor.h"
#include "src/Interpreter/Semantic.h"
#include "src/Utils/Utils.h"
#include <cstdint>

namespace Interpreter {

class Interpreter : public Visitor<Bytes> {
public:
  Interpreter(): parser(lexer), semantic(parser) {}

  Interpreter& load(std::filesystem::path path) { return (preprocessor.scan(path), *this); }

  std::unordered_set<std::filesystem::path>& files() { return preprocessor.files; }

  std::list<std::string> scan() { return scan(preprocessor.source); }
  std::list<std::string> scan(std::string src) {
    const auto& err = semantic.scan(src);
    if (err.size()) return err;

//     if (parser.scan(src)) { 
//       for (auto& err : parser.errors) errors.push_back("File '" + filename.string() + "' " + err);
//       return true;
//     }

      // parser

//     if (vars == nullptr) {
//       env.reset();
//       for (auto& stmt : parser.stmt) env.insert(evaluate(stmt.get()));
//       return errors.size();
//     }

//     // NOTE: Main loop linker + final result
//     env.reset(); unlock(); 
//     for (auto& stmt : parser.stmt) env.insert(evaluate(stmt.get()));

//     // Interpreter
//     env.reset(); lock();
//     for (auto& stmt : parser.stmt) env.insert(evaluate(stmt.get()));


//     for (auto& var : env.unknown) {
//       error(nullptr, "Unknown variable '" + var + "'.");
//     }

    return {};
  }

  Bytes visitExprUnknown(Expression* expr) override { return {}; }

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

      default: return {};
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

      default: return {};
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

      default: return {};
    }
  }

  Bytes visitExprVariable(ExpressionVariable* expr) override {
    return evaluate(semantic.vars[expr->label.lexeme]);
  }

  Bytes visitStmtAddress(StatementAddress* stmt) override {
//     // MemoryT expr = evaluate(stmt->expr.get());
//     // if (expr.size() > 2) {
//     //   error(stmt->label, "Address byte size exceeded allowed size.");
//     //   return {};
//     // }

//     // env.allocate(expr);
    return {};
  }

  Bytes visitStmtAllocate(StatementAllocate* stmt) override {
//     // MemoryT res;

//     // for (auto& expr : stmt->data) {
//     //   MemoryT bytes = evaluate(expr.get()); const auto nSize = (int32_t)bytes.size() / 2;
//     //   for (int32_t i = 1; i < stmt->size - nSize; i++) bytes.insert(bytes.begin(), 0);

//     //   // TODO: Bellow is quick fix, need to think about how impl this better
//     //   // Right now `dw "key"` works incorrectly
//     //   if (stmt->reverse) res.insert(res.end(), bytes.rbegin(), bytes.rend());
//     //   else res.insert(res.end(), bytes.begin(), bytes.end());
//     // }

//     // return res;
    return {};
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
    std::vector<uint32_t> args;
    for (auto& expr : stmt->args) args.push_back(Bytes2Int(evaluate(expr.get())));

    auto opcode = stmt->calc(args[0], args[1]);
    return write(Int2Bytes(opcode)), Bytes();
  }

  Bytes visitStmtVariable(StatementVariable* stmt) override {
    switch (stmt->type) {
      case StatementVariable::TypeT::ADDRESS:
        // TODO: think about this ?????
        // ? maybe can be done in semantic level ????
        // env.define(stmt->label->lexeme, Int2Bytes(env.addr));
        return {};

      case StatementVariable::TypeT::DEFINITION: 
        return evaluate(stmt->definition.get());

      default: return {};
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
    address += bytes.size();
    memory.insert(memory.end(), bytes.begin(), bytes.end());
  }

public:
  Bytes memory;
  uint32_t address = 0x0000;

private:
  Preprocessor preprocessor;
  Lexer lexer;
  Parser parser;
  Semantic semantic;
};

};
