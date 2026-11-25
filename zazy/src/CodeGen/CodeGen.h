#pragma once
#include "src/Sema/Sema.h"
#include "Ins.h"
#include "Runtime.h"

namespace Zazy {

/**
 * Z80 code generator.
 *
 * Model: an 8 bit value lives in reg A, a 16 bit value in reg HL ("the accumulator"). A second
 * operand is brought into reg E / reg DE. Variables are addressed absolutely (globals) or
 * through reg IX (locals and spilled parameters, positive offsets only, IX = SP after the
 * prologue). Everything the accumulator model cannot hold is pushed.
 *
 * Calls: arg 1 / 2 / 3 go to A,E,C (8 bit) or HL,DE,BC (16 bit), the result comes back in A / HL,
 * every register but IX is destroyed. `extern` functions with an asm("...") binding use the
 * registers their hand written routine expects instead.
 */
class CodeGen {
public:
  struct Options {
    bool bare = false;                     // no Init.z80, own tiny reset code (used by the tests)
    bool comments = true;
    std::string initPath = "lib/Init.z80"; // INCLUDE path of the Init.z80 in init mode
    std::string mathPath = "lib/utils/Math.z80"; // INCLUDE path of Math.z80 (@MUL16x8 / @DIV8 / @DIV16x8), added when the generated code calls them
    uint32_t dataBase = 0x8000;            // where global variables live
    std::string source = "";               // shown in the header
  };

  CodeGen(Sema& s, const Options& o): sema(s), opt(o) {}

  std::string generate(std::vector<stmt_t>& prog, const std::vector<std::string>& asmIncludes);

private:
  Sema& sema;
  Options opt;

  std::string out;
  std::vector<Ins> code;
  std::set<std::string> rt;                // used runtime routines
  FuncInfo* fn = nullptr;
  std::string fnBase;
  int lblSeq = 0;
  std::string retLabel;
  std::vector<std::string> breakL, contL;
  std::map<Stmt::Case*, std::string> caseLabel;
  bool needInit = false;
  bool hasSetup = false, hasMain = false;

  // ======================================================================== small helpers
  template <class T, class B> static T* as(B* p) { return dynamic_cast<T*>(p); }

  static std::string hex8(int v) { char b[16]; snprintf(b, sizeof b, "0x%02X", v & 0xFF); return b; }
  static std::string hex16(int v) { char b[16]; snprintf(b, sizeof b, "0x%04X", v & 0xFFFF); return b; }
  static std::string n8(int v) { v &= 0xFF; return v < 10 ? std::to_string(v) : hex8(v); }
  static std::string n16(int v) { v &= 0xFFFF; return v < 10 ? std::to_string(v) : v < 256 ? hex8(v) : hex16(v); }
  static std::string idx(int off) { return "(IX+" + std::to_string(off) + ")"; }
  static int W(const Type* t) { return t->isScalar() ? (int)t->width() : 0; }

  void I(const std::string& op, const std::string& a = "", const std::string& b = "", const std::string& c = "") {
    Ins i; i.op = op; i.a = a; i.b = b; if (opt.comments) i.cmt = c;
    code.push_back(i);
  }
  void L(const std::string& name) { Ins i; i.kind = Ins::LABEL; i.op = name; code.push_back(i); }
  void J(const std::string& cc, const std::string& target) { Ins i; i.kind = Ins::BRANCH; i.op = "JP"; i.a = cc; i.b = target; code.push_back(i); }
  void DJ(const std::string& target) { Ins i; i.kind = Ins::BRANCH; i.op = "DJNZ"; i.b = target; code.push_back(i); }
  void RAW(const std::string& t) { Ins i; i.kind = Ins::RAW; i.op = t; code.push_back(i); }
  std::string newL(const char* tag = "L") { return "@" + fnBase + "__" + tag + std::to_string(lblSeq++); }

  [[noreturn]] void internal(const std::string& msg) { throw std::runtime_error("internal compiler error: " + msg); }

  // ======================================================================== places
  struct Place {
    enum K { NONE, ABS, FRAME } k = NONE;
    std::string base;      // ABS: label, empty for an absolute address
    int off = 0;
  };

  static std::string absText(const Place& p, int extra = 0) {
    int o = p.off + extra;
    if (p.base.empty()) return hex16(o);
    if (!o) return p.base;
    return p.base + "+" + std::to_string(o);
  }

  Expression* strip(Expression* e) {
    for (;;) {
      if (auto* g = as<Expr::Group>(e)) { e = g->e.get(); continue; }
      if (auto* c = as<Expr::Cast>(e)) {
        if (!c->isConst && c->e->ty->isScalar() && c->ty->isScalar() && W(c->ty) == W(c->e->ty)) { e = c->e.get(); continue; }
      }
      return e;
    }
  }

  bool staticPlace(Expression* e, Place& p, bool limit = true) {
    e = stripGroup(e);
    if (auto* v = as<Expr::Var>(e)) {
      Symbol* s = v->sym;
      if (!s) return false;
      if (s->kind == Symbol::GLOBAL) {
        p.k = Place::ABS;
        if (s->hasAddr) { p.base = ""; p.off = (int)s->addr; } else { p.base = s->label; p.off = 0; }
        return true;
      }
      if (s->kind == Symbol::LOCAL || s->kind == Symbol::PARAM) {
        p.k = Place::FRAME; p.off = s->off;
        return !(limit && p.off + (int)e->ty->size() > 128);
      }
      return false;
    }
    if (auto* f = as<Expr::Field>(e)) {
      if (f->arrow || !staticPlace(f->base.get(), p, false)) return false;
      p.off += (int)f->member->offset;
      return !(limit && p.k == Place::FRAME && p.off + (int)e->ty->size() > 128);
    }
    if (auto* ix = as<Expr::Index>(e)) {
      if (!ix->index->isConst) return false;
      auto* c = as<Expr::Cast>(ix->base.get());
      if (!c || !c->e->ty->isArray() || !staticPlace(c->e.get(), p, false)) return false;
      p.off += ix->index->cv;
      return !(limit && p.k == Place::FRAME && p.off + (int)e->ty->size() > 128);
    }
    return false;
  }

  Expression* stripGroup(Expression* e) {
    while (auto* g = as<Expr::Group>(e)) e = g->e.get();
    return e;
  }

  // a link time constant address: &global, global array, string literal
  bool addrConst(Expression* e, std::string& text) {
    e = stripGroup(e);
    if (auto* fv = as<Expr::Var>(e)) {                       // function name: its label
      if (fv->sym && fv->sym->kind == Symbol::FUNC) { text = fv->sym->fn->label; return true; }
      return false;
    }
    if (auto* c = as<Expr::Cast>(e)) {
      if (c->e->ty->isArray()) {
        Expression* in = stripGroup(c->e.get());
        if (auto* l = as<Expr::Literal>(in)) { if (l->kind == Expr::Literal::STR) { text = "_zs" + std::to_string(l->label); return true; } }
        Place p;
        if (staticPlace(in, p, false) && p.k == Place::ABS) { text = absText(p); return true; }
        return false;
      }
      if (c->e->ty->isPtr()) return addrConst(c->e.get(), text);
      return false;
    }
    if (auto* u = as<Expr::Unary>(e)) {
      if (u->op != AMPERSAND) return false;
      Place p;
      if (staticPlace(u->e.get(), p, false) && p.k == Place::ABS) { text = absText(p); return true; }
    }
    return false;
  }

  bool simple8(Expression* e) {
    if (e->isConst) return true;
    e = strip(e);
    Place p;
    return W(e->ty) == 1 && staticPlace(e, p);
  }
  bool simple16(Expression* e) {
    if (e->isConst) return true;
    std::string t;
    if (addrConst(e, t)) return true;
    e = strip(e);
    Place p;
    return W(e->ty) == 2 && staticPlace(e, p);
  }
  bool simple(Expression* e, int w) { return w == 1 ? simple8(e) : simple16(e); }

  // ======================================================================== loads / stores
  void loadConst(int32_t v, int w) {
    if (w == 1) { v &= 0xFF; if (!v) I("XOR", "A"); else I("LD", "A", n8(v)); }
    else I("LD", "HL", n16(v));
  }

  void loadPlace(const Place& p, int w) {
    if (w == 1) {
      if (p.k == Place::FRAME) I("LD", "A", idx(p.off));
      else I("LD", "A", "(" + absText(p) + ")");
    } else {
      if (p.k == Place::FRAME) { I("LD", "L", idx(p.off)); I("LD", "H", idx(p.off + 1)); }
      else I("LD", "HL", "(" + absText(p) + ")");
    }
  }

  void storePlace(const Place& p, int w) {
    if (w == 1) {
      if (p.k == Place::FRAME) I("LD", idx(p.off), "A");
      else I("LD", "(" + absText(p) + ")", "A");
    } else {
      if (p.k == Place::FRAME) { I("LD", idx(p.off), "L"); I("LD", idx(p.off + 1), "H"); }
      else I("LD", "(" + absText(p) + ")", "HL");
    }
  }

