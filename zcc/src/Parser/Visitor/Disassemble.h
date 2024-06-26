#pragma once
#include "src/Parser/Expression/Var.h"


namespace Zcc {

class Disassemble : public Visitor {
public:

  void scan(std::shared_ptr<Expression> expr) {
    char* src = (char*)evaluate(expr);
    if (src == nullptr) return (void)printf("NULLPTR\n");

    printf("[REVERSE] %s\n", src); free(src);
  }
  
public:
  void* visitExprArray(Expr::Array* expr) override {
    // TODO:
    return nullptr;
  }

  void* visitExprAssign(Expr::Assign* expr) override {
    char* left = (char*)evaluate(expr->left);
    char* right = (char*)evaluate(expr->right);
    if (left == nullptr || right == nullptr) return nullptr;

    void* ptr = malloc((strlen(left) + strlen(right) + 2) * sizeof(char));
    if (ptr == nullptr) return nullptr;

    ((char*)ptr)[0] = '\0'; 
    strcat((char*)ptr, left); strcat((char*)ptr, "="); strcat((char*)ptr, right);

    free(left); free(right);
    return ptr;
  }

  void* visitExprBinary(Expr::Binary* expr) override {
    char* left = (char*)evaluate(expr->left);
    char* right = (char*)evaluate(expr->right);
    if (left == nullptr || right == nullptr) return nullptr;

    void* ptr = malloc((strlen(left) + strlen(right) + expr->operation->lexeme.size() + 1) * sizeof(char));
    if (ptr == nullptr) return nullptr;


    ((char*)ptr)[0] = '\0';
    strcat((char*)ptr, left); strcat((char*)ptr, expr->operation->lexeme.c_str()); strcat((char*)ptr, right);

    free(left); free(right);
    return ptr;
  }

  void* visitExprCast(Expr::Cast* expr) override {
    char* type = (char*)evaluate(expr->type);
    char* right = (char*)evaluate(expr->expr);
    if (type == nullptr || expr == nullptr) return nullptr;

    void* ptr = malloc((strlen(type) + strlen(right) + 3) * sizeof(char));

    ((char*)ptr)[0] = '\0'; 
    strcat((char*)ptr, "("); strcat((char*)ptr, type); strcat((char*)ptr, ")"); strcat((char*)ptr, right);

    free(type); free(right);
    return ptr;
  }

  void* visitExprGet(Expr::Get* expr) override { return nullptr; }

  void* visitExprGroup(Expr::Group* expr) override {
    char* left = (char*)evaluate(expr->expr);
    if (left == nullptr) return nullptr;

    void* ptr = malloc((strlen(left) + 3) * sizeof(char));
    if (ptr == nullptr) return nullptr;

    ((char*)ptr)[0] = '\0'; 
    strcat((char*)ptr, "("); strcat((char*)ptr, left); strcat((char*)ptr, ")");

    free(left);
    return ptr;
  }

  void* visitExprInvoke(Expr::Invoke* expr) override { return nullptr; }

  void* visitExprLiteral(Expr::Literal* expr) override {
    void* ptr = malloc((expr->token->literal.size() + 1) * sizeof(char));
    if (ptr == nullptr) return nullptr;

    ((char*)ptr)[0] = '\0';
    strcpy((char*)ptr, expr->token->literal.c_str());

    return ptr;
  }

  void* visitExprSuffix(Expr::Suffix* expr) override { return nullptr; }

  void* visitExprTernary(Expr::Ternary* expr) override { return nullptr; }

  void* visitExprType(Expr::Type* expr) override {
    auto make = [](const char* str) {
      void* ptr = malloc((strlen(str) + 1) * sizeof(char));
      if (ptr == nullptr) return (void*)nullptr;
      
      ((char*)ptr)[0] = '\0';
      strcpy((char*)ptr, str);
      return ptr;
    };

    switch (expr->type->token) {
      case TokenT::STAR: return make("*");
      case TokenT::CHAR: return make("char");
      case TokenT::SHORT: return make("short");
      case TokenT::INT: return make("int");
      case TokenT::UNSIGNED_CHAR: return make("unsigned char");
      case TokenT::UNSIGNED_SHORT: return make("unsigned short");
      case TokenT::UNSIGNED_INT: return make("unsigned int");
    }

    return nullptr;
  }

  void* visitExprUnary(Expr::Unary* expr) override {
    char* right = (char *)evaluate(expr->right);
    if (right == nullptr) return nullptr;

    void* ptr = malloc((strlen(right) + expr->operation->lexeme.size() + 1) * sizeof(char));
    if (ptr == nullptr) return nullptr;

    ((char*)ptr)[0] = '\0';
    strcat((char*)ptr, expr->operation->lexeme.c_str()); strcat((char*)ptr, right);

    free(right);
    return ptr;
  }

  void* visitExprVar(Expr::Var* expr) {
    void* ptr = malloc((expr->token->lexeme.size() + 1) * sizeof(char));
    if (ptr == nullptr) return nullptr;

    ((char*)ptr)[0] = '\0';
    strcpy((char*)ptr, expr->token->lexeme.c_str());

    return ptr;
  }

  void* visitExprUnknown(Expression* expr) override { return nullptr; }

private:
  inline void* evaluate(std::shared_ptr<Expression> expr) {
    return expr->accept(this);
  }
};
};
