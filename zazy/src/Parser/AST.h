#pragma once
#include "Defs.h"

namespace Zazy {

#define ACCEPT(name) void accept(Visitor* v) override { v->visit##name(this); }

// ======================================================================================= Expr
namespace Expr {

/** 123  'a'  "text"  (enum constants are folded to a Literal by the parser) */
class Literal : public Expression {
public:
  enum Kind { NUM, CHR, STR };
  Literal(Kind k, int32_t v): kind(k), value(v) {}
  ACCEPT(ExprLiteral)

  Kind kind;
  int32_t value = 0;
  std::string bytes;       // STR: decoded bytes (without the terminating 0)
  bool forceUnsigned = false;
  int label = -1;          // STR: index in the string pool, filled by Sema
};

class Var : public Expression {
public:
  Var(std::string n): name(n) {}
  ACCEPT(ExprVar)

  std::string name;
  Symbol* sym = nullptr;
};

/** - ! ~ * & ++ -- (prefix) */
class Unary : public Expression {
public:
  Unary(TokenT o, expr_t e): op(o), e(e) {}
  ACCEPT(ExprUnary)

  TokenT op;               // OP_MINUS OP_PLUS OP_NOT BIT_NOT STAR AMPERSAND OP_INC OP_DEC
  expr_t e;
};

/** x++  x-- */
class Suffix : public Expression {
public:
  Suffix(TokenT o, expr_t e): op(o), e(e) {}
  ACCEPT(ExprSuffix)

  TokenT op;               // OP_INC OP_DEC
  expr_t e;
};

class Binary : public Expression {
public:
  Binary(TokenT o, expr_t l, expr_t r): op(o), l(l), r(r) {}
  ACCEPT(ExprBinary)

  TokenT op;               // arithmetic, BIT_*, AMPERSAND (bit and), comparisons, OP_AND, OP_OR
  expr_t l, r;
  Type* opTy = nullptr;    // Sema: type the operation itself is done in (after widening)
};

class Assign : public Expression {
public:
  Assign(TokenT o, expr_t t, expr_t v): op(o), target(t), value(v) {}
  ACCEPT(ExprAssign)

  TokenT op;               // ASSIGN, ASSIGN_PLUS ...
  expr_t target, value;
  Type* opTy = nullptr;
};

class Ternary : public Expression {
public:
  Ternary(expr_t c, expr_t a, expr_t b): cond(c), a(a), b(b) {}
  ACCEPT(ExprTernary)

  expr_t cond, a, b;
};

class Comma : public Expression {
public:
  Comma(expr_t l, expr_t r): l(l), r(r) {}
  ACCEPT(ExprComma)

  expr_t l, r;
};

class Group : public Expression {
public:
  Group(expr_t e): e(e) {}
  ACCEPT(ExprGroup)

  expr_t e;
};

class Cast : public Expression {
public:
  Cast(Type* t, expr_t e): to(t), e(e) {}
  ACCEPT(ExprCast)

  Type* to;
  expr_t e;
};

/** sizeof(type) or sizeof expr */
class Sizeof : public Expression {
public:
  Sizeof(Type* t, expr_t e): of(t), e(e) {}
  ACCEPT(ExprSizeof)

  Type* of;                // set for sizeof(type)
  expr_t e;                // set for sizeof expr
};

/** base[index] */
class Index : public Expression {
public:
  Index(expr_t b, expr_t i): base(b), index(i) {}
  ACCEPT(ExprIndex)

  expr_t base, index;
};

/** base.name  base->name */
class Field : public Expression {
public:
  Field(expr_t b, std::string n, bool arrow): base(b), name(n), arrow(arrow) {}
  ACCEPT(ExprField)

  expr_t base;
  std::string name;
  bool arrow;
  const ::Zazy::Member* member = nullptr;    // Sema
};

class Invoke : public Expression {
public:
  Invoke(expr_t c, std::vector<expr_t> a): callee(c), args(std::move(a)) {}
  ACCEPT(ExprInvoke)

  expr_t callee;
  std::vector<expr_t> args;
  FuncInfo* fn = nullptr;  // Sema: direct call
  Type* fnType = nullptr;  // Sema: call through a pointer (fn is null)
};

/** { a, b, c } initializer list (only valid as initializer) */
class Array : public Expression {
public:
  Array(std::vector<expr_t> e): elems(std::move(e)) {}
  ACCEPT(ExprArray)