  void loadHL(int w) {                         // value at (HL) -> acc
    if (w == 1) I("LD", "A", "(HL)");
    else I("RST", "0x20", "", "Load the word at (HL) to reg HL");
  }

  // load a 16 bit value into a register pair (HL, DE, BC) without touching anything else
  void loadPair(Expression* e, const std::string& pr) {
    static const std::map<std::string, std::pair<std::string, std::string>> lh = {{"HL", {"L", "H"}}, {"DE", {"E", "D"}}, {"BC", {"C", "B"}}};
    if (e->isConst) { I("LD", pr, n16(e->cv)); return; }
    std::string t;
    if (addrConst(e, t)) { I("LD", pr, t); return; }
    Place p;
    if (!staticPlace(strip(e), p)) internal("loadPair on a non simple expression");
    if (p.k == Place::FRAME) { I("LD", lh.at(pr).first, idx(p.off)); I("LD", lh.at(pr).second, idx(p.off + 1)); }
    else I("LD", pr, "(" + absText(p) + ")");
  }

  void loadDE(Expression* e) { loadPair(e, "DE"); }

  // operand of an 8 bit ALU instruction; may emit LD HL,addr first
  std::string operand8(Expression* e) {
    if (e->isConst) return n8(e->cv);
    Place p;
    if (!staticPlace(strip(e), p)) internal("operand8 on a non simple expression");
    if (p.k == Place::FRAME) return idx(p.off);
    I("LD", "HL", absText(p));
    return "(HL)";
  }

  void addConst(int32_t k) {                    // HL += k, clobbers DE when big
    k = (int16_t)k;
    if (k == 0) return;
    if (k > 0 && k <= 3) { for (int i = 0; i < k; i++) I("INC", "HL"); return; }
    if (k < 0 && k >= -3) { for (int i = 0; i < -k; i++) I("DEC", "HL"); return; }
    I("LD", "DE", n16(k));
    I("ADD", "HL", "DE");
  }

  void convert(const Type* from, const Type* to) {
    int fw = from->isScalar() ? (int)from->width() : 2, tw = to->isScalar() ? (int)to->width() : 2;
    if (fw == tw || to->isVoid()) return;
    if (fw == 1) {
      if (from->isSigned()) { I("LD", "L", "A"); I("ADD", "A", "A", "Extend the sign"); I("SBC", "A", "A"); I("LD", "H", "A"); }
      else { I("LD", "L", "A"); I("LD", "H", "0"); }
    } else I("LD", "A", "L");
  }

  // ======================================================================== addresses
  void addrOfPlace(const Place& p) {
    if (p.k == Place::ABS) { I("LD", "HL", absText(p)); return; }
    I("PUSH", "IX"); I("POP", "HL");
    addConst(p.off);
  }

  void genAddr(Expression* e) {                // HL = address of the lvalue
    e = stripGroup(e);
    Place p;
    if (staticPlace(e, p, false)) { addrOfPlace(p); return; }
    if (auto* c = as<Expr::Cast>(e)) {
      if (c->e->ty->isArray()) { genAddr(c->e.get()); return; }
    }
    if (auto* u = as<Expr::Unary>(e)) {
      if (u->op == STAR) { gen(u->e.get()); return; }
    }
    if (auto* ix = as<Expr::Index>(e)) { genIndexAddr(ix); return; }
    if (auto* f = as<Expr::Field>(e)) {
      if (f->arrow) gen(f->base.get()); else genAddr(f->base.get());
      addConst((int32_t)f->member->offset);
      return;
    }
    if (auto* l = as<Expr::Literal>(e)) {
      if (l->kind == Expr::Literal::STR) { I("LD", "HL", "_zs" + std::to_string(l->label)); return; }
    }
    internal("cannot take the address of this expression");
  }

  void genIndexAddr(Expr::Index* ix) {
    Expression* idxE = ix->index.get();
    if (idxE->isConst) { gen(ix->base.get()); addConst(idxE->cv); return; }
    if (simple16(ix->base.get())) { gen(idxE); loadDE(ix->base.get()); I("ADD", "HL", "DE"); return; }
    gen(ix->base.get());
    I("PUSH", "HL");
    gen(idxE);
    I("POP", "DE");
    I("ADD", "HL", "DE");
  }

  // ======================================================================== expressions
  void gen(Expression* e, bool discard = false) {
    if (e->isConst && e->ty->isScalar()) { loadConst(e->cv, W(e->ty)); return; }

    if (auto* g = as<Expr::Group>(e)) { gen(g->e.get(), discard); return; }
    { std::string lbl; if (as<Expr::Var>(e) && addrConst(e, lbl)) { I("LD", "HL", lbl); return; } }
    if (as<Expr::Var>(e) || as<Expr::Index>(e) || as<Expr::Field>(e)) { genLoad(e); return; }
    if (auto* c = as<Expr::Cast>(e)) { genCast(c); return; }
    if (auto* u = as<Expr::Unary>(e)) { genUnary(u, discard); return; }
    if (auto* s = as<Expr::Suffix>(e)) { genIncDec(s->e.get(), s->op == OP_INC, false, discard, s->ty); return; }
    if (auto* b = as<Expr::Binary>(e)) { genBinary(b); return; }
    if (auto* a = as<Expr::Assign>(e)) { genAssign(a, discard); return; }
    if (auto* t = as<Expr::Ternary>(e)) {
      std::string le = newL(), lend = newL();
      branch(t->cond.get(), le, false);
      gen(t->a.get(), discard);
      J("", lend);
      L(le);
      gen(t->b.get(), discard);
      L(lend);
      return;
    }
    if (auto* c = as<Expr::Comma>(e)) { gen(c->l.get(), true); gen(c->r.get(), discard); return; }
    if (auto* c = as<Expr::Invoke>(e)) { genCall(c); return; }
    if (auto* l = as<Expr::Literal>(e)) {
      if (l->kind == Expr::Literal::STR) { I("LD", "HL", "_zs" + std::to_string(l->label)); return; }
    }
    internal("unhandled expression");
  }

  void genLoad(Expression* e) {
    int w = W(e->ty);
    if (!w) internal("load of a non scalar value");
    Place p;
    if (staticPlace(e, p)) { loadPlace(p, w); return; }
    genAddr(e);
    loadHL(w);
  }

  void genCast(Expr::Cast* c) {
    if (c->ty->isVoid()) { gen(c->e.get(), true); return; }
    if (c->e->ty->isArray()) {
      std::string t;
      if (addrConst(c, t)) I("LD", "HL", t);
      else genAddr(c->e.get());
      return;
    }
    gen(c->e.get());
    convert(c->e->ty, c->ty);
  }

  void genUnary(Expr::Unary* u, bool discard) {
    switch (u->op) {
      case AMPERSAND: {
        std::string t;
        if (addrConst(u, t)) I("LD", "HL", t); else genAddr(u->e.get());
        return;
      }
      case STAR: genLoad(u); return;
      case OP_INC: case OP_DEC: genIncDec(u->e.get(), u->op == OP_INC, true, discard, u->ty); return;
      case OP_NOT: genBool(u); return;
      case OP_PLUS: gen(u->e.get()); return;
      case OP_MINUS:
        gen(u->e.get());
        if (W(u->ty) == 1) I("NEG");
        else { I("XOR", "A"); I("SUB", "L"); I("LD", "L", "A"); I("SBC", "A", "A"); I("SUB", "H"); I("LD", "H", "A"); }
        return;
      case BIT_NOT:
        gen(u->e.get());
        if (W(u->ty) == 1) I("CPL");
        else { I("LD", "A", "L"); I("CPL"); I("LD", "L", "A"); I("LD", "A", "H"); I("CPL"); I("LD", "H", "A"); }
        return;
      default: internal("unary operator");
    }
  }

  void genBool(Expression* e) {                // 0 / 1 in reg A
    std::string lt = newL(), lend = newL();
    branch(e, lt, true);
    I("XOR", "A");
    J("", lend);
    L(lt);
    I("LD", "A", "1");
    L(lend);
  }

  // ---------------------------------------------------------------- binary
  static bool isCmp(TokenT op) {
    return op == OP_EQUAL || op == OP_NOT_EQUAL || op == OP_LESS || op == OP_LESS_OR_EQUAL || op == OP_MORE || op == OP_MORE_OR_EQUAL;
  }

  void genBinary(Expr::Binary* b) {
    TokenT op = b->op;
    if (isCmp(op) || op == OP_AND || op == OP_OR) { genBool(b); return; }
    switch (op) {
      case STAR: genMul(b); return;
      case OP_DIV: case OP_MOD: genDivMod(b); return;
      case BIT_LEFT_SHIFT: case BIT_RIGHT_SHIFT: genShift(b); return;
      default: break;
    }
    genAddSub(b);
  }

