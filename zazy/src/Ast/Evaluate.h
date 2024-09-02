#pragma once
#include "Environment.h"

namespace Zazy {

class Evaluate : public Visitor {
public:

  Evaluate(Parser p):
    parser(p), env(std::make_unique<Environment>(nullptr)) {}

  Evaluate(Parser p, env_t env):
    parser(p), env(std::move(env)) {}

  // void scan(std::shared_ptr<Expression> expr) {
  //   char* src = (char*)evaluate(expr);
  //   if (src == nullptr) return (void)printf("NULLPTR\n");

  //   printf("[REVERSE] %s\n", src); free(src);
  // }

  obj_t next() {
    auto ast = parser.next();
    if (ast == nullptr) return nullptr;

    return evaluate(ast);
  }

  inline auto move() { return std::move(env); }
  inline bool isAtEnd() { return parser.isAtEnd(); }
  
public:

  // virtual obj_t visitDeclStruct(Decl::Struct* decl) { return nullptr; }
  // virtual obj_t visitDeclEnum(Decl::Enum* decl) { return nullptr; }
  // virtual obj_t visitDeclFunc(Decl::Func* decl) { return nullptr; }

  obj_t visitDeclVar(Decl::Var* decl) override {
    auto value = decl->value == nullptr ? null() : evaluate(decl->value);
    if (isError(value)) return value;
    if (env->has((std::string&)decl->name->lexeme)) return error(std::string("Can't redeclare variable '") + decl->name->lexeme.c_str() + "'.");

    env->set(decl->name->lexeme, decl->type, cast(decl->type, value));
    return null();
  }

  obj_t visitStmtBlock(Stmt::Block* stmt) override { 
    obj_t result = null();
    env = std::make_unique<Environment>(std::move(env));

    for (auto& s : stmt->stmt) {
      auto res = evaluate(s);
      if (isReturn(res) || isError(res)) { result = res; break; }
    }

    env = env->move();
    return result;
  }

  obj_t visitStmtExpr(Stmt::Expr* stmt) override { 
    return evaluate(stmt->expr);
  }

  virtual obj_t visitStmtIf(Stmt::If* stmt) {
    auto condition = evaluate(stmt->condition);
    if (isError(condition)) return condition;

    if (condition->value) return evaluate(stmt->then);
    if (stmt->otherwise != nullptr) return evaluate(stmt->otherwise);

    return null();
  }

  // virtual obj_t visitStmtFor(Stmt::For* stmt) { return nullptr; }
  // // virtual obj_t visitStmt(Stmt::If* stmt) { return nullptr; }
  // virtual obj_t visitStmtUntil(Stmt::Until* stmt) { return nullptr; }
  // virtual obj_t visitStmtWhile(Stmt::While* stmt) { return nullptr; }

  obj_t visitStmtReturn(Stmt::Return* stmt) override {
    return std::make_shared<Obj::Return>(stmt->expr == nullptr ? null() : evaluate(stmt->expr));
  }


  // virtual obj_t visitExprArray(Expr::Array* expr) { return nullptr; }
  obj_t visitExprAssign(Expr::Assign* expr) override {
    // if (env->has((std::string&)expr->name->lexeme)) return error(std::string("Can't redeclare variable '") + decl->name->lexeme.c_str() + "'.");

    // TODO: ????????????
    // env->set(decl->name->lexeme, cast(decl->type, value));
    return error("NNNNNNNNNNN");
    // return nullptr;
  }

  obj_t visitExprBinary(Expr::Binary* expr) override {
    auto left = evaluate(expr->left);
    auto right = evaluate(expr->right);

    if (isError(left)) return left;
    if (isError(right)) return right;

    switch (expr->operation->token) {
      case TokenT::OP_PLUS:
        return number(left, right, left->value + right->value);

      case TokenT::OP_MINUS:
        return number(left, right, left->value - right->value);

      case TokenT::STAR:
        return number(left, right, left->value * right->value);

      case TokenT::OP_DIV:
        return number(left, right, left->value / right->value);

      case TokenT::OP_MOD:
        return number(left, right, left->value % right->value);

      case TokenT::AMPERSAND:
        return number(left, right, left->value & right->value);

      case TokenT::BIT_OR:
        return number(left, right, left->value | right->value);

      case TokenT::BIT_XOR:
        return number(left, right, left->value ^ right->value);

      case TokenT::BIT_RIGHT_SHIFT:
        return number(left, right, left->value >> right->value);

      case TokenT::BIT_LEFT_SHIFT:
        return number(left, right, left->value << right->value);

      case TokenT::OP_EQUAL:
        return boolean(left->value == right->value);

      case TokenT::OP_NOT_EQUAL:
        return boolean(left->value != right->value);

      case TokenT::OP_MORE:
        return boolean(left->value > right->value);

      case TokenT::OP_MORE_OR_EQUAL:
        return boolean(left->value >= right->value);

      case TokenT::OP_LESS:
        return boolean(left->value < right->value);

      case TokenT::OP_LESS_OR_EQUAL:
        return boolean(left->value <= right->value);

      case TokenT::OP_AND:
        return boolean(left->value && right->value);

      case TokenT::OP_OR:
        return boolean(left->value || right->value);
    }

    return error("Unknown binary operation.");
  }

