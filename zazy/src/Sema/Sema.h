#pragma once
#include <map>
#include <set>
#include "Symbols.h"

namespace Zazy {

/**
 * Semantic analysis: resolves names, types every expression, folds constants, inserts the
 * implicit conversions as explicit Cast nodes, lays out stack frames and collects switch cases
 * and string literals. After it ran without errors the tree is ready for the code generator,
 * which never has to think about types again.
 *
 * Arithmetic width rule (differs from ISO C on purpose, it keeps 8 bit code 8 bit):
 *   an operation is done in the wider of its operand types, constants adopt the other operand's
 *   type when they fit. When such a narrow result is *assigned / passed / cast to a wider type*
 *   the whole expression is redone in the wider type (retype), so `uint16_t c = a + b` with two
 *   uint8_t does not lose the carry. Comparisons, division and right shifts are never retyped.
 */
class Sema {
public:
  std::deque<Symbol> symStore;
  std::deque<FuncInfo> fnStore;
  std::vector<FuncInfo*> funcs;          // in source order
  std::vector<Symbol*> globals;          // variables incl. static locals, in source order
  std::vector<std::string> strings;      // string literal pool

  bool run(std::vector<stmt_t>& prog) {
    int before = Diag::errorCount();
    scopes.clear();
    scopes.emplace_back();
    declareBuiltins();

    // pass 1: names of all functions and globals (so order of definition does not matter)
    for (auto& s : prog) {
      try {
        if (auto* f = dynamic_cast<Decl::Func*>(s.get())) declareFunc(f);
        else if (auto* v = dynamic_cast<Decl::Var*>(s.get())) declareGlobal(v);
      } catch (Abort&) {}
    }
    // pass 2: initialisers and bodies
    for (auto& s : prog) {
      try {
        if (auto* f = dynamic_cast<Decl::Func*>(s.get())) { if (f->body) checkFunc(f); }
        else if (auto* v = dynamic_cast<Decl::Var*>(s.get())) { if (v->init) checkGlobalInit(v); }
      } catch (Abort&) {}
    }
    for (auto* f : funcs) {
      if (f->used && !f->defined && !f->decl->bind.bound)
        Diag::error(f->decl->tok ? f->decl->tok->line : 0, f->decl->tok ? f->decl->tok->col : 0, "function '" + f->name + "' is used but never defined");
    }
    return Diag::errorCount() == before;
  }

  int strLabel(const std::string& bytes) {
    for (size_t i = 0; i < strings.size(); i++) if (strings[i] == bytes) return (int)i;
    strings.push_back(bytes);
    return (int)strings.size() - 1;
  }

  Symbol* findGlobal(const std::string& n) {
    auto it = scopes[0].find(n);
    return it == scopes[0].end() ? nullptr : it->second;
  }

  // ------------------------------------------------------------------ type helpers (public: used by CodeGen)
  static bool fits(int32_t v, const Type* t) {
    switch (t->kind) {
      case Type::U8:  return v >= 0 && v <= 255;
      case Type::I8:  return v >= -128 && v <= 127;
      case Type::U16: case Type::PTR: return v >= 0 && v <= 65535;
      case Type::I16: return v >= -32768 && v <= 32767;
      default: return false;
    }
  }

  static Type* valueType(int32_t v) {
    if (v >= 0 && v <= 255) return Types::U8();
    if (v >= -128 && v < 0) return Types::I8();
    if (v >= 256 && v <= 65535) return Types::U16();
    return Types::I16();
  }

  static int32_t truncTo(int32_t v, const Type* t) {
    switch (t->kind) {
      case Type::U8:  return v & 0xFF;
      case Type::I8:  return (int8_t)v;
      case Type::U16: case Type::PTR: return v & 0xFFFF;
      case Type::I16: return (int16_t)v;
      default: return v;
    }
  }

private:
  struct Abort {};

  std::vector<std::map<std::string, Symbol*>> scopes;
  FuncInfo* fn = nullptr;
  int curOff = 0;
  int loopDepth = 0, switchDepth = 0;
  std::vector<Stmt::Switch*> switches;
  std::set<std::string> gotoTargets;
  std::vector<std::pair<std::string, token_t>> gotos;
  int staticSeq = 0;
  std::set<std::string> usedLabels;

  // ------------------------------------------------------------------ diagnostics
  [[noreturn]] void fail(const token_t& t, const std::string& msg) {
    Diag::error(t ? t->line : 0, t ? t->col : 0, msg);
    throw Abort();
  }
  void err(const token_t& t, const std::string& msg) { Diag::error(t ? t->line : 0, t ? t->col : 0, msg); }
  void warn(const token_t& t, const std::string& msg) { Diag::warning(t ? t->line : 0, t ? t->col : 0, msg); }

  // ------------------------------------------------------------------ scopes
  Symbol* newSym(Symbol::Kind k, const std::string& n, Type* t) {
    symStore.emplace_back();
    Symbol* s = &symStore.back();
    s->kind = k; s->name = n; s->type = t;
    return s;
  }

  Symbol* lookup(const std::string& n) {
    for (size_t i = scopes.size(); i-- > 0;) {
      auto it = scopes[i].find(n);
      if (it != scopes[i].end()) return it->second;
    }
    return nullptr;
  }

  static std::string mangle(std::string s) {
    // the assembler's reader treats these upper case words as directives anywhere inside a line
    for (const char* bad : {"INCLUDE", "MACRO", "ENDM"}) {
      size_t p;
      while ((p = s.find(bad)) != std::string::npos) s.insert(p + 1, "_");
    }
    return s;
  }

  // ------------------------------------------------------------------ builtins used by lowered initializers
  std::vector<std::shared_ptr<Decl::Func>> builtins;

  void builtin(const std::string& name, std::vector<Type*> ps, const std::string& label, std::vector<std::string> in) {
    auto f = std::make_shared<Decl::Func>();
    f->name = name; f->ret = Types::Void(); f->isExtern = true;
    for (size_t i = 0; i < ps.size(); i++) { Decl::Param p; p.name = "a" + std::to_string(i); p.type = ps[i]; f->params.push_back(p); }
    f->type = Types::func(f->ret, ps);
    f->bind.bound = true; f->bind.label = label; f->bind.in = in; f->bind.hasClobber = false;
    builtins.push_back(f);
    declareFunc(f.get());
  }

  void declareBuiltins() {
    Type* bp = Types::ptr(Types::U8());
    builtin("zz_memset", {bp, Types::U8(), Types::U16()}, "@ZZ_MEMSET", {"HL", "A", "BC"});
    builtin("zz_memcpy", {bp, Types::ptr(Types::constOf(Types::U8()), true), Types::U16()}, "$ldir", {"DE", "HL", "BC"});
  }

  // ------------------------------------------------------------------ lowering of local initializers
  expr_t numLit(int32_t v, const token_t& tok) {
    auto l = std::make_shared<Expr::Literal>(Expr::Literal::NUM, v);
    l->tok = tok;
    return l;
  }