  void op8(TokenT op, const std::string& x) {
    switch (op) {
      case OP_PLUS: I("ADD", "A", x); break;
      case OP_MINUS: I("SUB", x); break;
      case AMPERSAND: I("AND", x); break;
      case BIT_OR: I("OR", x); break;
      case BIT_XOR: I("XOR", x); break;
      default: internal("op8");
    }
  }

  void byteOp(TokenT op, const std::string& reg, int v) {
    v &= 0xFF;
    if (op == AMPERSAND) {
      if (v == 0xFF) return;
      if (v == 0) { I("LD", reg, "0"); return; }
    } else if (op == BIT_OR) {
      if (v == 0) return;
      if (v == 0xFF) { I("LD", reg, "0xFF"); return; }
    } else if (v == 0) return;
    I("LD", "A", reg);
    op8(op, n8(v));
    I("LD", reg, "A");
  }

  void genAddSub(Expr::Binary* b) {
    TokenT op = b->op;
    Expression *l = b->l.get(), *r = b->r.get();
    int w = W(b->opTy);
    bool commut = op != OP_MINUS;

    if (w == 1) {
      if (r->isConst) {
        gen(l);
        int v = r->cv & 0xFF;
        if (op == OP_PLUS && v == 1) I("INC", "A");
        else if (op == OP_MINUS && v == 1) I("DEC", "A");
        else if ((op == OP_PLUS || op == OP_MINUS || op == BIT_OR || op == BIT_XOR) && v == 0) {}
        else if (op == AMPERSAND && v == 0xFF) {}
        else if (op == AMPERSAND && v == 0) I("XOR", "A");
        else op8(op, n8(v));
      } else if (simple8(r)) { gen(l); op8(op, operand8(r)); }
      else if (commut && simple8(l)) { gen(r); op8(op, operand8(l)); }
      else {
        gen(l); I("PUSH", "AF");
        gen(r); I("LD", "E", "A"); I("POP", "AF");
        op8(op, "E");
      }
      return;
    }

    // 16 bit
    if (r->isConst || (commut && l->isConst)) {
      Expression* k = r->isConst ? r : l;
      Expression* v = r->isConst ? l : r;
      gen(v);
      int32_t c = k->cv & 0xFFFF;
      if (op == OP_PLUS) addConst(c);
      else if (op == OP_MINUS) addConst(-c);
      else { byteOp(op, "H", c >> 8); byteOp(op, "L", c); }
      return;
    }
    if (simple16(r)) { gen(l); loadDE(r); }
    else if (commut && simple16(l)) { gen(r); loadDE(l); }
    else { gen(l); I("PUSH", "HL"); gen(r); I("EX", "DE", "HL"); I("POP", "HL"); }
    op16(op);
  }

  void op16(TokenT op) {                       // HL = HL op DE
    switch (op) {
      case OP_PLUS: I("ADD", "HL", "DE"); break;
      case OP_MINUS: I("OR", "A"); I("SBC", "HL", "DE"); break;
      case AMPERSAND: case BIT_OR: case BIT_XOR: {
        I("LD", "A", "H"); op8(op, "D"); I("LD", "H", "A");
        I("LD", "A", "L"); op8(op, "E"); I("LD", "L", "A");
        break;
      }
      default: internal("op16");
    }
  }

  // second operand into E / DE, first operand is already in the accumulator
  void second(Expression* r, int w) {
    if (w == 1) {
      if (simple8(r)) { I("LD", "E", operand8(r)); return; }
      I("PUSH", "AF"); gen(r); I("LD", "E", "A"); I("POP", "AF");
    } else {
      if (simple16(r)) { loadDE(r); return; }
      I("PUSH", "HL"); gen(r); I("EX", "DE", "HL"); I("POP", "HL");
    }
  }

  static int log2exact(int32_t v) {
    if (v <= 0 || (v & (v - 1))) return -1;
    int k = 0;
    while ((1 << k) != v) k++;
    return k;
  }

  // u8 value zero extended to 16 bit ?
  Expression* zext8(Expression* e) {
    if (e->isConst) return (e->cv >= 0 && e->cv <= 255) ? e : nullptr;
    e = stripGroup(e);
    if (auto* c = as<Expr::Cast>(e)) {
      if (!c->e->ty->isArray() && c->e->ty->isInt() && W(c->e->ty) == 1 && !c->e->ty->isSigned() && W(c->ty) == 2) return c->e.get();
    }
    return nullptr;
  }

  void genMul(Expr::Binary* b) {
    Expression *l = b->l.get(), *r = b->r.get();
    int w = W(b->opTy);
    if (l->isConst && !r->isConst) std::swap(l, r);

    if (r->isConst) {
      int32_t k = r->cv & (w == 1 ? 0xFF : 0xFFFF);
      gen(l);
      if (k == 0) { loadConst(0, w); return; }
      if (k == 1) return;
      int top = 0;
      while ((k >> (top + 1)) != 0) top++;
      int pop = 0;
      for (int i = 0; i <= top; i++) pop += (k >> i) & 1;
      if (top + pop <= 14) {
        // Horner: acc = x; for every lower bit: acc <<= 1, + x when set
        if (pop > 1) { if (w == 1) I("LD", "D", "A"); else { I("LD", "D", "H"); I("LD", "E", "L"); } }
        for (int i = top - 1; i >= 0; i--) {
          if (w == 1) I("ADD", "A", "A"); else I("ADD", "HL", "HL");
          if ((k >> i) & 1) { if (w == 1) I("ADD", "A", "D"); else I("ADD", "HL", "DE"); }
        }
        return;
      }
      // generic
      if (w == 1) { I("LD", "E", n8(k)); rt.insert("@ZZ_MUL8"); I("CALL", "@ZZ_MUL8"); }
      else { I("LD", "DE", n16(k)); rt.insert("@ZZ_MUL16"); I("CALL", "@ZZ_MUL16"); }
      return;
    }

    if (w == 2) {
      // both operands are bytes: the product fits 16 bits and the existing @MUL16x8 is exact
      Expression *zl = zext8(l), *zr = zext8(r);
      if (zl && zr) {
        if (simple8(zr)) { gen(zl); I("LD", "L", "A"); I("LD", "H", "0"); loadA(zr); }
        else {
          gen(zr); I("PUSH", "AF");
          gen(zl); I("LD", "L", "A"); I("LD", "H", "0"); I("POP", "AF");
        }
        I("CALL", "@MUL16x8", "", "Use the hand written 16x8 multiplication");
        usesLib = true;
        return;
      }
    }

    gen(l);
    second(r, w);
    if (w == 1) { rt.insert("@ZZ_MUL8"); I("CALL", "@ZZ_MUL8"); }
    else { rt.insert("@ZZ_MUL16"); I("CALL", "@ZZ_MUL16"); }
  }

  bool usesLib = false;

  void loadA(Expression* e) {                  // simple 8 bit expression -> reg A, nothing else touched
    if (e->isConst) { loadConst(e->cv, 1); return; }
    Place p;
    if (!staticPlace(strip(e), p)) internal("loadA");
    loadPlace(p, 1);
  }

  void genDivMod(Expr::Binary* b) {
    Expression *l = b->l.get(), *r = b->r.get();
    bool isDiv = b->op == OP_DIV;
    Type* t = b->opTy;
    int w = W(t);
    bool sgn = t->isSigned();

    if (r->isConst && !sgn && r->cv > 0) {
      int32_t k = r->cv;
      int sh = log2exact(k);
      if (sh >= 0) {                           // power of two
        gen(l);
        if (isDiv) shiftRight(w, sh, false);
        else if (w == 1) I("AND", n8(k - 1));
        else if (k <= 256) { I("LD", "H", "0"); I("LD", "A", "L"); I("AND", n8(k - 1)); I("LD", "L", "A"); }
        else { I("LD", "A", "H"); I("AND", n8((k - 1) >> 8)); I("LD", "H", "A"); }
        return;
      }
      if (w == 1 || k <= 127) {                // @DIV8 is exact for every 8 bit divisor, @DIV16x8 only up to 127 (checked on the emulator)
        gen(l);
        usesLib = true;
        if (w == 1) {
          I("LD", "D", "A"); I("LD", "E", n8(k));
          I("CALL", "@DIV8", "", "Use the hand written 8 bit division");
          if (isDiv) I("LD", "A", "D");
        } else {
          I("LD", "A", n8(k));
          I("CALL", "@DIV16x8", "", "Use the hand written 16x8 division");
          if (!isDiv) { I("LD", "L", "A"); I("LD", "H", "0"); }
        }
        return;
      }
    }

    // generic: always in 16 bit
    gen(l);
    if (w == 1) convert(t, sgn ? Types::I16() : Types::U16());
    if (r->isConst) {
      I("LD", "DE", n16(w == 1 && sgn ? (int)(int8_t)r->cv : r->cv));
    } else {
      I("PUSH", "HL");
      gen(r);
      if (w == 1) convert(t, sgn ? Types::I16() : Types::U16());
      I("EX", "DE", "HL");
      I("POP", "HL");
    }
    if (sgn) { rt.insert("@ZZ_DIVS16"); I("CALL", "@ZZ_DIVS16"); }
    else { rt.insert("@ZZ_DIVU16"); I("CALL", "@ZZ_DIVU16"); }
    if (!isDiv) I("EX", "DE", "HL");
    if (w == 1) I("LD", "A", "L");
  }

