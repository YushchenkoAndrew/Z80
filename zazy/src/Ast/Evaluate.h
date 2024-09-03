#pragma once
#include "Environment.h"

namespace Zazy {

/**
 * This code was hardly expired by the book {Writing an Interpreter in Go}
 * Check out this link for more info: https://interpreterbook.com/
 *
 * Build in functions:
 *   void* malloc(int size)                - Allocates size bytes of uninitialized storage
 *   void* realloc(void* ptr, int size)    - Reallocates the given area of memory.
 *   void  free(void* ptr)                 - Deallocates the space previously allocated by malloc()
 *   void  asm(void* ptr)                  - Gives the ability to embed assembly language source code 
 * 
 */
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
    for (auto& err : parser.errors) printf("%s\n", err.c_str());

    return evaluate(ast);
  }

  inline auto move() { return std::move(env); }
  inline bool isAtEnd() { return parser.isAtEnd(); }
  
public:

  // virtual obj_t visitDeclStruct(Decl::Struct* decl) { return nullptr; }
  // virtual obj_t visitDeclEnum(Decl::Enum* decl) { return nullptr; }

  obj_t visitDeclFunc(Decl::Func* decl) override {
    auto func = env->get(decl->name->lexeme);
    if (func != nullptr) {
      if (!isFunc(func)) return error(std::string("Can't redeclare variable '")+ decl->name->lexeme.c_str() + "' to be a function.");
      if (std::static_pointer_cast<Obj::Func>(func)->body != nullptr) return error(std::string("Can't redeclare function '") + decl->name->lexeme.c_str() + "'.");
    }

    env->set(decl->name->lexeme, std::make_shared<Obj::Func>(decl->type, decl->params, decl->body));
    return null();
  }

  obj_t visitDeclVar(Decl::Var* decl) override {
    auto value = decl->value == nullptr ? null() : evaluate(decl->value);
    if (isError(value)) return value;
    if (env->has((std::string&)decl->name->lexeme)) return error(std::string("Can't redeclare variable '") + decl->name->lexeme.c_str() + "'.");

    env->set(decl->name->lexeme, cast(decl->type, value));
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

  obj_t visitStmtIf(Stmt::If* stmt) override {
    auto condition = evaluate(stmt->condition);
    if (isError(condition)) return condition;

    if (condition->value) return evaluate(stmt->then);
    if (stmt->otherwise != nullptr) return evaluate(stmt->otherwise);

    return null();
  }

  obj_t visitStmtFor(Stmt::For* stmt) override {
    obj_t err = null(), body = null();

    env = std::make_unique<Environment>(std::move(env));
    if (stmt->init != nullptr) err = evaluate(stmt->init);

    while (true) {
      obj_t condition = stmt->condition == nullptr ? nullptr : evaluate(stmt->condition); 
      if (condition != nullptr && !condition->value) break;

      body = stmt->body == nullptr ? null() : evaluate(stmt->body);
      if (isReturn(body)) break;


      auto next = stmt->next == nullptr ? null() : evaluate(stmt->next);
      err = isError(next) ? next : isError(body) ? body : condition != nullptr && isError(condition) ? condition : err;

      if (isError(err)) break;
    }

    env = env->move();
    if (isError(err)) return err;
    return body;
  }

  // // virtual obj_t visitStmt(Stmt::If* stmt) { return nullptr; }
  // virtual obj_t visitStmtUntil(Stmt::Until* stmt) { return nullptr; }
  // virtual obj_t visitStmtWhile(Stmt::While* stmt) { return nullptr; }

  obj_t visitStmtReturn(Stmt::Return* stmt) override {
    return std::make_shared<Obj::Return>(stmt->expr == nullptr ? null() : evaluate(stmt->expr));
  }


  // virtual obj_t visitExprArray(Expr::Array* expr) { return nullptr; }
  obj_t visitExprAssign(Expr::Assign* expr) override {
    auto var = evaluate(expr->left);
    auto value = evaluate(expr->right);

    if (isError(var)) return var;
    if (isError(value)) return value;

    return (var->value = cast(var->type, value->value)->value), var;
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

  obj_t visitExprUnary(Expr::Unary* expr) override {
    auto right = evaluate(expr->right);
    if (isError(right)) return right;

//  *  unary       -> ( '+' | '-' | '*' | '&' | '!' | '~' ) unary | cast 
    switch (expr->operation->token) {
      // TODO: ????????????
      // case TokenT::STAR:
      // case TokenT::AMPERSAND:

      case TokenT::OP_PLUS:
        return cast(right->type, right->value);

      case TokenT::OP_MINUS:
        return cast(right->type, ~right->value + 1);

      case TokenT::OP_NOT:
        return cast(Obj::CHAR, !right->value);

      case TokenT::BIT_NOT:
        return cast(right->type, ~right->value);
    }

    return error("Unknown unary operation.");
  }

  obj_t visitExprSuffix(Expr::Suffix* expr) override {
    auto left = evaluate(expr->left);
    if (isError(left)) return left;

    switch (expr->operation->token) {
      case TokenT::OP_INC:
        return cast(left->type, left->value++);

      case TokenT::OP_DEC:
        return cast(left->type, left->value--);
    }

    return error("Unknown suffix operation.");
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
    

    printf("%d %s\n", expr->token->token, expr->token->lexeme.c_str());
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

  obj_t visitExprTernary(Expr::Ternary* expr) override {
    auto condition = evaluate(expr->condition);
    if (isError(condition)) return condition;

    if (condition->value) return evaluate(expr->then);
    return evaluate(expr->otherwise);
  }

  obj_t visitExprInvoke(Expr::Invoke* expr) override {
    auto ref = evaluate(expr->left);

    if (isError(ref)) return ref;
    if (!isFunc(ref)) return error("Reference '" + ref->string() + "' is not callable.");

    auto func = std::static_pointer_cast<Obj::Func>(ref);
    if (func->body == nullptr) return error("Function '" + ref->string() + "' is declared but not defined.");
    if (func->params.size() != expr->args.size()) return error("Incorrect amount of args, expects " + func->string() + ".");


    std::vector<obj_t> args; obj_t result = nullptr;
    env = std::make_unique<Environment>(std::move(env));

    for (int32_t i = 0; i < func->params.size(); i++) {
      auto value = evaluate(expr->args[i]);
      if (isError(value)) { result = value; break; }

      auto param = std::static_pointer_cast<Decl::Var>(func->params[i]);
      if (env->has((std::string&)param->name->lexeme)) { result = error(std::string("Can't redeclare variable '") + param->name->lexeme.c_str() + "'."); break; }

      env->set(param->name->lexeme, cast(param->type, value));
    }

    result = result == nullptr ? evaluate(std::static_pointer_cast<Obj::Func>(func)->body) : result;
    env = env->move();

    if (isError(result)) return result;

    auto value = isReturn(result) ? std::static_pointer_cast<Obj::Return>(result)->obj : null();
    return cast(std::static_pointer_cast<Obj::Func>(func)->type, value);
  }

  // virtual obj_t visitExprGet(Expr::Get* expr) { return nullptr; }


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
    return cast(max, value);
  }

  inline obj_t cast(Obj::TypeT type, uint32_t value) {
    switch (type) {
      case Obj::TypeT::CHAR:
        return std::make_shared<Obj::Char>((uint8_t)value);

      case Obj::TypeT::SHORT:
        return std::make_shared<Obj::Short>((uint16_t)value);

      case Obj::TypeT::INT:
        return std::make_shared<Obj::Int>((uint32_t)value);

      case Obj::TypeT::PTR:
        return std::make_shared<Obj::Ptr>((uint16_t)value);

      case Obj::TypeT::VOID:
        return std::make_shared<Obj::Void>();
    }

    return error("Unknown cast type.");
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

  inline bool isFunc(obj_t& obj) { return obj->type == Obj::FUNC; }
  inline bool isError(obj_t& obj) { return obj->type == Obj::ERROR; }
  inline bool isReturn(obj_t& obj) { return obj->type == Obj::RETURN; }

private:
  env_t env;

  Parser parser;
};
};