  expr_t byteAddr(expr_t lvalue, const token_t& tok) {
    auto a = std::make_shared<Expr::Unary>(AMPERSAND, lvalue);
    a->tok = tok;
    auto c = std::make_shared<Expr::Cast>(Types::ptr(Types::U8()), a);
    c->tok = tok;
    return c;
  }

  void callStmt(const std::string& fname, std::vector<expr_t> args, const token_t& tok, std::vector<stmt_t>& out) {
    auto v = std::make_shared<Expr::Var>(fname);
    v->tok = tok;
    auto call = std::make_shared<Expr::Invoke>(v, std::move(args));
    call->tok = tok;
    auto es = std::make_shared<Stmt::ExprStmt>(call);
    es->tok = tok;
    out.push_back(es);
  }

  void zeroBytes(expr_t lvalue, uint32_t count, const token_t& tok, std::vector<stmt_t>& out) {
    if (!count) return;
    callStmt("zz_memset", {byteAddr(lvalue, tok), numLit(0, tok), numLit((int32_t)count, tok)}, tok, out);
  }

  void lower(expr_t target, Type* t, expr_t init, std::vector<stmt_t>& out, const token_t& tok) {
    if (auto* lst = dynamic_cast<Expr::Array*>(init.get())) {
      if (t->isArray()) {
        size_t i = 0;
        for (; i < lst->elems.size(); i++) {
          auto ix = std::make_shared<Expr::Index>(target, numLit((int32_t)i, tok));
          ix->tok = tok;
          lower(ix, t->base, lst->elems[i], out, tok);
        }
        if (i < t->count) {
          auto ix = std::make_shared<Expr::Index>(target, numLit((int32_t)i, tok));
          ix->tok = tok;
          zeroBytes(ix, (t->count - (uint32_t)i) * t->base->size(), tok, out);
        }
      } else if (t->isStruct()) {
        auto& ms = t->sdef->members;
        for (size_t i = 0; i < ms.size(); i++) {
          auto f = std::make_shared<Expr::Field>(target, ms[i].name, false);
          f->tok = tok;
          if (i < lst->elems.size()) lower(f, ms[i].type, lst->elems[i], out, tok);
          else zeroBytes(f, ms[i].type->size(), tok, out);
        }
      } else lower(target, t, lst->elems[0], out, tok);
      return;
    }
    if (t->isArray()) {                                   // char s[N] = "text"
      auto* lit = dynamic_cast<Expr::Literal*>(init.get());
      lit->label = strLabel(lit->bytes);
      lit->ty = Types::array(Types::constOf(Types::U8()), (uint32_t)lit->bytes.size() + 1);
      uint32_t n = std::min<uint32_t>((uint32_t)lit->bytes.size() + 1, t->count);
      auto ix = std::make_shared<Expr::Index>(target, numLit(0, tok));
      ix->tok = tok;
      callStmt("zz_memcpy", {byteAddr(ix, tok), init, numLit((int32_t)n, tok)}, tok, out);
      if (n < t->count) {
        auto ix2 = std::make_shared<Expr::Index>(target, numLit((int32_t)n, tok));
        ix2->tok = tok;
        zeroBytes(ix2, t->count - n, tok, out);
      }
      return;
    }
    auto a = std::make_shared<Expr::Assign>(ASSIGN, target, init);
    a->tok = tok;
    auto es = std::make_shared<Stmt::ExprStmt>(a);
    es->tok = tok;
    out.push_back(es);
  }

  // ------------------------------------------------------------------ declarations
  static bool regIs8(const std::string& r) { return r.size() == 1; }

  void declareFunc(Decl::Func* d) {
    Symbol* ex = findGlobal(d->name);
    FuncInfo* fi = nullptr;
    if (ex) {
      if (ex->kind != Symbol::FUNC) fail(d->tok, "'" + d->name + "' redeclared as a function");
      fi = ex->fn;
      if (!Types::same(fi->type, d->type)) fail(d->tok, "conflicting types for '" + d->name + "': " + Types::str(fi->type) + " vs " + Types::str(d->type));
      if (d->body && fi->defined) fail(d->tok, "redefinition of '" + d->name + "'");
      if (d->bind.bound && fi->decl->bind.bound && d->bind.label != fi->decl->bind.label) fail(d->tok, "conflicting asm binding for '" + d->name + "'");
    } else {
      fnStore.emplace_back();
      fi = &fnStore.back();
      Symbol* s = newSym(Symbol::FUNC, d->name, d->type);
      s->fn = fi; s->isExtern = d->isExtern;
      fi->sym = s;
      fi->name = d->name; fi->type = d->type; fi->ret = d->ret;
      scopes[0][d->name] = s;
      funcs.push_back(fi);
    }
    if (!fi->decl || d->body || (d->bind.bound && !fi->decl->bind.bound)) fi->decl = d;
    d->info = fi;
    fi->isStatic = d->isStatic;
    fi->isNaked = d->isNaked;
    fi->isInterrupt = d->isInterrupt;
    if (d->body) fi->defined = true;

    if (d->bind.bound) {
      fi->label = d->bind.label;
      checkBinding(d);
    } else {
      fi->label = d->name == "main" ? "MAIN" : d->name == "setup" ? "SETUP" : "@" + mangle(d->name);
      if (d->type->params.size() > 3) fail(d->tok, "'" + d->name + "' has " + std::to_string(d->type->params.size()) + " parameters, at most 3 are passed in registers (pass a struct pointer instead)");
    }
    if (d->name == "main" || d->name == "setup") {
      if (!d->type->params.empty()) err(d->tok, "'" + d->name + "' must not take parameters");
      if (!d->ret->isVoid()) warn(d->tok, "'" + d->name + "' should return void");
    }
    if (d->isNaked && !d->params.empty()) err(d->tok, "naked function '" + d->name + "' cannot have parameters");
    if (d->isInterrupt) {
      if (!d->params.empty()) err(d->tok, "interrupt function '" + d->name + "' cannot have parameters");
      if (!d->ret->isVoid()) err(d->tok, "interrupt function '" + d->name + "' must return void");
      if (d->isNaked) err(d->tok, "'" + d->name + "' cannot be both naked and interrupt");
    }
    if (d->body && d->bind.bound) fi->used = true;                // named entry points are referenced from asm
  }

  void checkBinding(Decl::Func* d) {
    auto& b = d->bind;
    if (!b.in.empty()) {
      if (b.in.size() != d->params.size()) fail(d->tok, "in(...) lists " + std::to_string(b.in.size()) + " registers for " + std::to_string(d->params.size()) + " parameters");
      std::set<std::string> seen;
      for (size_t i = 0; i < b.in.size(); i++) {
        bool w8 = d->params[i].type->width() == 1;
        if (w8 != regIs8(b.in[i])) fail(d->tok, "parameter " + std::to_string(i + 1) + " of '" + d->name + "' is " + std::string(w8 ? "8" : "16") + " bit, register " + b.in[i] + " is not");
        if (!seen.insert(b.in[i]).second) fail(d->tok, "register " + b.in[i] + " used twice in in(...)");
      }
    } else if (d->params.size() > 3) fail(d->tok, "too many parameters, give an explicit in(...) list");
    if (!b.out.empty()) {
      if (d->ret->isVoid()) fail(d->tok, "out(" + b.out + ") on a void function");
      bool w8 = d->ret->width() == 1;
      if (w8 != regIs8(b.out)) fail(d->tok, "return value of '" + d->name + "' does not fit register " + b.out);
    }
  }