  void shiftRight(int w, int k, bool sgn) {
    if (k <= 0) return;
    if (w == 1) {
      if (k >= 8) { if (sgn) { I("ADD", "A", "A"); I("SBC", "A", "A"); } else I("XOR", "A"); return; }
      for (int i = 0; i < k; i++) I(sgn ? "SRA" : "SRL", "A");
      return;
    }
    if (k >= 16) {
      if (sgn) { I("LD", "A", "H"); I("ADD", "A", "A"); I("SBC", "A", "A"); I("LD", "H", "A"); I("LD", "L", "A"); }
      else I("LD", "HL", "0");
      return;
    }
    if (k >= 8) {
      I("LD", "L", "H");
      if (sgn) { I("LD", "A", "H"); I("ADD", "A", "A"); I("SBC", "A", "A"); I("LD", "H", "A"); }
      else I("LD", "H", "0");
      for (int i = 8; i < k; i++) I(sgn ? "SRA" : "SRL", "L");
      return;
    }
    for (int i = 0; i < k; i++) { I(sgn ? "SRA" : "SRL", "H"); I("RR", "L"); }
  }

  void shiftLeft(int w, int k) {
    if (k <= 0) return;
    if (w == 1) {
      if (k >= 8) { I("XOR", "A"); return; }
      for (int i = 0; i < k; i++) I("ADD", "A", "A");
      return;
    }
    if (k >= 16) { I("LD", "HL", "0"); return; }
    if (k >= 8) { I("LD", "H", "L"); I("LD", "L", "0"); k -= 8; }
    for (int i = 0; i < k; i++) I("ADD", "HL", "HL");
  }

  void genShift(Expr::Binary* b) {
    Expression *l = b->l.get(), *r = b->r.get();
    int w = W(b->opTy);
    bool left = b->op == BIT_LEFT_SHIFT;
    bool sgn = b->opTy->isSigned();
    gen(l);
    if (r->isConst) {
      if (left) shiftLeft(w, r->cv); else shiftRight(w, r->cv, sgn);
      return;
    }
    // variable count in reg B
    if (simple8(r) && w == 1) {
      std::string o = operand8(r);                 // may use HL, fine for 8 bit values
      I("LD", "B", o);
    } else if (r->ty && simple8(r) && w == 2) {
      Place p;
      if (staticPlace(strip(r), p) && p.k == Place::FRAME) I("LD", "B", idx(p.off));
      else { pushAcc(w); gen(r); I("LD", "B", "A"); popAcc(w); }
    } else { pushAcc(w); gen(r); I("LD", "B", "A"); popAcc(w); }
    std::string chk = newL("sh"), lp = newL("sh");
    I("INC", "B");
    J("", chk);
    L(lp);
    if (left) { if (w == 1) I("ADD", "A", "A"); else I("ADD", "HL", "HL"); }
    else if (w == 1) I(sgn ? "SRA" : "SRL", "A");
    else { I(sgn ? "SRA" : "SRL", "H"); I("RR", "L"); }
    L(chk);
    DJ(lp);
  }

  void pushAcc(int w) { I("PUSH", w == 1 ? "AF" : "HL"); }
  void popAcc(int w) { I("POP", w == 1 ? "AF" : "HL"); }

  // ---------------------------------------------------------------- conditions
  static std::string inv(const std::string& cc) {
    if (cc == "Z") return "NZ";
    if (cc == "NZ") return "Z";
    if (cc == "C") return "NC";
    if (cc == "NC") return "C";
    return cc;
  }

  // sets the flags for `first - second`
  void cmpFlags(Expression* first, Expression* second, Type* t, bool zeroTest) {
    int w = W(t);
    bool sgn = t->isSigned();
    if (zeroTest && second->isConst && second->cv == 0) {
      gen(first);
      if (w == 1) I("OR", "A"); else { I("LD", "A", "H"); I("OR", "L"); }
      return;
    }
    if (w == 1) {
      gen(first);
      if (!sgn) {
        if (simple8(second)) I("CP", operand8(second));
        else { I("PUSH", "AF"); gen(second); I("LD", "E", "A"); I("POP", "AF"); I("CP", "E"); }
        return;
      }
      if (second->isConst) { I("XOR", "0x80"); I("CP", n8((second->cv & 0xFF) ^ 0x80)); return; }
      if (simple8(second)) I("LD", "E", operand8(second));
      else { I("PUSH", "AF"); gen(second); I("LD", "E", "A"); I("POP", "AF"); }
      I("XOR", "0x80"); I("LD", "D", "A"); I("LD", "A", "E"); I("XOR", "0x80"); I("LD", "E", "A"); I("LD", "A", "D");
      I("CP", "E");
      return;
    }
    gen(first);
    if (sgn) { I("LD", "A", "H"); I("XOR", "0x80"); I("LD", "H", "A"); }
    if (second->isConst) {
      I("LD", "DE", n16(sgn ? (second->cv ^ 0x8000) : second->cv));
    } else {
      second16(second);
      if (sgn) { I("LD", "A", "D"); I("XOR", "0x80"); I("LD", "D", "A"); }
    }
    I("OR", "A");
    I("SBC", "HL", "DE");
  }

  void second16(Expression* r) {
    if (simple16(r)) { loadDE(r); return; }
    I("PUSH", "HL"); gen(r); I("EX", "DE", "HL"); I("POP", "HL");
  }

  // jump to `target` when the truth of `e` equals `jumpIfTrue`, fall through otherwise
  void branch(Expression* e, const std::string& target, bool jumpIfTrue) {
    if (e->isConst) { if ((e->cv != 0) == jumpIfTrue) J("", target); return; }
    e = stripGroup(e);
    if (auto* u = as<Expr::Unary>(e)) {
      if (u->op == OP_NOT) { branch(u->e.get(), target, !jumpIfTrue); return; }
    }
    if (auto* c = as<Expr::Cast>(e)) {
      if (c->e->ty->isScalar() && c->ty->isScalar() && c->ty->width() >= c->e->ty->width()) { branch(c->e.get(), target, jumpIfTrue); return; }
    }
    if (auto* b = as<Expr::Binary>(e)) {
      if (b->op == OP_AND) {
        if (!jumpIfTrue) { branch(b->l.get(), target, false); branch(b->r.get(), target, false); }
        else { std::string skip = newL(); branch(b->l.get(), skip, false); branch(b->r.get(), target, true); L(skip); }
        return;
      }
      if (b->op == OP_OR) {
        if (jumpIfTrue) { branch(b->l.get(), target, true); branch(b->r.get(), target, true); }
        else { std::string skip = newL(); branch(b->l.get(), skip, true); branch(b->r.get(), target, false); L(skip); }
        return;
      }
      if (isCmp(b->op)) {
        std::string cc;
        Expression *first = b->l.get(), *secondE = b->r.get();
        switch (b->op) {
          case OP_EQUAL: cc = "Z"; break;
          case OP_NOT_EQUAL: cc = "NZ"; break;
          case OP_LESS: cc = "C"; break;
          case OP_MORE_OR_EQUAL: cc = "NC"; break;
          case OP_MORE: cc = "C"; std::swap(first, secondE); break;
          case OP_LESS_OR_EQUAL: cc = "NC"; std::swap(first, secondE); break;
          default: break;
        }
        cmpFlags(first, secondE, b->opTy, b->op == OP_EQUAL || b->op == OP_NOT_EQUAL);
        J(jumpIfTrue ? cc : inv(cc), target);
        return;
      }
    }
    // plain value: test against zero
    int w = W(e->ty);
    gen(e);
    if (w == 1) I("OR", "A"); else { I("LD", "A", "H"); I("OR", "L"); }
    J(jumpIfTrue ? "NZ" : "Z", target);
  }

  // ---------------------------------------------------------------- assignment
  static bool pure(Expression* e) {
    if (!e) return true;
    if (as<Expr::Assign>(e) || as<Expr::Invoke>(e) || as<Expr::Suffix>(e)) return false;
    if (auto* u = as<Expr::Unary>(e)) { if (u->op == OP_INC || u->op == OP_DEC) return false; return pure(u->e.get()); }
    if (auto* g = as<Expr::Group>(e)) return pure(g->e.get());
    if (auto* c = as<Expr::Cast>(e)) return pure(c->e.get());
    if (auto* b = as<Expr::Binary>(e)) return pure(b->l.get()) && pure(b->r.get());
    if (auto* i = as<Expr::Index>(e)) return pure(i->base.get()) && pure(i->index.get());
    if (auto* f = as<Expr::Field>(e)) return pure(f->base.get());
    if (auto* t = as<Expr::Ternary>(e)) return pure(t->cond.get()) && pure(t->a.get()) && pure(t->b.get());
    return true;
  }

