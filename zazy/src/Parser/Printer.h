#pragma once
#include "AST.h"

namespace Zazy {

/** Indented dump of the (typed) tree, used by the REPL `\p` command and `--ast`. */
class Printer {
public:
  static void print(const std::vector<stmt_t>& prog) { for (auto& s : prog) stmt(s.get(), 0); }

private:
  template <class T, class B> static T* as(B* p) { return dynamic_cast<T*>(p); }
  static void pad(int d) { printf("%*s", d * 2, ""); }
  static std::string ty(Expression* e) { return e->ty ? " : " + Types::str(e->ty) + (e->isConst ? " = " + std::to_string(e->cv) : "") : ""; }

  static void expr(Expression* e, int d) {
    if (!e) return;
    pad(d);
    if (auto* x = as<Expr::Literal>(e)) {
      if (x->kind == Expr::Literal::STR) printf("string \"%s\"%s\n", x->bytes.c_str(), ty(e).c_str());
      else printf("literal %d%s\n", x->value, ty(e).c_str());
    } else if (auto* x = as<Expr::Var>(e)) printf("var %s%s\n", x->name.c_str(), ty(e).c_str());
    else if (auto* x = as<Expr::Unary>(e)) { printf("unary %s%s\n", TokenName(x->op), ty(e).c_str()); expr(x->e.get(), d + 1); }
    else if (auto* x = as<Expr::Suffix>(e)) { printf("suffix %s%s\n", TokenName(x->op), ty(e).c_str()); expr(x->e.get(), d + 1); }
    else if (auto* x = as<Expr::Binary>(e)) { printf("binary %s%s\n", TokenName(x->op), ty(e).c_str()); expr(x->l.get(), d + 1); expr(x->r.get(), d + 1); }
    else if (auto* x = as<Expr::Assign>(e)) { printf("assign %s%s\n", TokenName(x->op), ty(e).c_str()); expr(x->target.get(), d + 1); expr(x->value.get(), d + 1); }
    else if (auto* x = as<Expr::Ternary>(e)) { printf("ternary%s\n", ty(e).c_str()); expr(x->cond.get(), d + 1); expr(x->a.get(), d + 1); expr(x->b.get(), d + 1); }
    else if (auto* x = as<Expr::Comma>(e)) { printf("comma%s\n", ty(e).c_str()); expr(x->l.get(), d + 1); expr(x->r.get(), d + 1); }
    else if (auto* x = as<Expr::Group>(e)) { printf("group%s\n", ty(e).c_str()); expr(x->e.get(), d + 1); }
    else if (auto* x = as<Expr::Cast>(e)) { printf("cast to %s%s\n", Types::str(x->to).c_str(), ty(e).c_str()); expr(x->e.get(), d + 1); }
    else if (auto* x = as<Expr::Sizeof>(e)) { printf("sizeof%s\n", ty(e).c_str()); if (x->e) expr(x->e.get(), d + 1); }
    else if (auto* x = as<Expr::Index>(e)) { printf("index%s\n", ty(e).c_str()); expr(x->base.get(), d + 1); expr(x->index.get(), d + 1); }
    else if (auto* x = as<Expr::Field>(e)) { printf("field %s%s%s\n", x->arrow ? "->" : ".", x->name.c_str(), ty(e).c_str()); expr(x->base.get(), d + 1); }
    else if (auto* x = as<Expr::Invoke>(e)) { printf("call%s\n", ty(e).c_str()); expr(x->callee.get(), d + 1); for (auto& a : x->args) expr(a.get(), d + 1); }
    else if (auto* x = as<Expr::Array>(e)) { printf("list\n"); for (auto& a : x->elems) expr(a.get(), d + 1); }
  }

  static void stmt(Statement* s, int d) {
    if (!s) return;
    pad(d);
    if (auto* x = as<Decl::Func>(s)) {
      printf("func %s%s%s\n", x->name.c_str(), Types::str(x->type).c_str(), x->bind.bound ? (" asm " + x->bind.label).c_str() : "");
      if (x->body) stmt(x->body.get(), d + 1);
    } else if (auto* x = as<Decl::Var>(s)) { printf("var %s : %s\n", x->name.c_str(), Types::str(x->type).c_str()); if (x->init) expr(x->init.get(), d + 1); }
    else if (auto* x = as<Decl::Struct>(s)) printf("struct %s\n", x->type->sdef->tag.c_str());
    else if (as<Decl::Enum>(s)) printf("enum\n");
    else if (auto* x = as<Decl::Typedef>(s)) printf("typedef %s = %s\n", x->name.c_str(), Types::str(x->type).c_str());
    else if (auto* x = as<Stmt::Block>(s)) { printf("block\n"); for (auto& c : x->body) stmt(c.get(), d + 1); }
    else if (auto* x = as<Stmt::ExprStmt>(s)) { printf("expr\n"); expr(x->e.get(), d + 1); }
    else if (auto* x = as<Stmt::If>(s)) { printf("if\n"); expr(x->cond.get(), d + 1); stmt(x->then.get(), d + 1); if (x->els) { pad(d); printf("else\n"); stmt(x->els.get(), d + 1); } }
    else if (auto* x = as<Stmt::While>(s)) { printf("while\n"); expr(x->cond.get(), d + 1); stmt(x->body.get(), d + 1); }
    else if (auto* x = as<Stmt::Do>(s)) { printf("do\n"); stmt(x->body.get(), d + 1); expr(x->cond.get(), d + 1); }
    else if (auto* x = as<Stmt::For>(s)) { printf("for\n"); stmt(x->init.get(), d + 1); expr(x->cond.get(), d + 1); expr(x->step.get(), d + 1); stmt(x->body.get(), d + 1); }
    else if (auto* x = as<Stmt::Return>(s)) { printf("return\n"); expr(x->value.get(), d + 1); }
    else if (as<Stmt::Break>(s)) printf("break\n");
    else if (as<Stmt::Continue>(s)) printf("continue\n");
    else if (auto* x = as<Stmt::Goto>(s)) printf("goto %s\n", x->name.c_str());
    else if (auto* x = as<Stmt::Label>(s)) printf("label %s\n", x->name.c_str());
    else if (auto* x = as<Stmt::Case>(s)) { if (x->isDefault) printf("default\n"); else printf("case %d\n", x->cv); }
    else if (auto* x = as<Stmt::Switch>(s)) { printf("switch\n"); expr(x->cond.get(), d + 1); stmt(x->body.get(), d + 1); }
    else if (auto* x = as<Stmt::Asm>(s)) printf("asm \"%s\"\n", x->text.c_str());
  }
};
};