  void declareGlobal(Decl::Var* v) {
    Symbol* ex = findGlobal(v->name);
    if (ex) {
      if (ex->kind == Symbol::FUNC) fail(v->tok, "'" + v->name + "' redeclared as a variable");
      if (!Types::same(ex->type, v->type) && !(ex->type->isArray() && v->type->isArray() && Types::same(ex->type->base, v->type->base)))
        fail(v->tok, "conflicting types for '" + v->name + "'");
      if (!v->isExtern && !ex->isExtern && !(ex->var && !ex->var->init && !v->init)) {
        if (v->init && ex->var && ex->var->init) fail(v->tok, "redefinition of '" + v->name + "'");
      }
      if (!v->isExtern) { ex->isExtern = false; if (v->init || !ex->var) ex->var = v; ex->type = v->type; }
      v->sym = ex;
      return;
    }
    Symbol* s = newSym(Symbol::GLOBAL, v->name, v->type);
    s->label = "_" + mangle(v->name);
    s->isConst = v->isConst;
    s->isExtern = v->isExtern;
    s->hasAddr = v->hasAddr; s->addr = v->addr;
    s->var = v;
    v->sym = s;
    scopes[0][v->name] = s;
    if (!v->isExtern) globals.push_back(s);
    else if (!v->hasAddr) s->label = "_" + mangle(v->name);   // defined in hand written asm
  }

  void checkGlobalInit(Decl::Var* v) {
    initCheck(v->type, v->init, v->tok, true);
  }

  // ------------------------------------------------------------------ initialisers
  bool isAddressConst(Expression* e) {
    if (auto* c = dynamic_cast<Expr::Cast*>(e)) return isAddressConst(c->e.get());
    if (auto* g = dynamic_cast<Expr::Group*>(e)) return isAddressConst(g->e.get());
    if (auto* l = dynamic_cast<Expr::Literal*>(e)) return l->kind == Expr::Literal::STR;
    if (auto* u = dynamic_cast<Expr::Unary*>(e)) {
      if (u->op != AMPERSAND) return false;
      Expression* x = u->e.get();
      while (true) {
        if (auto* f = dynamic_cast<Expr::Field*>(x)) { if (f->arrow) return false; x = f->base.get(); }
        else if (auto* ix = dynamic_cast<Expr::Index*>(x)) { if (!ix->index->isConst) return false; x = ix->base.get(); }
        else if (auto* cc = dynamic_cast<Expr::Cast*>(x)) x = cc->e.get();          // array to pointer decay of an inner dimension
        else break;
      }
      auto* v = dynamic_cast<Expr::Var*>(x);
      return v && v->sym && v->sym->kind == Symbol::GLOBAL;
    }
    if (auto* v = dynamic_cast<Expr::Var*>(e)) {
      if (v->sym && v->sym->kind == Symbol::FUNC) return true;                       // function address
      return v->sym && v->sym->kind == Symbol::GLOBAL && v->ty && v->ty->isArray();
    }
    if (auto* b = dynamic_cast<Expr::Binary*>(e)) {
      if (b->op != OP_PLUS && b->op != OP_MINUS) return false;
      return (isAddressConst(b->l.get()) && b->r->isConst) || (b->op == OP_PLUS && b->l->isConst && isAddressConst(b->r.get()));
    }
    return false;
  }

  void initCheck(Type* t, expr_t& init, const token_t& where, bool global) {
    if (auto* lst = dynamic_cast<Expr::Array*>(init.get())) {
      if (t->isArray()) {
        if (lst->elems.size() > t->count) fail(lst->tok, "too many initializers (" + std::to_string(lst->elems.size()) + ") for " + Types::str(t));
        for (auto& e : lst->elems) initCheck(t->base, e, where, global);
      } else if (t->isStruct()) {
        auto& ms = t->sdef->members;
        if (lst->elems.size() > ms.size()) fail(lst->tok, "too many initializers for " + Types::str(t));
        for (size_t i = 0; i < lst->elems.size(); i++) initCheck(ms[i].type, lst->elems[i], where, global);
      } else {
        if (lst->elems.size() != 1) fail(lst->tok, "scalar initializer needs exactly one value");
        initCheck(t, lst->elems[0], where, global);
      }
      lst->ty = t;
      return;
    }
    // char s[] = "text"
    if (t->isArray()) {
      auto* lit = dynamic_cast<Expr::Literal*>(init.get());
      if (lit && lit->kind == Expr::Literal::STR && t->base->size() == 1) {
        if (lit->bytes.size() > t->count) fail(lit->tok, "string is too long for " + Types::str(t));
        lit->ty = t;
        return;
      }
      fail(init->tok, "array initializer must be a { list } or a string");
    }
    if (t->isStruct()) {
      if (global) fail(init->tok, "struct initializer must be a { list }");
      coerce(init, t, init->tok, "initialization");        // local: copy from another struct of the same type
      return;
    }
    coerce(init, t, init->tok, "initialization");
    if (global && !init->isConst && !isAddressConst(init.get())) fail(init->tok, "initializer of a global must be a constant");
  }

  // ------------------------------------------------------------------ functions
  void checkFunc(Decl::Func* d) {
    FuncInfo* f = d->info;
    fn = f;
    curOff = 0;
    loopDepth = switchDepth = 0;
    gotoTargets.clear(); gotos.clear();
    scopes.emplace_back();

    f->params.clear();
    if (!d->isNaked) {
      for (auto& p : d->params) {
        Symbol* s = newSym(Symbol::PARAM, p.name, p.type);
        s->off = curOff; curOff += p.type->size();
        s->used = true;
        if (scopes.back().count(p.name)) err(p.tok, "duplicate parameter '" + p.name + "'");
        scopes.back()[p.name] = s;
        f->params.push_back(s);
      }
    }
    f->frame = curOff;
    try { block(static_cast<Stmt::Block*>(d->body.get()), false); } catch (Abort&) {}

    for (auto& g : gotos) if (!gotoTargets.count(g.first)) err(g.second, "label '" + g.first + "' used but not defined");
    f->labels.assign(gotoTargets.begin(), gotoTargets.end());
    scopes.pop_back();
    fn = nullptr;
  }

  int allocLocal(Symbol* s) {
    s->off = curOff;
    curOff += std::max<uint32_t>(1, s->type->size());
    if (curOff > fn->frame) fn->frame = curOff;
    return s->off;
  }