  void storeAcc(Expression* target, int w, bool discard) {       // acc -> target
    Place p;
    if (staticPlace(target, p)) { storePlace(p, w); return; }
    pushAcc(w);
    genAddr(target);
    if (w == 1) { I("POP", "AF"); I("LD", "(HL)", "A"); }
    else {
      I("POP", "DE"); I("LD", "(HL)", "E"); I("INC", "HL"); I("LD", "(HL)", "D");
      if (!discard) I("EX", "DE", "HL");
    }
  }

  void genAssign(Expr::Assign* a, bool discard) {
    Expression* target = a->target.get();
    Type* tt = target->ty;

    if (tt->isStruct()) {
      uint32_t n = tt->size();
      genAddr(a->value.get());
      I("PUSH", "HL");
      genAddr(target);
      I("EX", "DE", "HL");
      I("POP", "HL");
      I("LD", "BC", n16((int)n));
      I("LDIR", "", "", "Copy the struct");
      return;
    }

    int w = W(tt);
    if (a->op != ASSIGN) { genCompound(a, discard); return; }

    Expression* val = a->value.get();
    Place p;
    if (staticPlace(target, p)) {
      if (val->isConst) {
        int32_t v = val->cv;
        if (p.k == Place::FRAME) {
          if (w == 1) I("LD", idx(p.off), n8(v));
          else { I("LD", idx(p.off), n8(v)); I("LD", idx(p.off + 1), n8(v >> 8)); }
          if (!discard) loadConst(v, w);
          return;
        }
        loadConst(v, w);
        storePlace(p, w);
        return;
      }
      gen(val);
      storePlace(p, w);
      return;
    }

    // target needs an address computation
    if (val->isConst || simple(val, w)) {
      genAddr(target);
      if (w == 1) {
        if (val->isConst) { I("LD", "(HL)", n8(val->cv)); if (!discard) loadConst(val->cv, 1); }
        else { loadA(val); I("LD", "(HL)", "A"); }
      } else {
        loadDE(val);
        I("LD", "(HL)", "E"); I("INC", "HL"); I("LD", "(HL)", "D");
        if (!discard) I("EX", "DE", "HL");
      }
      return;
    }
    gen(val);
    storeAcc(target, w, discard);
  }

  void genCompound(Expr::Assign* a, bool discard) {
    if (!pure(a->target.get())) throw std::runtime_error(Diag::where(a->tok->line, a->tok->col) + ": error: compound assignment with side effects in the left side, split it into two statements");
    Type* tt = a->target->ty;
    Type* ot = a->opTy;
    TokenT op;
    switch (a->op) {
      case ASSIGN_PLUS: op = OP_PLUS; break;
      case ASSIGN_MINUS: op = OP_MINUS; break;
      case ASSIGN_MUL: op = STAR; break;
      case ASSIGN_DIV: op = OP_DIV; break;
      case ASSIGN_MOD: op = OP_MOD; break;
      case ASSIGN_BIT_OR: op = BIT_OR; break;
      case ASSIGN_BIT_AND: op = AMPERSAND; break;
      case ASSIGN_BIT_XOR: op = BIT_XOR; break;
      case ASSIGN_SHL: op = BIT_LEFT_SHIFT; break;
      default: op = BIT_RIGHT_SHIFT; break;
    }
    expr_t lhs = a->target;
    if (!Types::same(tt, ot)) {
      auto c = std::make_shared<Expr::Cast>(ot, lhs);
      c->ty = ot; c->tok = a->tok;
      lhs = c;
    }
    Expr::Binary bin(op, lhs, a->value);
    bin.ty = bin.opTy = ot;
    bin.tok = a->tok;
    genBinary(&bin);
    convert(ot, tt);
    storeAcc(a->target.get(), W(tt), discard);
  }

  void genIncDec(Expression* target, bool inc, bool prefix, bool discard, Type* ty) {
    int w = W(ty);
    int step = 1;
    if (ty->isPtr()) step = (int)ty->base->size();
    Place p;
    if (staticPlace(target, p)) {
      if (w == 1) {
        if (p.k == Place::FRAME) {
          if (!discard && !prefix) loadPlace(p, 1);
          I(inc ? "INC" : "DEC", idx(p.off));
          if (!discard && prefix) loadPlace(p, 1);
        } else {
          I("LD", "HL", absText(p));
          if (!discard && !prefix) I("LD", "A", "(HL)");
          I(inc ? "INC" : "DEC", "(HL)");
          if (!discard && prefix) I("LD", "A", "(HL)");
        }
        return;
      }
      loadPlace(p, 2);
      incHL(inc, step);
      storePlace(p, 2);
      if (!discard && !prefix) incHL(!inc, step);
      return;
    }
    genAddr(target);
    if (w == 1) {
      if (!discard && !prefix) I("LD", "A", "(HL)");
      I(inc ? "INC" : "DEC", "(HL)");
      if (!discard && prefix) I("LD", "A", "(HL)");
      return;
    }
    I("PUSH", "HL");
    I("RST", "0x20", "", "Load the word at (HL) to reg HL");
    incHL(inc, step);
    I("POP", "DE");
    I("EX", "DE", "HL");
    I("LD", "(HL)", "E"); I("INC", "HL"); I("LD", "(HL)", "D");
    I("EX", "DE", "HL");
    if (!discard && !prefix) incHL(!inc, step);
  }

  void incHL(bool inc, int step) {
    if (step <= 3) { for (int i = 0; i < step; i++) I(inc ? "INC" : "DEC", "HL"); return; }
    I("LD", "DE", n16(inc ? step : -step));
    I("ADD", "HL", "DE");
  }

  // ---------------------------------------------------------------- calls
  static std::string slotReg(int slot, int w) {
    static const char* r8[] = {"A", "E", "C"};
    static const char* r16[] = {"HL", "DE", "BC"};
    return w == 1 ? r8[slot] : r16[slot];
  }

  bool argSimple(Expression* e, const std::string& reg) {
    if (e->isConst) return true;
    if (reg.size() == 2) return simple16(e);
    Expression* s = strip(e);
    Place p;
    if (W(s->ty) != 1 || !staticPlace(s, p)) return false;
    return p.k == Place::FRAME || reg == "A";
  }

  void loadReg(Expression* e, const std::string& reg) {
    if (reg.size() == 2) { loadPair(e, reg); return; }
    if (e->isConst) { if (reg == "A" && !(e->cv & 0xFF)) I("XOR", "A"); else I("LD", reg, n8(e->cv)); return; }
    Place p;
    staticPlace(strip(e), p);
    if (p.k == Place::FRAME) I("LD", reg, idx(p.off));
    else I("LD", "A", "(" + absText(p) + ")");
  }

  void moveAccTo(int w, const std::string& reg) {
    if (w == 1) { if (reg != "A") I("LD", reg, "A"); return; }
    if (reg == "HL") return;
    if (reg == "DE") I("EX", "DE", "HL");
    else { I("LD", "B", "H"); I("LD", "C", "L"); }
  }

  // call through a function pointer: the target travels in IY (nothing else uses it), `CALL @ZZ_ICALL`
  // is `JP (IY)` behind a CALL. Arguments use the normal slots, so up to 3 parameters work.
  void genIndirectCall(Expr::Invoke* c) {
    Type* ft = c->fnType;
    size_t n = c->args.size();
    std::vector<std::string> regs(n);
    for (size_t i = 0; i < n; i++) regs[i] = slotReg((int)i, W(ft->params[i]));

    // a global pointer is read straight into IY at the end, anything else is parked on the stack
    std::string direct;
    Expression* callee = stripGroup(c->callee.get());
    bool parked = false;
    Place pl;
    if (auto* v = as<Expr::Var>(callee); v && !addrConst(callee, direct) && staticPlace(callee, pl, false) && pl.k == Place::ABS) direct = "(" + absText(pl) + ")";
    else if (addrConst(callee, direct)) direct = direct;       // a known function: IY gets its label
    else { gen(callee); I("PUSH", "HL"); parked = true; direct.clear(); }

    std::vector<size_t> complexArgs;
    for (size_t i = 0; i < n; i++) if (!argSimple(c->args[i].get(), regs[i])) complexArgs.push_back(i);
    std::stable_sort(complexArgs.begin(), complexArgs.end(), [&](size_t x, size_t y) { return (regs[x] == "A") > (regs[y] == "A"); });
    if (complexArgs.size() == 1) {
      size_t i = complexArgs[0];
      gen(c->args[i].get());
      moveAccTo(W(ft->params[i]), regs[i]);
    } else {
      for (size_t i : complexArgs) { gen(c->args[i].get()); pushAcc(W(ft->params[i])); }
      for (size_t k = complexArgs.size(); k-- > 0;) {
        size_t i = complexArgs[k];
        if (W(ft->params[i]) == 2) I("POP", regs[i]);
        else { I("POP", "AF"); if (regs[i] != "A") I("LD", regs[i], "A"); }
      }
    }
    for (size_t i = 0; i < n; i++)
      if (std::find(complexArgs.begin(), complexArgs.end(), i) == complexArgs.end()) loadReg(c->args[i].get(), regs[i]);

    if (parked) I("POP", "IY"); else I("LD", "IY", direct);
    rt.insert("@ZZ_ICALL");
    I("CALL", "@ZZ_ICALL");
  }