  obj_t visitExprLiteral(Expr::Literal* expr) override {
    switch (expr->token->token) {
      case TokenT::CHAR:
        return std::make_shared<Obj::Char>((uint8_t)std::stoul(expr->token->literal));

      case TokenT::SHORT:
        return std::make_shared<Obj::Short>((uint16_t)std::stoul(expr->token->literal));

      case TokenT::INT:
        return std::make_shared<Obj::Int>((uint32_t)std::stoul(expr->token->literal));

      case TokenT::STAR:
        return std::make_shared<Obj::Ptr>((uint16_t)std::stoul(expr->token->literal));

      case TokenT::VOID:
        return std::make_shared<Obj::Void>();
    }
    
    return error("Unknown literal.");
  }

  obj_t visitExprCast(Expr::Cast* expr) override {
    auto right = evaluate(expr->right);
    if (isError(right)) return right;
    return cast(expr->type, right);
  }

  obj_t visitExprVar(Expr::Var* expr) override {
    auto value = env->get(expr->name->lexeme);
    if (value != nullptr) return value;
    return error("Unknown variable '" + expr->name->lexeme + "'.");
  }

  obj_t visitExprGroup(Expr::Group* expr) override { 
    return evaluate(expr->expr);
  }

  // virtual obj_t visitExprGet(Expr::Get* expr) { return nullptr; }
  // virtual obj_t visitExprInvoke(Expr::Invoke* expr) { return nullptr; }
  // virtual obj_t visitExprSuffix(Expr::Suffix* expr) { return nullptr; }
  // virtual obj_t visitExprTernary(Expr::Ternary* expr) { return nullptr; }
  // virtual obj_t visitExprUnary(Expr::Unary* expr) { return nullptr; }


private:
  inline obj_t evaluate(std::shared_ptr<Expression> expr) {
    return expr->accept(this);
  }

  inline obj_t evaluate(std::shared_ptr<Statement> stmt) {
    return stmt->accept(this);
  }


private:
  inline obj_t null() {
    return std::make_shared<Obj::Void>();
  }

  inline obj_t boolean(uint32_t value) {
    return std::make_shared<Obj::Char>((uint8_t)(value));
  }

  inline obj_t number(obj_t left, obj_t right, uint32_t value) {
    auto max = left->type > right->type ? left->type : right->type;

    switch (max) {
      case Obj::TypeT::CHAR:
        return std::make_shared<Obj::Char>((uint8_t)value);

      case Obj::TypeT::SHORT:
        return std::make_shared<Obj::Short>((uint16_t)value);

      case Obj::TypeT::INT:
        return std::make_shared<Obj::Int>(value);

      case Obj::TypeT::PTR:
        return std::make_shared<Obj::Ptr>((uint16_t)value);

      case Obj::TypeT::VOID:
        return std::make_shared<Obj::Void>();
    }

    return error("Unknown number type.");
  }

  inline obj_t cast(token_t& type, obj_t& value) {
    switch (type->token) {
      case TokenT::W_CHAR:
        return std::make_shared<Obj::Char>((uint8_t)value->value);

      case TokenT::W_SHORT:
        return std::make_shared<Obj::Short>((uint16_t)value->value);

      case TokenT::W_INT:
        return std::make_shared<Obj::Int>((uint32_t)value->value);

      case TokenT::STAR:
        return std::make_shared<Obj::Ptr>((uint16_t)value->value);

      case TokenT::W_VOID:
        return std::make_shared<Obj::Void>();

      case TokenT::W_AUTO:
        if (value->value >= 0 && value->value <= UINT8_MAX)  return std::make_shared<Obj::Char>((uint8_t)value->value);
        if (value->value >= 0 && value->value <= UINT16_MAX) return std::make_shared<Obj::Short>((uint16_t)value->value);
        if (value->value >= 0 && value->value <= UINT32_MAX) return std::make_shared<Obj::Int>((uint32_t)value->value);

        return error("Number value outside of boundaries.");
    }

    return error("Unknown cast type.");
  }


  inline obj_t error(std::string msg) {
    return std::make_shared<Obj::Error>(msg);
  }

  inline bool isError(obj_t& obj) { return obj->type == Obj::ERROR; }
  inline bool isReturn(obj_t& obj) { return obj->type == Obj::RETURN; }

private:
  env_t env;

  Parser parser;
};
};