  void block(Stmt::Block* b, bool scope = true) {
    int saveOff = curOff;
    if (scope) scopes.emplace_back();
    for (auto& s : b->body) {
      try { stmt(s); } catch (Abort&) {}
    }
    if (scope) scopes.pop_back();
    curOff = saveOff;
  }

  void cond(expr_t& e) {
    value(e);
    if (!e->ty->isScalar()) fail(e->tok, "condition must be an integer or pointer, not " + Types::str(e->ty));
  }

  void stmt(stmt_t& s) {
    Statement* p = s.get();
    if (!p) return;
    if (auto* b = dynamic_cast<Stmt::Block*>(p)) { block(b); return; }
    if (auto* e = dynamic_cast<Stmt::ExprStmt*>(p)) { expr(e->e); return; }
    if (auto* v = dynamic_cast<Decl::Var*>(p)) { localVar(v); return; }
    if (auto* i = dynamic_cast<Stmt::If*>(p)) {
      cond(i->cond);
      scoped(i->then);
      if (i->els) scoped(i->els);
      return;
    }
    if (auto* w = dynamic_cast<Stmt::While*>(p)) {
      cond(w->cond);
      loopDepth++; scoped(w->body); loopDepth--;
      return;
    }
    if (auto* d = dynamic_cast<Stmt::Do*>(p)) {
      loopDepth++; scoped(d->body); loopDepth--;
      cond(d->cond);
      return;
    }
    if (auto* f = dynamic_cast<Stmt::For*>(p)) {
      int saveOff = curOff;
      scopes.emplace_back();
      if (f->init) stmt(f->init);
      if (f->cond) cond(f->cond);
      if (f->step) expr(f->step);
      loopDepth++; scoped(f->body); loopDepth--;
      scopes.pop_back();
      curOff = saveOff;
      return;
    }
    if (auto* r = dynamic_cast<Stmt::Return*>(p)) {
      if (fn->ret->isVoid()) {
        if (r->value) err(r->tok, "void function '" + fn->name + "' returns a value");
      } else if (!r->value) {
        warn(r->tok, "non-void function '" + fn->name + "' returns nothing");
      } else coerce(r->value, fn->ret, r->tok, "return");
      return;
    }
    if (dynamic_cast<Stmt::Break*>(p)) { if (!loopDepth && !switchDepth) err(p->tok, "break outside of a loop or switch"); return; }
    if (dynamic_cast<Stmt::Continue*>(p)) { if (!loopDepth) err(p->tok, "continue outside of a loop"); return; }
    if (auto* g = dynamic_cast<Stmt::Goto*>(p)) { gotos.push_back({g->name, g->tok}); return; }
    if (auto* l = dynamic_cast<Stmt::Label*>(p)) {
      if (!gotoTargets.insert(l->name).second) err(l->tok, "duplicate label '" + l->name + "'");
      return;
    }
    if (auto* sw = dynamic_cast<Stmt::Switch*>(p)) {
      cond(sw->cond);
      if (!sw->cond->ty->isInt()) err(sw->tok, "switch needs an integer");
      switches.push_back(sw); switchDepth++;
      sw->cases.clear();
      scoped(sw->body);
      switchDepth--; switches.pop_back();
      return;
    }
    if (auto* c = dynamic_cast<Stmt::Case*>(p)) {
      if (switches.empty()) fail(c->tok, "case outside of a switch");
      auto* sw = switches.back();
      for (auto* o : sw->cases) {
        if (o->isDefault && c->isDefault) err(c->tok, "duplicate default");
        if (!o->isDefault && !c->isDefault && o->cv == c->cv) err(c->tok, "duplicate case " + std::to_string(c->cv));
      }
      if (!c->isDefault && !fits(c->cv, sw->cond->ty) && !(sw->cond->ty->width() == 2 && c->cv >= -32768 && c->cv <= 65535))
        warn(c->tok, "case value " + std::to_string(c->cv) + " does not fit " + Types::str(sw->cond->ty));
      sw->cases.push_back(c);
      return;
    }
    if (auto* a = dynamic_cast<Stmt::Asm*>(p)) { asmRefs(a); return; }
    // typedef / struct / enum declarations inside functions need nothing
  }

  void scoped(stmt_t& s) {
    int saveOff = curOff;
    scopes.emplace_back();
    try { stmt(s); } catch (Abort&) {}
    scopes.pop_back();
    curOff = saveOff;
  }

  void asmRefs(Stmt::Asm* a) {
    // {name} refers to a C variable or function
    const std::string& t = a->text;
    for (size_t i = 0; i < t.size(); i++) {
      if (t[i] != '{') continue;
      size_t e = t.find('}', i);
      if (e == std::string::npos) fail(a->tok, "unterminated { in asm");
      std::string n = Trim(t.substr(i + 1, e - i - 1));
      Symbol* s = lookup(n);
      if (!s) fail(a->tok, "asm refers to unknown name '" + n + "'");
      s->used = true;
      if (s->kind == Symbol::FUNC) { s->fn->used = true; }
      bool dup = false;
      for (auto& r : a->refs) if (r.first == n) dup = true;
      if (!dup) a->refs.push_back({n, s});
      i = e;
    }
  }

  void localVar(Decl::Var* v) {
    if (scopes.back().count(v->name)) fail(v->tok, "redefinition of '" + v->name + "' in this scope");
    Symbol* s;
    if (v->isStatic || v->isConst) {
      // static (and const) locals live in RAM / ROM like globals
      s = newSym(Symbol::GLOBAL, v->name, v->type);
      s->label = "_" + mangle(fn->name) + "_" + mangle(v->name) + (staticSeq ? "_" + std::to_string(staticSeq) : "");
      staticSeq++;
      s->isConst = v->isConst;
      s->var = v;
      globals.push_back(s);
      if (v->init) initCheck(v->type, v->init, v->tok, true);
    } else {
      s = newSym(Symbol::LOCAL, v->name, v->type);
      if (v->init) initCheck(v->type, v->init, v->tok, false);
      allocLocal(s);
      s->var = v;
    }
    v->sym = s;
    scopes.back()[v->name] = s;

    if (v->init && s->kind == Symbol::LOCAL) {
      auto target = std::make_shared<Expr::Var>(v->name);
      target->tok = v->tok;
      lower(target, v->type, v->init, v->initCode, v->tok);
      for (auto& st : v->initCode) stmt(st);
    }
  }

  // ------------------------------------------------------------------ conversions
  expr_t castTo(expr_t e, Type* t) {
    auto c = std::make_shared<Expr::Cast>(t, e);
    c->tok = e->tok;
    c->ty = t;
    c->lval = false;
    if (e->isConst && t->isScalar()) { c->isConst = true; c->cv = truncTo(e->cv, t); }
    return c;
  }