  void genCall(Expr::Invoke* c) {
    if (!c->fn) { genIndirectCall(c); return; }
    FuncInfo* f = c->fn;
    Decl::Func* d = f->decl;
    const auto& bd = d->bind;
    const std::string& label = f->label;
    size_t n = c->args.size();

    // intrinsics
    if (label == "$in") {
      Expression* port = c->args[0].get();
      if (port->isConst) I("IN", "A", "(" + n8(port->cv) + ")");
      else { gen(port); I("LD", "C", "A"); I("IN", "A", "(C)"); }
      return;
    }
    if (label == "$out") {
      Expression *port = c->args[0].get(), *val = c->args[1].get();
      if (port->isConst) { gen(val); I("OUT", "(" + n8(port->cv) + ")", "A"); return; }
      gen(port); I("LD", "C", "A");
      if (simple8(val)) loadA(val);
      else { I("PUSH", "BC"); gen(val); I("POP", "BC"); }
      I("OUT", "(C)", "A");
      return;
    }

    std::vector<std::string> regs(n);
    for (size_t i = 0; i < n; i++) {
      if (bd.bound && !bd.in.empty()) regs[i] = bd.in[i];
      else regs[i] = slotReg((int)i, W(f->type->params[i]));
    }

    std::vector<size_t> complexArgs;
    for (size_t i = 0; i < n; i++) if (!argSimple(c->args[i].get(), regs[i])) complexArgs.push_back(i);
    std::stable_sort(complexArgs.begin(), complexArgs.end(), [&](size_t x, size_t y) { return (regs[x] == "A") > (regs[y] == "A"); });

    if (complexArgs.size() == 1) {
      size_t i = complexArgs[0];
      gen(c->args[i].get());
      moveAccTo(W(f->type->params[i]), regs[i]);
    } else {
      for (size_t i : complexArgs) { gen(c->args[i].get()); pushAcc(W(f->type->params[i])); }
      for (size_t k = complexArgs.size(); k-- > 0;) {
        size_t i = complexArgs[k];
        if (W(f->type->params[i]) == 2) I("POP", regs[i]);
        else { I("POP", "AF"); if (regs[i] != "A") I("LD", regs[i], "A"); }
      }
    }
    for (size_t i = 0; i < n; i++)
      if (std::find(complexArgs.begin(), complexArgs.end(), i) == complexArgs.end()) loadReg(c->args[i].get(), regs[i]);

    if (label == "$ldir") I("LDIR");
    else if (label.compare(0, 4, "RST ") == 0) I("RST", label.substr(4));
    else {
      if (label == "@ZZ_MEMSET") rt.insert(label);
      I("CALL", label);
    }

    if (bd.bound && !bd.out.empty() && !f->ret->isVoid()) {
      int w = W(f->ret);
      if (w == 1) { if (bd.out != "A") I("LD", "A", bd.out); }
      else if (bd.out == "DE") I("EX", "DE", "HL");
      else if (bd.out == "BC") { I("LD", "H", "B"); I("LD", "L", "C"); }
    }
  }

  // ======================================================================== statements
  void genStmt(Statement* s) {
    if (!s) return;
    if (auto* b = as<Stmt::Block>(s)) { for (auto& x : b->body) genStmt(x.get()); return; }
    if (auto* e = as<Stmt::ExprStmt>(s)) { gen(e->e.get(), true); return; }
    if (auto* v = as<Decl::Var>(s)) { for (auto& x : v->initCode) genStmt(x.get()); return; }
    if (auto* i = as<Stmt::If>(s)) {
      std::string le = newL();
      branch(i->cond.get(), le, false);
      genStmt(i->then.get());
      if (i->els) {
        std::string lend = newL();
        J("", lend);
        L(le);
        genStmt(i->els.get());
        L(lend);
      } else L(le);
      return;
    }
    if (auto* w = as<Stmt::While>(s)) {
      std::string top = newL(), end = newL();
      L(top);
      branch(w->cond.get(), end, false);
      breakL.push_back(end); contL.push_back(top);
      genStmt(w->body.get());
      breakL.pop_back(); contL.pop_back();
      J("", top);
      L(end);
      return;
    }
    if (auto* d = as<Stmt::Do>(s)) {
      std::string top = newL(), cont = newL(), end = newL();
      L(top);
      breakL.push_back(end); contL.push_back(cont);
      genStmt(d->body.get());
      breakL.pop_back(); contL.pop_back();
      L(cont);
      branch(d->cond.get(), top, true);
      L(end);
      return;
    }
    if (auto* f = as<Stmt::For>(s)) {
      std::string top = newL(), cont = newL(), end = newL();
      if (f->init) genStmt(f->init.get());
      L(top);
      if (f->cond) branch(f->cond.get(), end, false);
      breakL.push_back(end); contL.push_back(cont);
      genStmt(f->body.get());
      breakL.pop_back(); contL.pop_back();
      L(cont);
      if (f->step) gen(f->step.get(), true);
      J("", top);
      L(end);
      return;
    }
    if (auto* r = as<Stmt::Return>(s)) {
      if (r->value) gen(r->value.get());
      if ((fn->frame == 0 && !fn->isInterrupt) || fn->isNaked) I("RET"); else J("", retLabel);
      return;
    }
    if (as<Stmt::Break>(s)) { J("", breakL.back()); return; }
    if (as<Stmt::Continue>(s)) { J("", contL.back()); return; }
    if (auto* g = as<Stmt::Goto>(s)) { J("", "@" + fnBase + "__" + g->name); return; }
    if (auto* l = as<Stmt::Label>(s)) { L("@" + fnBase + "__" + l->name); return; }
    if (auto* sw = as<Stmt::Switch>(s)) { genSwitch(sw); return; }
    if (auto* c = as<Stmt::Case>(s)) { L(caseLabel.at(c)); return; }
    if (auto* a = as<Stmt::Asm>(s)) { genAsm(a); return; }
    if (as<Decl::Struct>(s) || as<Decl::Enum>(s) || as<Decl::Typedef>(s)) return;
    internal("unhandled statement");
  }

  void genSwitch(Stmt::Switch* sw) {
    Type* t = sw->cond->ty;
    int w = W(t);
    std::string end = newL();
    std::string deflt = end;
    gen(sw->cond.get());
    for (auto* c : sw->cases) {
      std::string lbl = newL("case");
      caseLabel[c] = lbl;
      if (c->isDefault) { deflt = lbl; continue; }
      if (w == 1) { I("CP", n8(c->cv)); J("Z", lbl); }
      else {
        std::string nx = newL();
        I("LD", "A", "L"); I("CP", n8(c->cv)); J("NZ", nx);
        I("LD", "A", "H"); I("CP", n8(c->cv >> 8)); J("Z", lbl);
        L(nx);
      }
    }
    J("", deflt);
    breakL.push_back(end);
    genStmt(sw->body.get());
    breakL.pop_back();
    L(end);
  }

  void genAsm(Stmt::Asm* a) {
    std::string text = a->text;
    std::istringstream ss(text);
    std::string line;
    while (std::getline(ss, line)) {
      line = Trim(line);
      if (line.empty()) continue;
      for (auto& r : a->refs) {
        std::string pat = "{" + r.first + "}";
        std::string rep;
        Symbol* s = r.second;
        if (s->kind == Symbol::GLOBAL) rep = s->hasAddr ? hex16((int)s->addr) : s->label;
        else if (s->kind == Symbol::FUNC) rep = s->fn->label;
        else rep = "IX+" + std::to_string(s->off);
        size_t p;
        while ((p = line.find(pat)) != std::string::npos) line.replace(p, pat.size(), rep);
      }
      bool isLabel = line.back() == ':';
      RAW(isLabel ? line : "  " + line);
    }
  }

  // ======================================================================== functions
  std::string signature(Decl::Func* d) {
    std::string s = d->name + "(";
    for (size_t i = 0; i < d->params.size(); i++) s += (i ? ", " : "") + Types::str(d->params[i].type) + " " + d->params[i].name;
    return s + ") -> " + Types::str(d->ret);
  }

  void flushCode() {
    peephole(code);
    relax(code);
    for (auto& c : code) out += insText(c) + "\n";
    code.clear();
  }

