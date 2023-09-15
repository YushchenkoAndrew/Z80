#pragma once
#include "Environment.h"

namespace Interpreter {

class Interpreter : public Visitor {
public:

  Interpreter(): parser(Parser()) {}
  Interpreter(Parser p): parser(p) {}

  bool scan(std::string src) {
    reset();

    if (parser.scan(src)) { 
      errors.insert(errors.end(), parser.errors.begin(), parser.errors.end());
      return true;
    }

    for (auto& stmt : parser.stmt) {
      env.insert(evaluate(stmt.get()));
    }

    if (env.unknown.size()) {
      std::stringstream ss;
      for (auto& pair : env.unknown) {
        ss.str(""); ss << "Unknown variable '" << pair.first << "'.";
        error(nullptr, ss.str());
      }
    }

    return errors.size();
  }

  inline void reset() { errors.clear(); env.reset(); }

  MemoryT visitExprUnknown(Expression* expr) override {
    error(nullptr, "Unknown operation.");
    return {};
  }

  MemoryT visitExprLiteral(ExpressionLiteral* expr) override {
    switch (expr->token->token) {
      case TokenT::NUMBER: {
        uint32_t num = std::stoul(expr->token->literal);
        if (expr->size && (num & (0xFF << (expr->size * 8))) != 0) {
          error(expr->token, "Number byte size exceeded allowed size.");
          return {};
        }

        return Int2Bytes(num, expr->size);
      }

      case TokenT::STRING: {
        if (expr->size && expr->token->literal.size() != expr->size) {
          error(expr->token, "String byte size exceeded allowed size.");
          return {};
        }

        MemoryT bytes;
        for (auto c : expr->token->literal) bytes.push_back((uint8_t)c);
        return bytes;
      }
    }

    error(expr->token, "Unknown literal.");
    return {};
  }

  MemoryT visitExprBinary(ExpressionBinary* expr) override {
    MemoryT left = evaluate(expr->left.get());
    MemoryT right = evaluate(expr->right.get());

    if (expr->operation->token == TokenT::CONCATENATE) {
      left.insert(left.end(), right.begin(), right.end());
      return left;
    }

    int32_t diff = left.size() - right.size();
    if (diff > 0) right.insert(right.begin(), diff, 0);
    else if (diff < 0) left.insert(left.begin(), std::abs(diff), 0);

    switch (expr->operation->token) {
      case TokenT::PLUS:
        for (int32_t i = left.size(); i >= 0; i--) {
          uint16_t acc = left[i] + right[i];
          uint8_t carry = (acc & 0xFF00) >> 8;

          left[i] = acc & 0xFF;
          if (!carry) continue;

          if (!i) { left.insert(left.begin(), carry); break; }

          left[i - 1] += carry;
        }

        return left;

      case TokenT::MINUS:
        for (int32_t i = left.size(); i >= 0; i--) {
          uint16_t acc = left[i] + ~right[i] + 1;
          uint8_t carry = (acc & 0xFF00) >> 8;

          left[i] = acc & 0xFF;
          if (!carry) continue;

          if (i) { left[i - 1] += carry; continue; }

          // NOTE: Check the sign
          if (left[i] & 0x80 != 0) break;

          // NOTE: Create 'singed' value
          left.insert(left.begin(), carry); 
          break;
        }

        return left;

      case TokenT::BIT_OR:
        for (int32_t i = 0; i < left.size(); i++)  left[i] |= right[i];
        return left;

      case TokenT::BIT_AND:
        for (int32_t i = 0; i < left.size(); i++)  left[i] &= right[i];
        return left;

      case TokenT::BIT_XOR:
        for (int32_t i = 0; i < left.size(); i++)  left[i] ^= right[i];
        return left;


    }

    error(expr->operation, "Unknown operation.");
    return {};
  }

  MemoryT visitExprUnary(ExpressionUnary* expr) override {
    MemoryT right = evaluate(expr->right.get());

    switch (expr->operation->token) {
      case TokenT::MINUS:
        for (int32_t i = right.size(); i >= 0; i--) {
          uint16_t acc = ~right[i] + 1;
          uint8_t carry = (acc & 0xFF00) >> 8;

          right[i] = acc & 0xFF;
          if (!carry) continue;

          if (i) { right[i - 1] += carry; continue; }

          // NOTE: Check the sign
          if (right[i] & 0x80 != 0) break;

          // NOTE: Create 'singed' value
          right.insert(right.begin(), carry); 
          break;

        }
        return right;

      case TokenT::BIT_NOT:
        for (auto& val : right) val = val ^ 0xFF;
        return right;
    }

    error(expr->operation, "Unknown operation.");
    return {};
  }