  std::vector<expr_t> elems;
};
};

// ======================================================================================= Decl
namespace Decl {

struct Param {
  std::string name;
  Type* type = nullptr;
  token_t tok;
};

/** extern binding to a hand written asm routine: extern T f(T a, T b) asm("@LABEL", in(HL, A), out(HL), clobber(AF,BC));
 *  labels starting with '$' are compiler intrinsics (see lib/zazy.zh) */
struct Binding {
  bool bound = false;
  std::string label;
  std::vector<std::string> in;          // register per parameter, empty = default assignment
  std::string out;                      // register of the result, empty = default
  std::vector<std::string> clobber;
  bool hasClobber = false;
};

class Var : public Statement {
public:
  ACCEPT(DeclVar)

  std::string name;
  Type* type = nullptr;
  expr_t init;
  bool isStatic = false;
  bool isExtern = false;
  bool isConst = false;
  bool hasAddr = false;                 // uint8_t reg __at(0x6000);  fixed address, no storage
  uint32_t addr = 0;
  Symbol* sym = nullptr;
  std::vector<stmt_t> initCode;         // Sema: locals, the initializer lowered into assignments
};

class Func : public Statement {
public:
  ACCEPT(DeclFunc)

  std::string name;
  Type* ret = nullptr;
  Type* type = nullptr;                 // FUNC type
  std::vector<Param> params;
  stmt_t body;                          // Stmt::Block or null for a declaration
  bool isStatic = false;
  bool isExtern = false;
  bool isNaked = false;
  bool isInterrupt = false;   // saves and restores every register the body can touch
  Binding bind;
  FuncInfo* info = nullptr;
};

class Struct : public Statement {
public:
  ACCEPT(DeclStruct)
  Type* type = nullptr;
};

class Enum : public Statement {
public:
  ACCEPT(DeclEnum)
  std::string tag;
  std::vector<std::pair<std::string, int32_t>> items;
};

class Typedef : public Statement {
public:
  ACCEPT(DeclTypedef)
  std::string name;
  Type* type = nullptr;
};
};

// ======================================================================================= Stmt
namespace Stmt {

class Asm : public Statement {
public:
  Asm(std::string t): text(t) {}
  ACCEPT(StmtAsm)

  std::string text;                     // raw, may contain {name} references to C variables
  std::vector<std::pair<std::string, Symbol*>> refs;   // Sema
};

class Block : public Statement {
public:
  ACCEPT(StmtBlock)
  std::vector<stmt_t> body;
};

class ExprStmt : public Statement {
public:
  ExprStmt(expr_t e): e(e) {}
  ACCEPT(StmtExpr)
  expr_t e;
};

class If : public Statement {
public:
  ACCEPT(StmtIf)
  expr_t cond;
  stmt_t then, els;
};

class While : public Statement {
public:
  ACCEPT(StmtWhile)
  expr_t cond;
  stmt_t body;
};

class Do : public Statement {
public:
  ACCEPT(StmtDo)
  stmt_t body;
  expr_t cond;
};

class For : public Statement {
public:
  ACCEPT(StmtFor)
  stmt_t init;                          // declaration, expression statement or null
  expr_t cond, step;
  stmt_t body;
};

class Return : public Statement {
public:
  ACCEPT(StmtReturn)
  expr_t value;
};

class Break : public Statement {
public:
  ACCEPT(StmtBreak)
};

class Continue : public Statement {
public:
  ACCEPT(StmtContinue)
};

class Goto : public Statement {
public:
  Goto(std::string n): name(n) {}
  ACCEPT(StmtGoto)
  std::string name;
};

class Label : public Statement {
public:
  Label(std::string n): name(n) {}
  ACCEPT(StmtLabel)
  std::string name;
};

/** `case X:` / `default:` marker, the statements after it are siblings in the switch block */
class Case : public Statement {
public:
  ACCEPT(StmtCase)
  expr_t value;                         // null for default
  bool isDefault = false;
  int32_t cv = 0;                       // Sema
};

class Switch : public Statement {
public:
  ACCEPT(StmtSwitch)
  expr_t cond;
  stmt_t body;
  std::vector<Case*> cases;             // Sema
};
};

#undef ACCEPT
};