  // register pairs touched by code[from..]: operands name them; calls, raw asm and block instructions imply more
  std::set<std::string> interruptRegs(size_t from) {
    std::set<std::string> r{"AF"};                          // the flags are almost always changed
    auto addName = [&](const std::string& n) {
      static const std::map<std::string, std::string> m = {
        {"A", "AF"}, {"F", "AF"}, {"AF", "AF"}, {"B", "BC"}, {"C", "BC"}, {"BC", "BC"}, {"D", "DE"}, {"E", "DE"}, {"DE", "DE"},
        {"H", "HL"}, {"L", "HL"}, {"HL", "HL"}, {"IY", "IY"}, {"IYH", "IY"}, {"IYL", "IY"}, {"IX", "IX"}};
      auto it = m.find(n);
      if (it != m.end()) r.insert(it->second);
    };
    auto scan = [&](const std::string& s) {                  // identifiers inside an operand: "(IX+3)", "(HL)", "AF'"
      std::string w;
      for (size_t i = 0; i <= s.size(); i++) {
        char ch = i < s.size() ? s[i] : ' ';
        if (isalnum((unsigned char)ch) || ch == '\'') w += ch;
        else { if (!w.empty()) addName(w); w.clear(); }
      }
    };
    for (size_t i = from; i < code.size(); i++) {
      const Ins& c = code[i];
      if (c.kind == Ins::RAW) { std::set<std::string> all{"AF", "BC", "DE", "HL", "IY"}; if (!(fn->frame > 0)) all.insert("IX"); return all; }
      if (c.kind == Ins::LABEL) continue;
      if (c.kind == Ins::BRANCH) { if (c.op == "DJNZ") r.insert("BC"); continue; }
      const std::string& o = c.op;
      if (o == "CALL" || o == "RST") {                         // a bound routine says what it destroys
        const FuncInfo* callee = nullptr;
        for (auto* f : sema.funcs) if (f->label == (o == "CALL" ? c.a : o + " " + c.a)) callee = f;
        if (callee && callee->decl && callee->decl->bind.bound && callee->decl->bind.hasClobber) {
          for (auto& n : callee->decl->bind.clobber) addName(n);
          if (callee->decl->bind.out != "") addName(callee->decl->bind.out);
          continue;
        }
      }
      if (o == "CALL" || o == "RST" || o == "EXX" || (o == "EX" && (c.a == "AF" || c.b == "AF'"))) { r.insert("BC"); r.insert("DE"); r.insert("HL"); r.insert("IY"); continue; }
      if (o == "LDIR" || o == "LDDR" || o == "LDI" || o == "LDD" || o == "CPIR" || o == "CPDR" || o == "CPI" || o == "CPD" ||
          o == "INIR" || o == "INDR" || o == "OTIR" || o == "OTDR" || o == "INI" || o == "IND" || o == "OUTI" || o == "OUTD") { r.insert("BC"); r.insert("DE"); r.insert("HL"); continue; }
      if (o == "DAA" || o == "CPL" || o == "NEG" || o == "RLA" || o == "RRA" || o == "RLCA" || o == "RRCA") continue;
      if (o == "SUB" || o == "AND" || o == "OR" || o == "XOR" || o == "CP") { scan(c.a); continue; }
      scan(c.a); scan(c.b);
    }
    if (fn->frame > 0) r.erase("IX");                      // the frame prologue saves it already
    return r;
  }

  void emitFunc(Decl::Func* d) {
    fn = d->info;
    fnBase = fn->label[0] == '@' ? fn->label.substr(1) : fn->label;
    lblSeq = 0;
    retLabel = "@" + fnBase + "__ret";
    breakL.clear(); contL.clear();
    code.clear();

    out += ";;\n;; func " + signature(d) + ";\n";
    if (!d->isNaked) {
      for (size_t i = 0; i < d->params.size(); i++)
        out += ";;   arg " + std::to_string(i + 1) + " " + d->params[i].name + " -- reg " + slotReg((int)i, W(d->params[i].type)) + "\n";
      if (!d->ret->isVoid()) out += std::string(";;   ret -- reg ") + (W(d->ret) == 1 ? "A" : "HL") + "\n";
    }
    L(fn->label);
    if (fn->name == "setup" && needInit) I("CALL", "@ZZ_INIT", "", "Initialize global variables");
    size_t saveAt = code.size();                 // interrupt: the register saves are inserted here afterwards

    bool framed = fn->frame > 0 && !d->isNaked;
    if (framed) {
      I("PUSH", "IX", "", "Save the frame ptr of the caller");
      I("LD", "IX", hex16(-fn->frame), "Reserve " + std::to_string(fn->frame) + " byte(s) for the frame");
      I("ADD", "IX", "SP");
      I("LD", "SP", "IX");
      for (size_t i = 0; i < fn->params.size(); i++) {
        Symbol* s = fn->params[i];
        int w = W(s->type);
        std::string r = slotReg((int)i, w);
        if (w == 1) I("LD", idx(s->off), r);
        else {
          static const std::map<std::string, std::pair<std::string, std::string>> lh = {{"HL", {"L", "H"}}, {"DE", {"E", "D"}}, {"BC", {"C", "B"}}};
          I("LD", idx(s->off), lh.at(r).first); I("LD", idx(s->off + 1), lh.at(r).second);
        }
      }
    }

    genStmt(d->body.get());

    if (framed || fn->isInterrupt) L(retLabel);
    if (framed) {
      int fr = fn->frame;
      if (fr <= 8) {
        for (int i = 0; i + 1 < fr; i += 2) I("POP", "BC");
        if (fr & 1) I("INC", "SP");
      } else {
        I("LD", "IX", n16(fr));
        I("ADD", "IX", "SP");
        I("LD", "SP", "IX");
      }
      I("POP", "IX", "", "Restore the frame ptr of the caller");
    }
    if (fn->isInterrupt) {
      // which register pairs can the body disturb? calls and raw asm can disturb all of them
      std::set<std::string> save = interruptRegs(saveAt);
      std::vector<Ins> pre, post;
      static const char* order[] = {"AF", "BC", "DE", "HL", "IX", "IY"};
      for (const char* r : order) {
        if (!save.count(r)) continue;
        Ins p; p.op = "PUSH"; p.a = r; if (opt.comments && std::string(r) == "AF") p.cmt = "Save the registers the handler touches";
        pre.push_back(p);
        Ins q; q.op = "POP"; q.a = r; post.insert(post.begin(), q);
      }
      if (!post.empty() && opt.comments) post.front().cmt = "Restore the registers of the interrupted code";
      code.insert(code.begin() + saveAt, pre.begin(), pre.end());
      code.insert(code.end(), post.begin(), post.end());
    }
    if (!d->isNaked) I("RET");
    flushCode();
    out += "\n";
  }

  // ======================================================================== data
  struct DItem { bool word; std::string text; int32_t v; };

  static std::string quoteBytes(const std::string& s, size_t from, size_t to) {
    std::string r;
    for (size_t i = from; i < to; i++) {
      unsigned char c = s[i];
      if (c >= 32 && c < 127 && c != '"' && c != '\\') r += (char)c; else return "";
    }
    return r;
  }

  // collect initializer bytes of an object
  void initItems(Type* t, Expression* init, std::vector<DItem>& items) {
    if (!init) { for (uint32_t i = 0; i < t->size(); i++) items.push_back({false, "", 0}); return; }
    if (auto* lst = as<Expr::Array>(init)) {
      if (t->isArray()) {
        for (uint32_t i = 0; i < t->count; i++) initItems(t->base, i < lst->elems.size() ? lst->elems[i].get() : nullptr, items);
      } else if (t->isStruct()) {
        for (size_t i = 0; i < t->sdef->members.size(); i++) initItems(t->sdef->members[i].type, i < lst->elems.size() ? lst->elems[i].get() : nullptr, items);
      } else initItems(t, lst->elems[0].get(), items);
      return;
    }
    if (t->isArray()) {                                 // string into char array
      auto* lit = as<Expr::Literal>(init);
      for (uint32_t i = 0; i < t->count; i++) items.push_back({false, "", i < lit->bytes.size() ? (unsigned char)lit->bytes[i] : 0});
      return;
    }
    std::string txt;
    int w = W(t);
    if (init->isConst) { items.push_back({w == 2, "", init->cv}); if (w == 2) items.back().v &= 0xFFFF; else items.back().v &= 0xFF; return; }
    if (addrConst(init, txt)) { items.push_back({true, txt, 0}); return; }
    // address + constant offset
    if (auto* b = as<Expr::Binary>(stripGroup(init))) {
      std::string t1;
      if (addrConst(b->l.get(), t1) && b->r->isConst) { items.push_back({true, t1 + (b->op == OP_MINUS ? "-" : "+") + std::to_string(b->r->cv), 0}); return; }
    }
    internal("initializer is not constant");
  }