  MemoryT visitExprVariable(ExpressionVariable* expr) override {
    auto bytes = env.get(expr->token->lexeme, expr->size);
    if (bytes.size() == expr->size) return bytes;

    error(expr->token, "Variable byte size exceeded allowed size.");
    return {};
  }

  MemoryT visitStmtAddress(StatementAddress* stmt) override {
    MemoryT expr = evaluate(stmt->expr.get());
    if (expr.size() > 2) {
      error(stmt->label, "Address byte size exceeded allowed size.");
      return {};
    }

    env.allocate(stmt->label->lexeme, expr);
    return {};
  }

  MemoryT visitStmtAllocate(StatementAllocate* stmt) override {
    MemoryT res;

    for (auto& expr : stmt->data) {
      MemoryT bytes = evaluate(expr.get());
      res.insert(res.end(), bytes.begin(), bytes.end());
    }

    return res;
  }

  MemoryT visitStmtLambda(StatementLambda* stmt) override {
    std::vector<uint32_t> argv;
    for (auto& expr : stmt->expr) {
      for (auto byte : evaluate(expr.get())) {
        argv.push_back((uint32_t)byte);
      }
    }

    argv.insert(argv.end(), stmt->argv.begin(), stmt->argv.end());
    auto opcode = stmt->lambda(argv);
    if (opcode == 0x00) {
      error(nullptr, "Invalid lambda operation.");
      return {};
    }

    env.bind(stmt->token);
    return Int2Bytes(opcode);
  }

  MemoryT visitStmtNoArg(StatementNoArgCommand* stmt) override {
    env.bind(stmt->token);
    return Int2Bytes(stmt->opcode);
  }

  MemoryT visitStmtOneArg(StatementOneArgCommand* stmt) override {
    MemoryT expr = evaluate(stmt->expr.get());
    MemoryT result = Int2Bytes(stmt->opcode);

    result.insert(result.end(), expr.rbegin(), expr.rend());

    if (stmt->opcode == 0x01) {
      for (auto t : expr) printf("%02x ", t);
      printf(" size = %d\n", ((ExpressionLiteral *)stmt->expr.get())->size);
    }

    env.bind(stmt->token);
    return result;
  }

  MemoryT visitStmtVariable(StatementVariable* stmt) override {
    switch (stmt->type) {
      case StatementVariable::TypeT::ADDRESS:
        env.define(stmt->label->lexeme, Int2Bytes(env.addr));
        return {};

      case StatementVariable::TypeT::DEFINITION: 
        env.define(stmt->label->lexeme, evaluate(stmt->definition.get()));
        return {};
    }

    error(nullptr, "Invalid variable.");
    return {};
  }

private:
  inline MemoryT evaluate(Expression* expr) {
    return expr->accept(this);
  }

  inline MemoryT Int2Bytes(uint32_t val, int32_t size = 0) {
    if (val == 0x00) return MemoryT(size ? size : 1, 0x00);

    MemoryT result;
    std::array<uint8_t, 4> bytes = {
      (uint8_t)((val >> 24) & 0xFF),
      (uint8_t)((val >> 16) & 0xFF),
      (uint8_t)((val >>  8) & 0xFF),
      (uint8_t)((val >>  0) & 0xFF),
    };


    for (int32_t i = bytes.size() - size; size && i < bytes.size(); i++) {
      result.push_back(bytes[i]);
    }

    for (int32_t i = 0; !size && i < bytes.size(); i++) {
      if (bytes[i] == 0x00 && !result.size()) continue;
      result.push_back(bytes[i]);
    }

    return result;
  }

  void error(std::shared_ptr<Token> token, std::string message) {
    std::stringstream ss;

    if (token == nullptr) ss << "[Ln 1] Error: " << message << "\n";
    else ss << "[Ln " << token->line << " Col " << token->col << "] Error at '" << token->lexeme << "': " << message << "\n";

    errors.push_back(ss.str());
  }

public:
  Environment env;
  Parser parser;

  std::vector<std::string> errors;
};

};