  // natural width arithmetic that can be redone in a wider type without changing meaning
  static bool retypable(Expression* e) {
    if (e->isConst) return false;
    if (auto* b = dynamic_cast<Expr::Binary*>(e)) {
      switch (b->op) {
        case OP_PLUS: case OP_MINUS: case STAR: case AMPERSAND: case BIT_OR: case BIT_XOR: case BIT_LEFT_SHIFT:
          return b->ty && b->ty->isInt();
        default: return false;
      }
    }
    if (auto* u = dynamic_cast<Expr::Unary*>(e)) return (u->op == OP_MINUS || u->op == BIT_NOT) && u->ty->isInt();
    if (auto* g = dynamic_cast<Expr::Group*>(e)) return retypable(g->e.get());
    if (auto* t = dynamic_cast<Expr::Ternary*>(e)) return t->ty->isInt();
    return false;
  }

  void widenSlot(expr_t& c, Type* t) {
    if (c->ty->width() < t->width() && retypable(c.get())) retype(c, t);
    else if (!Types::same(c->ty, t)) c = castTo(c, t);
  }

  void retype(expr_t& e, Type* t) {
    if (auto* b = dynamic_cast<Expr::Binary*>(e.get())) {
      widenSlot(b->l, t);
      if (b->op != BIT_LEFT_SHIFT) widenSlot(b->r, t);
      b->ty = b->opTy = t;
    } else if (auto* u = dynamic_cast<Expr::Unary*>(e.get())) {
      widenSlot(u->e, t);
      u->ty = t;
    } else if (auto* g = dynamic_cast<Expr::Group*>(e.get())) {
      widenSlot(g->e, t);
      g->ty = t;
    } else if (auto* tn = dynamic_cast<Expr::Ternary*>(e.get())) {
      widenSlot(tn->a, t); widenSlot(tn->b, t);
      tn->ty = t;
    }
  }

  // assignment-like conversion of `e` to type `to`
  void coerce(expr_t& e, Type* to, const token_t& where, const std::string& ctx) {
    value(e);
    Type* from = e->ty;
    if (to->isVoid()) return;
    if (to->isStruct() || from->isStruct()) {
      if (!Types::same(from, to)) fail(where, "cannot convert " + Types::str(from) + " to " + Types::str(to) + " in " + ctx);
      return;
    }
    if (from->isVoid()) fail(where, "void value used in " + ctx);
    if (!to->isScalar() || !from->isScalar()) fail(where, "cannot convert " + Types::str(from) + " to " + Types::str(to) + " in " + ctx);
    if (to->isPtr() && from->isPtr()) {
      Type *a = to->base, *b = from->base;
      bool ok = a->isVoid() || b->isVoid() || Types::same(a, b) || (a->size() == 1 && b->size() == 1 && a->isInt() && b->isInt());
      if (!ok) warn(where, "incompatible pointer types in " + ctx + ": " + Types::str(from) + " to " + Types::str(to));
      if (b->isConst && !a->isConst && !a->isVoid()) warn(where, "discards const qualifier in " + ctx);
      if (!Types::same(from, to)) e = castTo(e, to);
      return;
    }
    if (e->isConst) {
      if (!fits(e->cv, to) && !(to->isInt() && to->width() == 1 && e->cv >= -128 && e->cv <= 255) && !(to->width() == 2 && e->cv >= -32768 && e->cv <= 65535))
        warn(where, "constant " + std::to_string(e->cv) + " truncated to " + Types::str(to));
      if (!Types::same(from, to)) e = castTo(e, to);
      return;
    }
    if (to->isPtr() && from->isInt() && from->width() == 1) warn(where, "8 bit value converted to a pointer");
    if (from->isPtr() && to->isInt() && to->width() == 1) warn(where, "pointer truncated to 8 bit");
    if (Types::same(from, to)) return;
    if (from->isInt() && to->isInt() && from->width() < to->width() && retypable(e.get())) { retype(e, to); return; }
    e = castTo(e, to);
  }

  // expression used as a value: arrays decay, functions / void / struct rejected where needed
  Type* value(expr_t& e) {
    expr(e);
    if (e->ty->isArray()) {
      Type* p = Types::ptr(e->ty->base, e->ty->base->isConst);
      auto c = std::make_shared<Expr::Cast>(p, e);
      c->tok = e->tok; c->ty = p;
      e = c;
    }
    return e->ty;
  }

  // ------------------------------------------------------------------ expressions
  void expr(expr_t& e) {
    Expression* p = e.get();
    if (e->ty) return;                                  // already typed (shared sub trees)
    if (auto* x = dynamic_cast<Expr::Literal*>(p)) return literal(x);
    if (auto* x = dynamic_cast<Expr::Var*>(p)) return variable(x);
    if (auto* x = dynamic_cast<Expr::Group*>(p)) {
      expr(x->e);
      x->ty = x->e->ty; x->lval = x->e->lval; x->isConst = x->e->isConst; x->cv = x->e->cv; x->typedConst = x->e->typedConst;
      return;
    }
    if (auto* x = dynamic_cast<Expr::Unary*>(p)) return unary(e, x);
    if (auto* x = dynamic_cast<Expr::Suffix*>(p)) return incdec(x->e, x, x->op);
    if (auto* x = dynamic_cast<Expr::Binary*>(p)) return binary(x);
    if (auto* x = dynamic_cast<Expr::Assign*>(p)) return assign(x);
    if (auto* x = dynamic_cast<Expr::Ternary*>(p)) return ternary(x);
    if (auto* x = dynamic_cast<Expr::Comma*>(p)) {
      expr(x->l); value(x->r);
      x->ty = x->r->ty;
      return;
    }
    if (auto* x = dynamic_cast<Expr::Cast*>(p)) return cast(x);
    if (auto* x = dynamic_cast<Expr::Sizeof*>(p)) {
      Type* t = x->of;
      if (!t) { expr(x->e); t = x->e->ty; }
      if (t->isVoid() || t->isFunc()) fail(x->tok, "sizeof of " + Types::str(t));
      x->ty = Types::U16(); x->isConst = true; x->cv = (int32_t)t->size();
      if (x->cv < 256) x->ty = Types::U8();
      return;
    }
    if (auto* x = dynamic_cast<Expr::Index*>(p)) return index(x);
    if (auto* x = dynamic_cast<Expr::Field*>(p)) return field(x);
    if (auto* x = dynamic_cast<Expr::Invoke*>(p)) return invoke(x);
    if (dynamic_cast<Expr::Array*>(p)) fail(e->tok, "initializer list is not an expression");
    fail(e->tok, "unsupported expression");
  }

  void literal(Expr::Literal* l) {
    if (l->kind == Expr::Literal::STR) {
      l->label = strLabel(l->bytes);
      l->ty = Types::array(Types::constOf(Types::U8()), (uint32_t)l->bytes.size() + 1);
      return;
    }
    if (l->value < -32768 || l->value > 65535) fail(l->tok, "constant " + std::to_string(l->value) + " does not fit 16 bits");
    l->isConst = true; l->cv = l->value;
    l->ty = valueType(l->value);
    if (l->forceUnsigned && l->ty->kind == Type::I16) l->ty = Types::U16();
  }

