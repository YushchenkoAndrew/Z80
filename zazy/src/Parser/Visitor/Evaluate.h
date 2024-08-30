#pragma once
#include "src/Parser/Parser.h"


namespace Zazy {

class Evaluate : public Visitor {
public:

  Evaluate(Parser p):
    parser(p) {}

  // void scan(std::shared_ptr<Expression> expr) {
  //   char* src = (char*)evaluate(expr);
  //   if (src == nullptr) return (void)printf("NULLPTR\n");

  //   printf("[REVERSE] %s\n", src); free(src);
  // }

  obj_t next() {
    // parser.next()

    return evaluate(parser.declaration());
  }
  
public:

  // virtual obj_t visitStmtBlock(Stmt::Block* stmt) { return nullptr; }
  obj_t visitStmtExpr(Stmt::Expr* stmt) override { 
    return evaluate(stmt->expr);
  }
  // virtual obj_t visitStmtFor(Stmt::For* stmt) { return nullptr; }
  // virtual obj_t visitStmtIf(Stmt::If* stmt) { return nullptr; }
  // // virtual obj_t visitStmt(Stmt::If* stmt) { return nullptr; }
  // virtual obj_t visitStmtReturn(Stmt::Return* stmt) { return nullptr; }
  // virtual obj_t visitStmtUntil(Stmt::Until* stmt) { return nullptr; }
  // virtual obj_t visitStmtWhile(Stmt::While* stmt) { return nullptr; }


  // virtual obj_t visitExprArray(Expr::Array* expr) { return nullptr; }
  // virtual obj_t visitExprAssign(Expr::Assign* expr) { return nullptr; }

  obj_t visitExprBinary(Expr::Binary* expr) override {
    auto left = evaluate(expr->left);
    auto right = evaluate(expr->right);

    if (left == nullptr || right == nullptr) return nullptr;

    switch (expr->operation->token) {
    case TokenT::OP_PLUS:
      if (left->match<1>({ Obj::CHAR }) && right->match<1>({ Obj::CHAR })) {
        return std::make_shared<Obj::Char>((uint8_t)(left->value + right->value));
      }
      // if (left->type() == Obj::CHAR)

      break;
    
    default:
      break;
    }

    return nullptr;
  }

  obj_t visitExprLiteral(Expr::Literal* expr) override {
    switch (expr->token->token) {
      case TokenT::CHAR:
        return std::make_shared<Obj::Char>((uint8_t)std::stoul(expr->token->literal));

    }
    

    return nullptr;
  }

  // virtual obj_t visitExprCast(Expr::Cast* expr) { return nullptr; }
  // virtual obj_t visitExprGet(Expr::Get* expr) { return nullptr; }
  // virtual obj_t visitExprGroup(Expr::Group* expr) { return nullptr; }
  // virtual obj_t visitExprInvoke(Expr::Invoke* expr) { return nullptr; }
  // virtual obj_t visitExprSuffix(Expr::Suffix* expr) { return nullptr; }
  // virtual obj_t visitExprTernary(Expr::Ternary* expr) { return nullptr; }
  // virtual obj_t visitExprUnary(Expr::Unary* expr) { return nullptr; }
  // virtual obj_t visitExprVar(Expr::Var* expr) { return nullptr; }


private:
  inline obj_t evaluate(std::shared_ptr<Expression> expr) {
    return expr->accept(this);
  }

  inline obj_t evaluate(std::shared_ptr<Statement> stmt) {
    return stmt->accept(this);
  }


private:
  Parser parser;
};
};