  void printItems(const std::vector<DItem>& items) {
    size_t i = 0;
    while (i < items.size()) {
      if (items[i].word) {
        std::string line = "  DW ";
        size_t cnt = 0;
        while (i < items.size() && items[i].word && cnt < 8) {
          if (cnt) line += ", ";
          line += items[i].text.empty() ? n16(items[i].v) : items[i].text;
          i++; cnt++;
        }
        out += line + "\n";
      } else {
        std::string line = "  DB ";
        size_t cnt = 0;
        while (i < items.size() && !items[i].word && cnt < 16) {
          if (cnt) line += ", ";
          line += n8(items[i].v);
          i++; cnt++;
        }
        out += line + "\n";
      }
    }
  }

  void emitStrings() {
    for (size_t i = 0; i < sema.strings.size(); i++) {
      const std::string& s = sema.strings[i];
      out += "_zs" + std::to_string(i) + ":\n";
      // printable runs as "text", everything else as numbers; the assembler reader must never see INCLUDE / MACRO / ENDM inside text
      std::string line = "  DB ";
      std::string run;
      bool first = true;
      auto flushRun = [&]() {
        if (run.empty()) return;
        line += (first ? "" : ", ") + std::string("\"") + run + "\"";
        first = false; run.clear();
      };
      for (size_t k = 0; k < s.size(); k++) {
        unsigned char c = s[k];
        bool printable = c >= 32 && c < 127 && c != '"' && c != '\\';
        if (printable) {
          run += (char)c;
          for (const char* bad : {"INCLUDE", "MACRO", "ENDM"}) {
            std::string b = bad;
            if (run.size() >= b.size() && run.compare(run.size() - b.size(), b.size(), b) == 0) {
              run.pop_back();
              flushRun();
              line += (first ? "" : ", ") + std::to_string((int)c); first = false;
            }
          }
        } else { flushRun(); line += (first ? "" : ", ") + n8(c); first = false; }
        if (line.size() > 100) { flushRun(); out += line + "\n"; line = "  DB "; first = true; }
      }
      flushRun();
      line += (first ? "" : ", ") + std::string("0");
      out += line + "\n";
    }
  }

  std::string equText;

  void emitGlobals() {
    // ROM objects and RAM layout
    std::vector<Symbol*> rom, data, bss;
    for (auto* s : sema.globals) {
      if (s->hasAddr || s->isExtern) continue;
      Decl::Var* v = s->var;
      if (s->isConst) rom.push_back(s);
      else if (v && v->init) data.push_back(s);
      else bss.push_back(s);
    }
    std::string romKeep;
    romKeep.swap(out);
    for (auto* s : sema.globals) if (s->hasAddr) equText += s->label + " EQU " + hex16((int)s->addr) + "\n";

    if (!rom.empty()) out += "\n;; Read only data\n";
    for (auto* s : rom) {
      out += s->label + ":\n";
      std::vector<DItem> it;
      initItems(s->type, s->var ? s->var->init.get() : nullptr, it);
      printItems(it);
    }

    uint32_t off = 0;
    std::vector<DItem> image;
    equText += "ZZ_DATA EQU " + hex16((int)opt.dataBase) + "\n";
    for (auto* s : data) {
      equText += s->label + " EQU ZZ_DATA+" + std::to_string(off) + "\n";
      initItems(s->type, s->var->init.get(), image);
      off += s->type->size();
    }
    uint32_t dataSize = off;
    equText += "ZZ_BSS EQU ZZ_DATA+" + std::to_string(dataSize) + "\n";
    off = 0;
    for (auto* s : bss) {
      equText += s->label + " EQU ZZ_BSS+" + std::to_string(off) + "\n";
      off += std::max<uint32_t>(1, s->type->size());
    }
    bssSize = off;
    dataSizeV = dataSize;
    if (dataSize) { out += "ZZ_DATA_IMG:\n"; printItems(image); }
    (void)romKeep;
  }

  uint32_t bssSize = 0, dataSizeV = 0;

  void emitInitRoutine() {
    out += ";;\n;; proc ZZ_INIT() -> void;\n;;   Zero the .bss, copy the initial values of the globals from ROM to RAM\n@ZZ_INIT:\n";
    if (dataSizeV) {
      out += "  LD HL, ZZ_DATA_IMG\n  LD DE, ZZ_DATA\n  LD BC, " + n16((int)dataSizeV) + "\n  LDIR\n";
    }
    if (bssSize) {
      out += "  LD HL, ZZ_BSS\n  XOR A\n";
      if (bssSize == 1) out += "  LD (HL), A\n";
      else out += "  LD (HL), A\n  LD D, H\n  LD E, L\n  INC DE\n  LD BC, " + n16((int)bssSize - 1) + "\n  LDIR\n";
    }
    out += "  RET\n\n";
  }
};

inline std::string CodeGen::generate(std::vector<stmt_t>& prog, const std::vector<std::string>& asmIncludes) {
  out.clear();
  needInit = false;
  for (auto* s : sema.globals) {
    if (s->hasAddr || s->isExtern || s->isConst) continue;
    needInit = true;
  }
  for (auto& st : prog) {
    if (auto* f = as<Decl::Func>(st.get())) {
      if (f->body && f->name == "setup") hasSetup = true;
      if (f->body && f->name == "main") hasMain = true;
    }
  }

  std::string body;                                  // everything after the header
  std::string saved;
  saved.swap(out);

  // functions and head asm in source order
  for (auto& st : prog) {
    if (auto* f = as<Decl::Func>(st.get())) {
      if (!f->body || !f->info) continue;
      if (f->info->isStatic && !f->info->used) continue;
      emitFunc(f);
    } else if (auto* a = as<Stmt::Asm>(st.get())) {
      code.clear();
      genAsm(a);
      for (auto& c : code) out += c.op + "\n";
      code.clear();
      out += "\n";
    }
  }
  if (!hasSetup && needInit && !opt.bare) out += "SETUP:\n  CALL @ZZ_INIT\n  RET\n\n";
  if (!hasMain && !opt.bare) out += "MAIN:\n  RET\n\n";

  // runtime routines (with their dependencies)
  std::set<std::string> done;
  std::vector<std::string> todo(rt.begin(), rt.end());
  while (!todo.empty()) {
    std::string n = todo.back(); todo.pop_back();
    if (!done.insert(n).second) continue;
    auto it = Runtime::table().find(n);
    if (it == Runtime::table().end()) continue;
    for (auto& d : it->second.needs) todo.push_back(d);
  }
  for (auto& n : done) out += Runtime::table().at(n).text + "\n";

  // data (computes sizes needed by the init routine first)
  std::string codeText;
  codeText.swap(out);
  emitGlobals();
  std::string dataText;
  dataText.swap(out);
  emitStrings();
  std::string strText;
  strText.swap(out);
  if (needInit) emitInitRoutine();
  std::string initText;
  initText.swap(out);

  out = saved;
  out += ";; Generated by zazy";
  if (!opt.source.empty()) out += " from " + opt.source;
  out += "\n;; Do not edit, change the C source instead\n\n";

  if (opt.bare) {
    out += ";; --bare: no Init.z80, a minimal reset code\nORG 0x0000\n  LD SP, 0x5FFF\n  JP @ZZ_START\n\n"
           "ORG 0x0010\nRST10:       ;; aka SHOW CHAR\n  OUT (0x00), A\n  RET\n\n"
           "ORG 0x0018\nRST18:       ;; aka SHOW STRING\n  LD A, (HL)\n  OR A\n  RET Z\n  OUT (0x00), A\n  INC HL\n  JR RST18-$\n\n"
           "ORG 0x0020\nRST20:       ;; aka LOAD WORD\n  LD A, (HL)\n  INC HL\n  LD H, (HL)\n  LD L, A\n  RET\n\n"
           "ORG 0x0100\n@ZZ_START:\n";
    if (hasSetup) out += "  CALL SETUP\n"; else if (needInit) out += "  CALL @ZZ_INIT\n";
    if (hasMain) out += "  CALL MAIN\n";
    out += "  HALT\n\n";
  } else {
    out += "INCLUDE \"" + opt.initPath + "\"\n";
    for (auto& a : asmIncludes) out += "INCLUDE \"" + a + "\"\n";
    out += "\n";
  }
  out += ";; Variables\n" + equText + "\n";
  out += codeText;
  if (opt.bare) for (auto& a : asmIncludes) out += "INCLUDE \"" + a + "\"\n";
  {
    bool usesMath = false;
    for (const char* r : {"CALL @MUL16x8", "CALL @DIV8", "CALL @DIV16x8"}) if (codeText.find(r) != std::string::npos) usesMath = true;
    bool have = false;
    for (auto& a : asmIncludes) if (a.size() >= 8 && a.compare(a.size() - 8, 8, "Math.z80") == 0) have = true;
    if (usesMath && !have) out += "\n;; hand written multiplication / division used by the code above\nINCLUDE \"" + opt.mathPath + "\"\n";
  }
  out += initText;
  out += dataText;
  out += "\n;; String literals\n" + strText;
  return out;
}

};