  void variable(Expr::Var* v) {
    Symbol* s = lookup(v->name);
    if (!s) fail(v->tok, "undeclared identifier '" + v->name + "'");
    if (s->kind == Symbol::FUNC) {                       // a function name used as a value is its address
      const auto& bd = s->fn->decl->bind;
      if (bd.bound && (!bd.in.empty() || !bd.out.empty() || s->fn->label[0] == '$' || s->fn->label.compare(0, 4, "RST ") == 0))
        fail(v->tok, "'" + v->name + "' uses its own register convention, a pointer to it would call it the wrong way");
      s->fn->used = true; s->used = true;
      v->sym = s; v->ty = Types::ptr(s->type); v->lval = false;
      return;
    }
    s->used = true;
    v->sym = s;
    v->ty = s->type;
    v->lval = !s->type->isArray() || true;
  }

  Type* promote2(expr_t& l, expr_t& r, bool isCompare) {
    Type *a = l->ty, *b = r->ty;
    Type* t;
    if (l->isConst && !l->typedConst && (!r->isConst || r->typedConst)) t = adopt(b, l->cv);
    else if (r->isConst && !r->typedConst && (!l->isConst || l->typedConst)) t = adopt(a, r->cv);
    else if (a->width() == b->width()) {
      if (a->isSigned() == b->isSigned()) t = a;
      else if (a->width() == 1) t = Types::I16();
      else t = Types::U16();
      if (a->isPtr()) t = Types::U16();
    } else t = a->width() > b->width() ? a : b;
    if (t->isPtr()) t = Types::U16();
    (void)isCompare;
    widenSlot(l, t);
    widenSlot(r, t);
    return t;
  }

  static Type* adopt(Type* other, int32_t cv) {
    if (other->isPtr()) return Types::U16();
    if (fits(cv, other)) return other;
    if (other->width() == 1) return cv > 32767 ? Types::U16() : Types::I16();
    return other;
  }

  expr_t scaled(expr_t idx, uint32_t size) {
    // index * element size as a 16 bit value
    Type* t = idx->ty->isSigned() ? Types::I16() : Types::U16();
    if (idx->isConst) {
      auto lit = std::make_shared<Expr::Literal>(Expr::Literal::NUM, (int32_t)((int64_t)idx->cv * size));
      lit->tok = idx->tok; lit->isConst = true; lit->cv = lit->value; lit->ty = Types::U16();
      if (lit->cv > 65535 || lit->cv < -32768) lit->cv &= 0xFFFF, lit->value = lit->cv;
      if (lit->cv < 0) lit->ty = Types::I16();
      return lit;
    }
    expr_t e = idx->ty->width() == 2 ? idx : castTo(idx, t);
    if (size == 1) return e;
    auto lit = std::make_shared<Expr::Literal>(Expr::Literal::NUM, (int32_t)size);
    lit->tok = idx->tok; lit->isConst = true; lit->cv = size; lit->ty = size < 256 ? Types::U8() : Types::U16();
    auto m = std::make_shared<Expr::Binary>(STAR, e, castTo(lit, t));
    m->tok = idx->tok; m->ty = m->opTy = t;
    return m;
  }

  void binary(Expr::Binary* b) {
    TokenT op = b->op;
    value(b->l);
    value(b->r);
    Type *lt = b->l->ty, *rt = b->r->ty;
    if (!lt->isScalar() || !rt->isScalar()) fail(b->tok, "invalid operands to binary operator: " + Types::str(lt) + " and " + Types::str(rt));

    if (op == OP_AND || op == OP_OR) {
      b->ty = Types::U8(); b->opTy = Types::U8();
      if (b->l->isConst && b->r->isConst) { b->isConst = true; b->cv = op == OP_AND ? (b->l->cv && b->r->cv) : (b->l->cv || b->r->cv); }
      return;
    }

    bool cmp = op == OP_EQUAL || op == OP_NOT_EQUAL || op == OP_LESS || op == OP_LESS_OR_EQUAL || op == OP_MORE || op == OP_MORE_OR_EQUAL;
    bool shift = op == BIT_LEFT_SHIFT || op == BIT_RIGHT_SHIFT;

    // pointer arithmetic
    if ((lt->isPtr() || rt->isPtr()) && !cmp) {
      if (op == OP_PLUS && lt->isPtr() && rt->isInt()) { ptrAdd(b, b->l, b->r, lt); return; }
      if (op == OP_PLUS && rt->isPtr() && lt->isInt()) { std::swap(b->l, b->r); ptrAdd(b, b->l, b->r, rt); return; }
      if (op == OP_MINUS && lt->isPtr() && rt->isInt()) { ptrAdd(b, b->l, b->r, lt); return; }
      if (op == OP_MINUS && lt->isPtr() && rt->isPtr()) {
        if (!Types::same(lt->base, rt->base)) fail(b->tok, "subtracting pointers of different types");
        uint32_t sz = lt->base->size();
        if (!sz) fail(b->tok, "pointer to void arithmetic");
        b->l = castTo(b->l, Types::U16()); b->r = castTo(b->r, Types::U16());
        b->ty = b->opTy = Types::U16();
        if (sz > 1) {
          auto lit = std::make_shared<Expr::Literal>(Expr::Literal::NUM, (int32_t)sz);
          lit->tok = b->tok; lit->isConst = true; lit->cv = sz; lit->ty = Types::U8();
          auto d = std::make_shared<Expr::Binary>(OP_DIV, nullptr, castTo(lit, Types::I16()));
          d->tok = b->tok;
          // wrap: (l - r) / size  -> build the division around this node by moving it into a copy
          auto inner = std::make_shared<Expr::Binary>(OP_MINUS, b->l, b->r);
          inner->tok = b->tok; inner->ty = inner->opTy = Types::I16();
          inner->l = castTo(inner->l, Types::I16()); inner->r = castTo(inner->r, Types::I16());
          d->l = inner; d->ty = d->opTy = Types::I16();
          b->op = OP_PLUS;                       // degenerate: (d) + 0
          b->l = d;
          auto zero = std::make_shared<Expr::Literal>(Expr::Literal::NUM, 0);
          zero->tok = b->tok; zero->isConst = true; zero->cv = 0; zero->ty = Types::I16();
          b->r = zero;
          b->ty = b->opTy = Types::I16();
        } else b->ty = Types::I16();
        return;
      }
      fail(b->tok, "invalid pointer arithmetic");
    }
    if (cmp && (lt->isPtr() || rt->isPtr())) {
      if (lt->isPtr() && rt->isPtr() && !Types::same(lt->base, rt->base) && !lt->base->isVoid() && !rt->base->isVoid())
        warn(b->tok, "comparison of distinct pointer types");
      Type* t = Types::U16();
      b->l = lt->width() == 2 ? b->l : castTo(b->l, t);
      b->r = rt->width() == 2 ? b->r : castTo(b->r, t);
      b->opTy = t; b->ty = Types::U8();
      return;
    }

    if (shift) {
      Type* t = lt;
      b->ty = b->opTy = t;
      // the count is a small unsigned value
      if (b->r->isConst) {
        if (b->r->cv < 0) err(b->tok, "negative shift count");
        if (b->r->cv >= (int)t->width() * 8) warn(b->tok, "shift count " + std::to_string(b->r->cv) + " is not smaller than the width of " + Types::str(t));
      } else if (rt->width() != 1 || rt->isSigned()) b->r = castTo(b->r, Types::U8());
    } else {
      Type* t = promote2(b->l, b->r, cmp);
      b->opTy = t;
      b->ty = cmp ? Types::U8() : t;
    }

    if (b->l->isConst && b->r->isConst) fold(b);
  }

