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
    // TODO: Throw error
  }

  MemoryT visit(Int2Type<EXPR_LITERAL>, ExpressionLiteral* expr) {
    switch (expr->token->token) {
      case TokenT::NUMBER: {
        uint32_t num = std::stoul(expr->token->lexeme);
        if (expr->size && num & ~(0xFF << (expr->size * 8)) != 0) {
          // TODO: Throw error
        }

        return Int2Bytes(num);
      }

      case TokenT::STRING: {
        if (expr->size && expr->token->lexeme.size() != expr->size) {
          // TODO: Throw error
        }

        MemoryT bytes;
        for (auto c : expr->token->lexeme) bytes.push_back((uint8_t)c);
        return bytes;
      }
    }

    // TODO: Throw error
    return {};
  }

  MemoryT visit(Int2Type<EXPR_BINARY>, ExpressionBinary* expr) {
    MemoryT right = evaluate(&expr->right);
    MemoryT left = evaluate(&expr->left);

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

    // TODO: Throw error
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

    // TODO: Throw error
    return {};
  }

  MemoryT visit(Int2Type<EXPR_VARIABLE>, ExpressionVariable* expr) {
    // TODO:
  }

  MemoryT visit(Int2Type<STMT_ADDRESS>, StatementAddress* stmt) {
    // TODO:
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
    if (opcode == 0x00) {
      // TODO: Throw error
    }

    return Int2Bytes(opcode);
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
    // TODO:
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

public:
  Environment env;
  Parser parser;
};

};
