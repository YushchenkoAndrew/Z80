#pragma once
#include "Environment.h"

namespace Interpreter {

class Interpreter : public Visitor<MemoryT> {
public:

  Interpreter(): parser(Parser()) {}
  Interpreter(Parser p): parser(p) {}

  bool scan(std::string src) {
    reset();

    if (parser.scan(src)) { 
      errors.insert(errors.end(), parser.errors.begin(), parser.errors.end());
      return true;
    }

    for (auto stmt : parser.stmt) {
      env.insert(evaluate(&stmt));
    }

    if (env.unknown.size()) {
      for (auto& pair : env.unknown) {
        std::stringstream ss;
        ss << "Unknown variable '" << pair.first << "'.";

        error(nullptr, ss.str());
      }
    }

    return errors.size();
  }

  inline void reset() { errors.clear(); env.reset(); }

  template <int32_t U>
  MemoryT visit(Int2Type<U>, Expression* expr) {
    error(nullptr, "Unknown operation.");
    return {};
  }

  MemoryT visit(Int2Type<EXPR_LITERAL>, ExpressionLiteral* expr) {
    switch (expr->token->token) {
      case TokenT::NUMBER: {
        uint32_t num = std::stoul(expr->token->lexeme);
        if (expr->size && num & ~(0xFF << (expr->size * 8)) != 0) {
          error(expr->token, "Number byte size exceeded allowed size.");
          return {};
        }

        return Int2Bytes(num);
      }

      case TokenT::STRING: {
        if (expr->size && expr->token->lexeme.size() != expr->size) {
          error(expr->token, "String byte size exceeded allowed size.");
          return {};
        }

        MemoryT bytes;
        for (auto c : expr->token->lexeme) bytes.push_back((uint8_t)c);
        return bytes;
      }
    }

    error(expr->token, "Unknown literal.");
    return {};
  }

  MemoryT visit(Int2Type<EXPR_BINARY>, ExpressionBinary* expr) {
    MemoryT left = evaluate(&expr->left);
    MemoryT right = evaluate(&expr->right);

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

  MemoryT visit(Int2Type<EXPR_UNARY>, ExpressionUnary* expr) {
    MemoryT right = evaluate(&expr->right);

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

  MemoryT visit(Int2Type<EXPR_VARIABLE>, ExpressionVariable* expr) {
    return env.get(expr->token->lexeme);
  }

  MemoryT visit(Int2Type<STMT_ADDRESS>, StatementAddress* stmt) {
    MemoryT expr = evaluate(&stmt->expr);
    if (expr.size() > 2) {
      error(stmt->label, "Address byte size exceeded allowed size.");
      return {};
    }

    env.allocate(stmt->label->lexeme, expr);
    return {};
  }

  MemoryT visit(Int2Type<STMT_ALLOCATE>, StatementAllocate* stmt) {
    MemoryT res;

    for (auto expr : stmt->data) {
      MemoryT bytes = evaluate(&expr);
      res.insert(res.end(), bytes.begin(), bytes.end());
    }

    return res;
  }

  MemoryT visit(Int2Type<STMT_LAMBDA>, StatementLambda* stmt) {
    std::vector<uint32_t> argv;
    for (auto expr : stmt->expr) {
      for (auto byte : evaluate(&expr)) {
        argv.push_back((uint32_t)byte);
      }
    }

    argv.insert(argv.end(), stmt->argv.begin(), stmt->argv.end());
    auto opcode = stmt->lambda(argv);
    if (opcode != 0x00) return Int2Bytes(opcode);

    error(nullptr, "Invalid lambda operation.");
    return {};
  }

  MemoryT visit(Int2Type<STMT_NO_ARG>, StatementNoArgCommand* stmt) {
    return Int2Bytes(stmt->opcode);
  }

  MemoryT visit(Int2Type<STMT_ONE_ARG>, StatementOneArgCommand* stmt) {
    MemoryT expr = evaluate(&stmt->expr);
    MemoryT result = Int2Bytes(stmt->opcode);

    result.insert(result.end(), expr.begin(), expr.end());
    return result;
  }

  MemoryT visit(Int2Type<STMT_VARIABLE>, StatementVariable* stmt) {
    switch (stmt->type) {
      case StatementVariable::TypeT::ADDRESS:
        env.define(stmt->label->lexeme, Int2Bytes(env.addr));
        return {};

      case StatementVariable::TypeT::DEFINITION: 
        env.define(stmt->label->lexeme, evaluate(&stmt->definition));
        return {};
    }

    error(nullptr, "Invalid variable.");
    return {};
  }

private:
  inline MemoryT evaluate(Expression* expr) {
    return expr->accept(this);
  }

  inline MemoryT Int2Bytes(uint32_t val) {
    if (val == 0x00) return { 0x00 };

    std::array<uint8_t, 4> bytes = {
      (uint8_t)((val >> 24) & 0xFF),
      (uint8_t)((val >> 16) & 0xFF),
      (uint8_t)((val >>  8) & 0xFF),
      (uint8_t)((val >>  0) & 0xFF),
    };

    MemoryT result;
    for (auto byte : bytes) {
      if (byte == 0x00 && !result.size()) continue;
      result.push_back(byte);
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