  void ptrAdd(Expr::Binary* b, expr_t& p, expr_t& i, Type* pt) {
    uint32_t sz = pt->base->size();
    if (!sz) fail(b->tok, "arithmetic on a pointer to void/function");
    i = scaled(i, sz);
    b->ty = pt;
    b->opTy = Types::U16();
    if (i->ty->width() != 2) i = castTo(i, Types::U16());
  }

  void fold(Expr::Binary* b) {
    int32_t x = b->l->cv, y = b->r->cv, v = 0;
    switch (b->op) {
      case OP_PLUS: v = x + y; break;
      case OP_MINUS: v = x - y; break;
      case STAR: v = x * y; break;
      case OP_DIV: case OP_MOD:
        if (y == 0) fail(b->tok, "division by zero");
        v = b->op == OP_DIV ? x / y : x % y; break;
      case AMPERSAND: v = x & y; break;
      case BIT_OR: v = x | y; break;
      case BIT_XOR: v = x ^ y; break;
      case BIT_LEFT_SHIFT: v = (y < 0 || y > 31) ? 0 : (int32_t)((uint32_t)x << y); break;
      case BIT_RIGHT_SHIFT: v = (y < 0 || y > 31) ? 0 : x >> y; break;
      case OP_EQUAL: v = x == y; break;
      case OP_NOT_EQUAL: v = x != y; break;
      case OP_LESS: v = x < y; break;
      case OP_LESS_OR_EQUAL: v = x <= y; break;
      case OP_MORE: v = x > y; break;
      case OP_MORE_OR_EQUAL: v = x >= y; break;
      default: return;
    }
    b->isConst = true;
    b->cv = v;
    b->typedConst = b->l->typedConst || b->r->typedConst;
    if (b->typedConst && !(b->op >= OP_EQUAL && b->op <= OP_NOT_EQUAL) && b->ty->isInt()) { b->cv = truncTo(v, b->ty); return; }
    if (b->ty->kind != Type::U8 || v < 0 || v > 255) {
      if (v >= -32768 && v <= 65535) b->ty = (b->op >= OP_EQUAL && b->op <= OP_NOT_EQUAL) ? b->ty : (fits(v, b->ty) ? b->ty : valueType(v));
      else { b->cv = truncTo(v, Types::U16()); warn(b->tok, "constant expression overflows 16 bits"); b->ty = Types::U16(); }
    }
  }

  void unary(expr_t& slot, Expr::Unary* u) {
    switch (u->op) {
      case AMPERSAND: {
        expr(u->e);
        if (u->e->ty->isPtr() && u->e->ty->base->isFunc()) {       // &function == function
          if (auto* fv = dynamic_cast<Expr::Var*>(u->e.get())) if (fv->sym && fv->sym->kind == Symbol::FUNC) { slot = u->e; return; }
        }
        if (u->e->ty->isArray()) { u->ty = Types::ptr(u->e->ty->base, u->e->ty->base->isConst); return; }
        if (!u->e->lval) fail(u->tok, "cannot take the address of this expression");
        if (auto* v = dynamic_cast<Expr::Var*>(u->e.get())) if (v->sym->hasAddr || v->sym->kind == Symbol::PARAM) {}
        u->ty = Types::ptr(u->e->ty, u->e->ty->kind != Type::PTR && u->e->ty->isConst);
        return;
      }
      case STAR: {
        value(u->e);
        if (!u->e->ty->isPtr()) fail(u->tok, "cannot dereference " + Types::str(u->e->ty));
        if (u->e->ty->base->isVoid()) fail(u->tok, "dereferencing a void pointer");
        if (u->e->ty->base->isFunc()) { slot = u->e; return; }       // *fp == fp
        u->ty = u->e->ty->base;
        u->lval = true;
        return;
      }
      case OP_INC: case OP_DEC:
        return incdec(u->e, u, u->op);
      default: break;
    }
    value(u->e);
    Type* t = u->e->ty;
    if (!t->isScalar()) fail(u->tok, "invalid operand to unary operator: " + Types::str(t));
    if (u->op == OP_NOT) {
      u->ty = Types::U8();
      if (u->e->isConst) { u->isConst = true; u->cv = !u->e->cv; }
      return;
    }
    if (t->isPtr()) fail(u->tok, "invalid operand to unary operator: " + Types::str(t));
    u->ty = t;
    if (u->e->isConst) {
      u->isConst = true;
      int32_t v = u->e->cv;
      u->cv = u->op == OP_MINUS ? -v : u->op == BIT_NOT ? ~v : v;
      u->typedConst = u->e->typedConst;
      if (u->typedConst) { u->cv = truncTo(u->cv, t); return; }
      if (u->op == BIT_NOT && t->kind == Type::U8) u->cv &= 0xFF, u->ty = Types::U8();
      else if (u->op == BIT_NOT && t->width() == 2) u->cv = truncTo(u->cv, t);
      else u->ty = valueType(u->cv);
    }
  }

  void incdec(expr_t& target, Expression* node, TokenT op) {
    (void)op;
    expr(target);
    if (!target->lval) fail(node->tok, "operand of ++/-- must be a variable");
    if (Types::readOnly(target->ty)) fail(node->tok, "cannot modify a const object");
    if (!target->ty->isScalar()) fail(node->tok, "invalid operand to ++/--");
    node->ty = target->ty;
  }

