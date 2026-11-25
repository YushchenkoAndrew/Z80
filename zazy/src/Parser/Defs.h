#pragma once
#include "src/Lexer/Lexer.h"
#include "src/Types/Types.h"
#include "src/Utils/Diag.h"

namespace Zazy {

struct Symbol;     // owned by Sema
struct FuncInfo;

namespace Expr {
  class Array;
  class Assign;
  class Binary;
  class Cast;
  class Comma;
  class Group;
  class Index;
  class Invoke;
  class Literal;
  class Field;
  class Sizeof;
  class Suffix;
  class Ternary;
  class Unary;
  class Var;
};

namespace Stmt {
  class Asm;
  class Block;
  class Break;
  class Case;
  class Continue;
  class Do;
  class ExprStmt;
  class For;
  class Goto;
  class If;
  class Label;
  class Return;
  class Switch;
  class While;
};

namespace Decl {
  class Enum;
  class Func;
  class Struct;
  class Typedef;
  class Var;
};

class Expression;
class Statement;

typedef std::shared_ptr<Expression> expr_t;
typedef std::shared_ptr<Statement>  stmt_t;

/**
 * Every pass (Sema, CodeGen, printers) is a Visitor. Same idea as before, but nothing is
 * evaluated any more: passes annotate the tree or emit code.
 */
class Visitor {
public:
  virtual ~Visitor() {}

  virtual void visitDeclStruct(Decl::Struct*) {}
  virtual void visitDeclEnum(Decl::Enum*) {}
  virtual void visitDeclTypedef(Decl::Typedef*) {}
  virtual void visitDeclFunc(Decl::Func*) {}
  virtual void visitDeclVar(Decl::Var*) {}

  virtual void visitStmtAsm(Stmt::Asm*) {}
  virtual void visitStmtBlock(Stmt::Block*) {}
  virtual void visitStmtBreak(Stmt::Break*) {}
  virtual void visitStmtCase(Stmt::Case*) {}
  virtual void visitStmtContinue(Stmt::Continue*) {}
  virtual void visitStmtDo(Stmt::Do*) {}
  virtual void visitStmtExpr(Stmt::ExprStmt*) {}
  virtual void visitStmtFor(Stmt::For*) {}
  virtual void visitStmtGoto(Stmt::Goto*) {}
  virtual void visitStmtIf(Stmt::If*) {}
  virtual void visitStmtLabel(Stmt::Label*) {}
  virtual void visitStmtReturn(Stmt::Return*) {}
  virtual void visitStmtSwitch(Stmt::Switch*) {}
  virtual void visitStmtWhile(Stmt::While*) {}

  virtual void visitExprArray(Expr::Array*) {}
  virtual void visitExprAssign(Expr::Assign*) {}
  virtual void visitExprBinary(Expr::Binary*) {}
  virtual void visitExprCast(Expr::Cast*) {}
  virtual void visitExprComma(Expr::Comma*) {}
  virtual void visitExprGroup(Expr::Group*) {}
  virtual void visitExprIndex(Expr::Index*) {}
  virtual void visitExprInvoke(Expr::Invoke*) {}
  virtual void visitExprLiteral(Expr::Literal*) {}
  virtual void visitExprField(Expr::Field*) {}
  virtual void visitExprSizeof(Expr::Sizeof*) {}
  virtual void visitExprSuffix(Expr::Suffix*) {}
  virtual void visitExprTernary(Expr::Ternary*) {}
  virtual void visitExprUnary(Expr::Unary*) {}
  virtual void visitExprVar(Expr::Var*) {}
};

class Expression {
public:
  virtual ~Expression() {}
  virtual void accept(Visitor* visitor) = 0;
  virtual void print() {}

public:
  token_t tok = nullptr;     // for diagnostics

  // filled by Sema
  Type* ty = nullptr;
  bool lval = false;
  bool isConst = false;      // folded compile time constant
  int32_t cv = 0;
  bool typedConst = false;   // constant with an explicit type (cast): does not adopt the type of the other operand
  bool exact = true;         // false: narrow arithmetic that may overflow its width (see Sema::needFull)
};

class Statement {
public:
  virtual ~Statement() {}
  virtual void accept(Visitor* visitor) = 0;
  virtual void print() {}

public:
  token_t tok = nullptr;
};
};
