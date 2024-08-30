#pragma once
#include "src/Lexer/Lexer.h"



namespace Zazy {

namespace Obj {
  enum TypeT {
    VOID, CHAR, SHORT, INT, PTR
  };

  class Char;
  class Short;
  class Int;
  class Void;
  class Ptr;
};


namespace Expr {
  #define ENCLOSE(...) printf("("); __VA_ARGS__ printf(")");

  class Array;
  class Assign;
  class Binary;
  class Cast;
  class Get;
  class Group;
  class Invoke;
  class Literal;
  class Suffix;
  class Ternary;
  class Unary;
  class Var;
};

namespace Stmt {

  class Block;
  class Expr;
  class For;
  class If;
  class Switch;
  class Return;
  class Until;
  class While;

//  *  struct_decl   -> TODO:
//  *  enum_decl     -> TODO:
//  *  func_decl     -> type IDENTIFIER '(' ')' block
//  *  var_decl      -> type IDENTIFIER ( '='  expression )? ';'
//  *  label_stmt    -> IDENTIFIER ':'
//  * 
//  *  statement     -> if_stmt | switch_stmt | for_stmt | while_stmt | do_while_stmt |
//  *                  return_stmt | goto_stmt | break_stmt | continue_stmt | expr_stmt | block
//  * 
//  *  if_stmt       -> 'if' '(' expression ')' statement ( 'else' statement )?
//  *  switch_stmt   -> 'switch' '(' expression ')' '{' case_stmt* '}'
//  *  case_stmt     -> ('case' expression ':' | 'default' ':') statement
//  *  for_stmt      -> 'for' '(' ( var_decl | expr_stmt | ';' ) expression? ';' expression? ')' statement
//  *  while_stmt    -> 'while' '(' expression ')' statement
//  *  do_while_stmt -> 'do' block 'while' '(' expression ')' ';'

};

class Object;
class Expression;
class Statement;

typedef std::shared_ptr<Object>     obj_t;
typedef std::shared_ptr<Expression> expr_t;
typedef std::shared_ptr<Statement>  stmt_t;

class Visitor {
public:
  virtual obj_t visitStmtBlock(Stmt::Block* stmt) { return nullptr; }
  virtual obj_t visitStmtExpr(Stmt::Expr* stmt) { return nullptr; }
  virtual obj_t visitStmtFor(Stmt::For* stmt) { return nullptr; }
  virtual obj_t visitStmtIf(Stmt::If* stmt) { return nullptr; }
  // virtual obj_t visitStmt(Stmt::If* stmt) { return nullptr; }
  virtual obj_t visitStmtReturn(Stmt::Return* stmt) { return nullptr; }
  virtual obj_t visitStmtUntil(Stmt::Until* stmt) { return nullptr; }
  virtual obj_t visitStmtWhile(Stmt::While* stmt) { return nullptr; }

  virtual obj_t visitExprArray(Expr::Array* expr) { return nullptr; }
  virtual obj_t visitExprAssign(Expr::Assign* expr) { return nullptr; }
  virtual obj_t visitExprBinary(Expr::Binary* expr) { return nullptr; }
  virtual obj_t visitExprCast(Expr::Cast* expr) { return nullptr; }
  virtual obj_t visitExprGet(Expr::Get* expr) { return nullptr; }
  virtual obj_t visitExprGroup(Expr::Group* expr) { return nullptr; }
  virtual obj_t visitExprInvoke(Expr::Invoke* expr) { return nullptr; }
  virtual obj_t visitExprLiteral(Expr::Literal* expr) { return nullptr; }
  virtual obj_t visitExprSuffix(Expr::Suffix* expr) { return nullptr; }
  virtual obj_t visitExprTernary(Expr::Ternary* expr) { return nullptr; }
  virtual obj_t visitExprUnary(Expr::Unary* expr) { return nullptr; }
  virtual obj_t visitExprVar(Expr::Var* expr) { return nullptr; }

  virtual obj_t visitExprUnknown(Expression* expr) { return nullptr; }
  virtual obj_t visitStmtUnknown(Statement*  stmt) { return nullptr; }
};

class Object {
public:
  Object(uint32_t value, uint8_t size):
    value(value), size(size) {}

  virtual Obj::TypeT type() { return Obj::TypeT::VOID; }
  virtual std::string string() { return ""; }

  template<int32_t T>
  bool match(std::array<Obj::TypeT, T> types) {
    for (auto& t: types) if (t == type()) return true;
    return false;
  }

public:
  uint8_t size;
  uint32_t value;
};

class Expression {
public:
  virtual obj_t accept(Visitor* visitor) {
    return visitor->visitExprUnknown(this);
  }
  
  virtual void print() {}
};

class Statement {
public:
  virtual obj_t accept(Visitor* visitor) {
    return visitor->visitStmtUnknown(this);
  }
  
  virtual void print() {}
};

};