  void assign(Expr::Assign* a) {
    expr(a->target);
    if (!a->target->lval) fail(a->tok, "left side of assignment is not a variable");
    if (a->target->ty->isArray()) fail(a->tok, "cannot assign to an array");
    if (Types::readOnly(a->target->ty)) fail(a->tok, "assignment of read-only object");
    Type* tt = a->target->ty;
    a->ty = tt;
    if (a->op == ASSIGN) {
      if (tt->isStruct()) {
        expr(a->value);
        if (!Types::same(tt, a->value->ty) || !a->value->lval) fail(a->tok, "struct assignment needs a struct of the same type");
        return;
      }
      coerce(a->value, tt, a->tok, "assignment");
      return;
    }
    // compound
    value(a->value);
    if (!tt->isScalar() || !a->value->ty->isScalar()) fail(a->tok, "invalid operands to compound assignment");
    if (tt->isPtr()) {
      if ((a->op != ASSIGN_PLUS && a->op != ASSIGN_MINUS) || !a->value->ty->isInt()) fail(a->tok, "invalid pointer compound assignment");
      uint32_t sz = tt->base->size();
      if (!sz) fail(a->tok, "arithmetic on a pointer to void");
      a->value = scaled(a->value, sz);
      a->opTy = Types::U16();
      return;
    }
    if (a->value->ty->isPtr()) fail(a->tok, "invalid operands to compound assignment");
    bool shift = a->op == ASSIGN_SHL || a->op == ASSIGN_SHR;
    if (shift) {
      a->opTy = tt;
      if (!a->value->isConst && (a->value->ty->width() != 1 || a->value->ty->isSigned())) a->value = castTo(a->value, Types::U8());
      return;
    }
    // the operation is done in the wider of both types, the result is truncated into the target
    expr_t dummy = a->target;                       // typing only, the target node must stay untouched
    Type* t;
    if (a->value->isConst) t = adopt(tt, a->value->cv);
    else if (tt->width() == a->value->ty->width()) {
      t = (tt->isSigned() == a->value->ty->isSigned()) ? tt : (tt->width() == 1 ? Types::I16() : Types::U16());
    } else t = tt->width() > a->value->ty->width() ? tt : a->value->ty;
    widenSlot(a->value, t);
    a->opTy = t;
  }

  void ternary(Expr::Ternary* t) {
    cond(t->cond);
    value(t->a); value(t->b);
    Type *x = t->a->ty, *y = t->b->ty;
    if (x->isVoid() && y->isVoid()) { t->ty = x; return; }
    if (x->isPtr() || y->isPtr()) {
      Type* p = x->isPtr() ? x : y;
      coerce(t->a, p, t->tok, "?:"); coerce(t->b, p, t->tok, "?:");
      t->ty = p;
      return;
    }
    if (!x->isInt() || !y->isInt()) fail(t->tok, "?: arms have incompatible types " + Types::str(x) + " and " + Types::str(y));
    t->ty = promote2(t->a, t->b, false);
    if (t->cond->isConst && t->a->isConst && t->b->isConst) { t->isConst = true; t->cv = t->cond->cv ? t->a->cv : t->b->cv; }
  }

  void cast(Expr::Cast* c) {
    Type* to = c->to;
    value(c->e);
    Type* from = c->e->ty;
    c->ty = to;
    if (to->isVoid()) return;
    if (!to->isScalar() || !from->isScalar()) fail(c->tok, "cannot cast " + Types::str(from) + " to " + Types::str(to));
    if (c->e->isConst) { c->isConst = true; c->typedConst = true; c->cv = truncTo(c->e->cv, to); return; }
    if (from->isInt() && to->isInt() && from->width() < to->width() && retypable(c->e.get())) retype(c->e, to);
  }

  void index(Expr::Index* x) {
    value(x->base);
    value(x->index);
    Type* bt = x->base->ty;
    if (!bt->isPtr() && x->index->ty->isPtr()) { std::swap(x->base, x->index); bt = x->base->ty; }
    if (!bt->isPtr()) fail(x->tok, "subscripted value is neither array nor pointer: " + Types::str(bt));
    if (!x->index->ty->isInt()) fail(x->tok, "array index is not an integer");
    if (bt->base->isVoid()) fail(x->tok, "subscript of a void pointer");
    uint32_t sz = bt->base->size();
    x->index = scaled(x->index, sz);
    x->ty = bt->base;
    x->lval = true;
    // bounds check of constant indices on real arrays
    if (auto* c = dynamic_cast<Expr::Cast*>(x->base.get())) {
      if (c->e->ty->isArray() && x->index->isConst && sz && (uint32_t)x->index->cv / sz >= c->e->ty->count && c->e->ty->count)
        warn(x->tok, "array index " + std::to_string(x->index->cv / (int)sz) + " is out of bounds (size " + std::to_string(c->e->ty->count) + ")");
    }
  }

  void field(Expr::Field* f) {
    Type* st;
    if (f->arrow) {
      value(f->base);
      if (!f->base->ty->isPtr() || !f->base->ty->base->isStruct()) fail(f->tok, "'->' needs a pointer to a struct, not " + Types::str(f->base->ty));
      st = f->base->ty->base;
      f->lval = true;
    } else {
      expr(f->base);
      if (!f->base->ty->isStruct()) fail(f->tok, "'.' needs a struct, not " + Types::str(f->base->ty) + " (use '->' for pointers)");
      st = f->base->ty;
      f->lval = f->base->lval;
    }
    if (!st->sdef->complete) fail(f->tok, "struct " + st->sdef->tag + " is incomplete");
    const Member* m = st->sdef->find(f->name);
    if (!m) fail(f->tok, "struct " + st->sdef->tag + " has no member '" + f->name + "'");
    f->member = m;
    f->ty = m->type;
    if (st->isConst && !m->type->isConst) f->ty = Types::constOf(m->type);
  }

  void invoke(Expr::Invoke* c) {
    Expression* ce = c->callee.get();
    while (auto* g = dynamic_cast<Expr::Group*>(ce)) ce = g->e.get();
    auto* v = dynamic_cast<Expr::Var*>(ce);
    Symbol* s = v ? lookup(v->name) : nullptr;
    if (!v || (s && s->kind != Symbol::FUNC)) {              // call through a pointer
      value(c->callee);
      Type* ft = c->callee->ty;
      if (!(ft->isPtr() && ft->base->isFunc())) fail(c->tok, "called object is not a function (" + Types::str(ft) + ")");
      ft = ft->base;
      if (fn) fn->hasCalls = true;
      if (c->args.size() != ft->params.size())
        fail(c->tok, "call expects " + std::to_string(ft->params.size()) + " argument(s), got " + std::to_string(c->args.size()));
      if (ft->params.size() > 3) fail(c->tok, "more than 3 parameters");
      for (size_t i = 0; i < c->args.size(); i++) coerce(c->args[i], ft->params[i], c->args[i]->tok ? c->args[i]->tok : c->tok, "argument " + std::to_string(i + 1));
      c->fnType = ft;
      c->ty = ft->base;
      return;
    }
    if (!s) fail(v->tok, "call to undeclared function '" + v->name + "'");
    FuncInfo* f = s->fn;
    f->used = true;
    v->sym = s; v->ty = s->type;
    if (fn) fn->hasCalls = true;
    if (c->args.size() != f->type->params.size())
      fail(c->tok, "'" + v->name + "' expects " + std::to_string(f->type->params.size()) + " argument(s), got " + std::to_string(c->args.size()));
    for (size_t i = 0; i < c->args.size(); i++) coerce(c->args[i], f->type->params[i], c->args[i]->tok ? c->args[i]->tok : c->tok, "argument " + std::to_string(i + 1) + " of '" + v->name + "'");
    c->fn = f;
    c->ty = f->ret;
  }
};
};
