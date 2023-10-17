#pragma once
#include "Environment.h"

namespace Interpreter {

class Interpreter : public Visitor {
public:

  Interpreter(uint32_t a = 0): parser(Parser()), env(a) {}
  Interpreter(Parser p): parser(p) {}

  bool Load(std::string path) {
    filedir = std::filesystem::path(path).remove_filename();

    filepath = std::filesystem::canonical(path);
    filename = std::filesystem::path(path).filename();

    std::ifstream f(path); std::stringstream buf;
    buf << f.rdbuf(); f.close();

    return scan((buf.str()));
  }

  bool scan(std::string src) {
    errors.clear();

    if (parser.scan(src)) { 
      for (auto& err : parser.errors) errors.push_back("File '" + filename.string() + "' " + err);
      return true;
    }

    // Linker
    env.reset(); env.unlock();
    for (auto& stmt : parser.stmt) env.insert(evaluate(stmt.get()));

    if (env.unknown.size()) {
      for (auto& var : env.unknown) {
        error(nullptr, "Unknown variable '" + var + "'.");
      }

      return errors.size();
    }

    // Interpreter
    env.reset(); env.lock();
    for (auto& stmt : parser.stmt) env.insert(evaluate(stmt.get()));

    return errors.size();
  }

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
        for (int32_t i = left.size() - 1; i >= 0; i--) {
          uint16_t acc = left[i] + right[i];
          uint8_t carry = (acc & 0xFF00) >> 8;

          left[i] = acc & 0xFF;
          if (!carry) continue;

          if (!i) { left.insert(left.begin(), carry); break; }

          left[i - 1] += carry;
        }

        return left;

      case TokenT::MINUS:
        for (int32_t i = left.size() - 1; i >= 0; i--) {
          uint16_t acc = (uint16_t)left[i] + ((uint16_t)right[i] ^ 0xFFFF) + 1u;
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

      case TokenT::RIGHT_SHIFT: 
        for (int32_t i = 0; i < Bytes2Int(right); i++) {
          for (int32_t j = 0, carry = 0; j < left.size(); j++) {
            const auto prev = left[i] & 0x01;
            left[i] = ((left[i] >> 1) & 0xFF) | (carry ? 0x80 : 0x00);
            carry = prev;
          }
        }

        return left;

      case TokenT::LEFT_SHIFT:
        for (int32_t i = 0; i < Bytes2Int(right); i++) {
          for (int32_t j = left.size() - 1, carry = 0; j >= 0; j--) {
            const auto prev = left[i] & 0x80;
            left[i] = ((left[i] << 1) & 0xFF) | (carry ? 0x01 : 0x00);
            carry = prev;
          }
        }

        return left;

      case TokenT::BIT_OR:
        for (int32_t i = 0; i < left.size(); i++)  left[i] = left[i] | right[i];
        return left;

      case TokenT::BIT_AND:
        for (int32_t i = 0; i < left.size(); i++)  left[i] = left[i] & right[i];
        return left;

      case TokenT::BIT_XOR:
        for (int32_t i = 0; i < left.size(); i++)  left[i] = left[i] ^ right[i];
        return left;


    }

    error(expr->operation, "Unknown operation.");
    return {};
  }

  MemoryT visitExprUnary(ExpressionUnary* expr) override {
    MemoryT right = evaluate(expr->right.get());

    switch (expr->operation->token) {
      case TokenT::MINUS:
        for (int32_t i = right.size() - 1; i >= 0; i--) {
          uint16_t acc = ((uint16_t)right[i] ^ 0xFFFF) + 1;
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
    if (!expr->size || bytes.size() == expr->size) return bytes;

    error(expr->token, "Variable byte size exceeded allowed size.");
    return {};
  }

  MemoryT visitStmtAddress(StatementAddress* stmt) override {
    MemoryT expr = evaluate(stmt->expr.get());
    if (expr.size() > 2) {
      error(stmt->label, "Address byte size exceeded allowed size.");
      return {};
    }

    env.allocate(expr);
    return {};
  }

  MemoryT visitStmtAllocate(StatementAllocate* stmt) override {
    MemoryT res;

    for (auto& expr : stmt->data) {
      MemoryT bytes = evaluate(expr.get()); const auto nSize = (int32_t)bytes.size() / 2;
      for (int32_t i = 1; i < stmt->size - nSize; i++) bytes.insert(bytes.begin(), 0);

      // TODO: Bellow is quick fix, need to think about how impl this better
      // Right now `dw "key"` works incorrectly
      if (stmt->reverse) res.insert(res.end(), bytes.rbegin(), bytes.rend());
      else res.insert(res.end(), bytes.begin(), bytes.end());
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

    env.bind(filepath, stmt->token);
    return Int2Bytes(opcode);
  }

  MemoryT visitStmtNoArg(StatementNoArgCommand* stmt) override {
    env.bind(filepath, stmt->token);
    return Int2Bytes(stmt->opcode);
  }

  MemoryT visitStmtOneArg(StatementOneArgCommand* stmt) override {
    MemoryT expr = evaluate(stmt->expr.get());
    MemoryT result = Int2Bytes(stmt->opcode);

    result.insert(result.end(), expr.rbegin(), expr.rend());

    env.bind(filepath, stmt->token);
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

  MemoryT visitStmtInclude(StatementInclude * stmt) override { 
    auto f = filedir / std::filesystem::path(stmt->expr->token->literal);
    if (!std::filesystem::exists(f)) {
      error(stmt->expr->token, "Include file doesn't exist.");
      return MemoryT();
    }

    Interpreter included = Interpreter(env.addr); included.Load(f);

    errors.insert(errors.end(), included.errors.begin(), included.errors.end());
    if (included.errors.size()) return MemoryT();

    for (auto& t : included.env.tokens) env.tokens[t.first] = t.second;
    for (auto& v : included.env.vars.second) env.define(v.first, v.second);
    included.env.memory.erase(included.env.memory.begin(), included.env.memory.begin() + env.addr);

    return included.env.memory;
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

  inline uint32_t Bytes2Int(MemoryT& bytes) {
    uint32_t acc = 0;
    for (auto& byte : bytes) acc = (acc << 8) | byte;

    return acc;
  }

  void error(std::shared_ptr<Token> token, std::string message) {
    std::stringstream ss; ss << "File '" << filename.string() << "' ";

    if (token == nullptr) ss << "[Ln 1] Error: " << message << "\n";
    else ss << "[Ln " << token->line << " Col " << token->col << "] Error at '" << token->lexeme << "': " << message << "\n";

    errors.push_back(ss.str());
  }

public:
  Environment env;
  Parser parser;

  std::filesystem::path filedir;
  std::filesystem::path filepath;
  std::filesystem::path filename;

  std::vector<std::string> errors;
};

};
