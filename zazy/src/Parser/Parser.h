#pragma once
#include <map>
#include "AST.h"
#include "src/Lexer/Lexer.h"

namespace Zazy {

/**
 * Recursive descent parser for the zazy C subset.
 *
 *   program     -> ( declaration | asm-block | ";" )* EOF
 *   declaration -> spec declarator ( "=" init )? ( "," declarator ... )* ";"
 *                | spec declarator "(" params ")" ( binding ";" | block )
 *
 * Types are resolved while parsing (typedef names have to be known to tell a cast from a
 * parenthesised expression, and struct layouts are needed for sizeof / array dimensions), enum
 * constants are folded into literals. Everything else (names, conversions, constness of
 * expressions) is Sema's job.
 */
class Parser {
  struct ParseError {};

  struct Spec {
    Type* type = nullptr;
    bool isStatic = false, isExtern = false, isTypedef = false, isNaked = false, isInterrupt = false, isConst = false;
    bool defined = false;           // a struct / enum body was parsed as part of the spec
    stmt_t node;                    // Decl::Struct / Decl::Enum for the printer
  };

public:
  Parser(const std::string& src) {
    Lexer lexer(src);
    for (;;) {
      token_t t = lexer.next();
      toks.push_back(t);
      if (t->token == T_EOF) break;
    }
    for (auto& e : lexer.errors) {
      int l = 0, c = 0;
      if (sscanf(e.c_str(), "[Ln %d Col %d]", &l, &c) == 2) {
        auto p = e.find("Error: ");
        std::string msg = p == std::string::npos ? e : e.substr(p + 7);
        while (!msg.empty() && (msg.back() == '\n' || msg.back() == '.')) msg.pop_back();
        Diag::error(l, c, msg);
      } else Diag::plain(e);
    }
  }

  std::vector<stmt_t> parse() {
    std::vector<stmt_t> out;
    while (!check(T_EOF)) {
      try { top(out); }
      catch (ParseError&) { sync(); }
    }
    return out;
  }

private:
  std::vector<token_t> toks;
  size_t pos = 0;
  int anon = 0;

  std::map<std::string, Type*> typedefs;
  std::map<std::string, StructDef*> structTags;
  std::map<std::string, int32_t> enumConsts;
  std::map<std::string, Type*> enumTags;

  // -------------------------------------------------------------------------- token helpers
  const token_t& cur() const { return toks[pos]; }
  const token_t& at(size_t n) const { return toks[std::min(pos + n, toks.size() - 1)]; }
  bool check(TokenT t) const { return cur()->token == t; }
  bool checkAt(size_t n, TokenT t) const { return at(n)->token == t; }
  token_t advance() { token_t t = cur(); if (t->token != T_EOF) pos++; return t; }
  bool match(TokenT t) { if (check(t)) { advance(); return true; } return false; }

  [[noreturn]] void fail(const token_t& t, const std::string& msg) {
    Diag::error(t->line, t->col, msg);
    throw ParseError();
  }

  token_t expect(TokenT t, const std::string& what) {
    if (!check(t)) fail(cur(), "expected " + what + (cur()->token == T_EOF ? " before end of file" : ", found '" + cur()->lexeme + "'"));
    return advance();
  }

  bool isIdent(const char* name) const { return check(IDENTIFIER) && cur()->lexeme == name; }

  void sync() {
    // skip to the end of the broken declaration / statement
    int depth = 0;
    while (!check(T_EOF)) {
      TokenT t = cur()->token;
      if (t == LEFT_CURLY_BRACE) depth++;
      if (t == RIGHT_CURLY_BRACE) { if (depth > 0) depth--; if (depth == 0) { advance(); return; } }
      if (t == SEMICOLON && depth == 0) { advance(); return; }
      advance();
    }
  }

  // -------------------------------------------------------------------------- types
  bool isTypeKeyword(TokenT t) const {
    switch (t) {
      case W_VOID: case W_UINT8: case W_INT8: case W_UINT16: case W_INT16:
      case W_CHAR: case W_SHORT: case W_INT: case W_STRUCT: case W_ENUM: case W_CONST:
        return true;
      default: return false;
    }
  }
  bool isTypeStart(size_t n = 0) const {
    const token_t& t = at(n);
    if (isTypeKeyword(t->token)) return true;
    return t->token == IDENTIFIER && typedefs.count(t->lexeme);
  }
  bool isDeclStart() const {
    TokenT t = cur()->token;
    return isTypeStart() || t == W_STATIC || t == W_EXTERN || t == W_TYPEDEF || t == W_NAKED || t == W_INTERRUPT || t == W_AUTO;
  }

  // storage class + qualifiers + base type
  Spec spec() {
    Spec s;
    token_t first = cur();
    for (;;) {
      TokenT t = cur()->token;
      if (t == W_STATIC) { s.isStatic = true; advance(); }
      else if (t == W_EXTERN) { s.isExtern = true; advance(); }
      else if (t == W_TYPEDEF) { s.isTypedef = true; advance(); }
      else if (t == W_NAKED) { s.isNaked = true; advance(); }
      else if (t == W_INTERRUPT) { s.isInterrupt = true; advance(); }
      else if (t == W_AUTO) { advance(); }
      else if (t == W_CONST) { s.isConst = true; advance(); }
      else break;
    }
    if (s.type) return s;

    TokenT t = cur()->token;
    switch (t) {
      case W_VOID:   advance(); s.type = Types::Void(); break;
      case W_UINT8:
      case W_CHAR:   advance(); s.type = Types::U8(); break;
      case W_INT8:   advance(); s.type = Types::I8(); break;
      case W_UINT16: advance(); s.type = Types::U16(); break;
      case W_INT16:
      case W_INT:    advance(); s.type = Types::I16(); break;
      case W_SHORT:  advance(); match(W_INT); s.type = Types::I16(); break;
      case W_STRUCT: s.type = structSpec(s); break;
      case W_ENUM:   s.type = enumSpec(s); break;
      case IDENTIFIER: {
        auto it = typedefs.find(cur()->lexeme);
        if (it == typedefs.end()) fail(cur(), "unknown type name '" + cur()->lexeme + "'");
        advance();
        s.type = it->second;
        break;
      }
      default: fail(cur(), "expected a type, found '" + cur()->lexeme + "'");
    }
    // `uint8_t const x`
    while (check(W_CONST)) { s.isConst = true; advance(); }
    if (s.isConst) s.type = Types::constOf(s.type);
    (void)first;
    return s;
  }

  Type* structSpec(Spec& s) {
    token_t kw = advance();
    std::string tag;
    token_t tagTok = cur();
    if (check(IDENTIFIER)) tag = advance()->lexeme;

    StructDef* d = nullptr;
    if (!tag.empty()) {
      auto it = structTags.find(tag);
      if (it != structTags.end()) d = it->second;
    }
    bool body = check(LEFT_CURLY_BRACE);
    if (!d) {
      if (tag.empty() && !body) fail(kw, "struct needs a tag or a body");
      d = Types::newStruct(tag.empty() ? "<anon" + std::to_string(anon++) + ">" : tag);
      if (!tag.empty()) structTags[tag] = d;
    }
    Type* ty = Types::structType(d);

    if (!body) return ty;
    advance();
    if (d->complete) fail(tagTok, "redefinition of struct " + d->tag);

    uint32_t offset = 0;
    while (!check(RIGHT_CURLY_BRACE) && !check(T_EOF)) {
      Spec ms = spec();
      if (ms.isStatic || ms.isExtern || ms.isTypedef) fail(cur(), "storage class not allowed in a struct member");
      do {
        std::string name; token_t nameTok = cur();
        Type* mt = declarator(ms.type, name, nameTok, nullptr, false);
        if (mt->isVoid()) fail(nameTok, "member '" + name + "' declared void");
        if (mt->isFunc()) fail(nameTok, "function members are not supported");
        if (mt->isStruct() && !mt->sdef->complete) fail(nameTok, "member '" + name + "' has incomplete type " + Types::str(mt));
        if (mt->isArray() && mt->count == 0) fail(nameTok, "member '" + name + "' needs an array size");
        if (d->find(name)) fail(nameTok, "duplicate member '" + name + "'");
        d->members.push_back({name, mt, offset});
        offset += mt->size();
      } while (match(COMMA));
      expect(SEMICOLON, "';' after struct member");
    }
    expect(RIGHT_CURLY_BRACE, "'}'");
    if (d->members.empty()) fail(kw, "empty struct");
    d->size = offset;
    d->complete = true;

    s.defined = true;
    auto node = std::make_shared<Decl::Struct>();
    node->tok = kw; node->type = ty;
    s.node = node;
    return ty;
  }

  Type* enumSpec(Spec& s) {
    token_t kw = advance();
    std::string tag;
    if (check(IDENTIFIER)) tag = advance()->lexeme;
    Type* ty = Types::I16();
    if (!check(LEFT_CURLY_BRACE)) {
      if (tag.empty()) fail(kw, "enum needs a tag or a body");
      return ty;
    }
    advance();
    auto node = std::make_shared<Decl::Enum>();
    node->tok = kw; node->tag = tag;
    int32_t next = 0;
    while (!check(RIGHT_CURLY_BRACE) && !check(T_EOF)) {
      token_t nt = expect(IDENTIFIER, "enumerator name");
      if (enumConsts.count(nt->lexeme)) fail(nt, "redefinition of enumerator '" + nt->lexeme + "'");
      if (typedefs.count(nt->lexeme)) fail(nt, "'" + nt->lexeme + "' is already a type name");
      if (match(ASSIGN)) next = constExpr();
      enumConsts[nt->lexeme] = next;
      node->items.push_back({nt->lexeme, next});
      next++;
      if (!match(COMMA)) break;
    }
    expect(RIGHT_CURLY_BRACE, "'}'");
    s.defined = true;
    s.node = node;
    return ty;
  }

  // Parses the `* name [N] (params)` part. `params` is non null where a function declarator is allowed.
  Type* declarator(Type* base, std::string& name, token_t& nameTok, std::vector<Decl::Param>* params, bool abstractOk) {
    while (check(STAR)) {
      advance();
      base = Types::ptr(base, base->kind != Type::PTR && base->isConst);
      while (check(W_CONST)) advance();          // `* const` : the pointer itself is const, ignored
    }
    // nested declarator: `void (*handler)(uint8_t)`, `uint8_t (*table[4])(void)`
    if (check(LEFT_BRACE) && checkAt(1, STAR)) {
      size_t inner = pos + 1, p = pos;
      int depth = 0;
      do {
        if (toks[p]->token == LEFT_BRACE) depth++;
        else if (toks[p]->token == RIGHT_BRACE) depth--;
        else if (toks[p]->token == T_EOF) fail(cur(), "unbalanced '(' in declarator");
        p++;
      } while (depth > 0);
      pos = p;                                           // the suffixes after `)` belong to the pointee
      std::vector<Decl::Param> dummy;
      Type* t = suffixes(base, &dummy, nullptr);
      size_t after = pos;
      pos = inner;
      Type* r = declarator(t, name, nameTok, nullptr, abstractOk);
      if (pos != p - 1) fail(cur(), "unexpected '" + cur()->lexeme + "' in declarator");
      pos = after;
      return r;
    }

    name.clear();
    nameTok = cur();
    if (check(IDENTIFIER)) { nameTok = cur(); name = advance()->lexeme; }
    else if (!abstractOk) fail(cur(), "expected a name, found '" + cur()->lexeme + "'");

    return suffixes(base, params, params);
  }

  // `(params)` and `[N]...` after a name. `fnParams` non null where a function declarator is allowed here.
  Type* suffixes(Type* base, std::vector<Decl::Param>* fnParams, std::vector<Decl::Param>* paramsHint) {
    (void)paramsHint;
    if (check(LEFT_BRACE)) {
      if (!fnParams) fail(cur(), "unexpected '(' in declarator");
      advance();
      std::vector<Type*> ptypes;
      paramList(*fnParams, ptypes);
      return Types::func(base, ptypes);
    }

    std::vector<std::pair<uint32_t, token_t>> dims;
    while (check(LEFT_SQUARE_BRACE)) {
      token_t b = advance();
      if (check(RIGHT_SQUARE_BRACE)) dims.push_back({0, b});
      else {
        int32_t n = constExpr();
        if (n <= 0 || n > 0xFFFF) fail(b, "bad array size " + std::to_string(n));
        dims.push_back({(uint32_t)n, b});
      }
      expect(RIGHT_SQUARE_BRACE, "']'");
    }
    for (size_t i = dims.size(); i-- > 0;) {
      if (dims[i].first == 0 && i != 0) fail(dims[i].second, "only the first array dimension may be empty");
      if (base->isVoid()) fail(dims[i].second, "array of void");
      if (base->isFunc()) fail(dims[i].second, "array of functions");
      base = Types::array(base, dims[i].first);
    }
    return base;
  }

  void paramList(std::vector<Decl::Param>& out, std::vector<Type*>& types) {
    if (match(RIGHT_BRACE)) return;
    if (check(W_VOID) && checkAt(1, RIGHT_BRACE)) { advance(); advance(); return; }
    for (;;) {
      Spec s = spec();
      if (s.isStatic || s.isExtern || s.isTypedef) fail(cur(), "storage class not allowed on a parameter");
      Decl::Param p;
      p.tok = cur();
      Type* t = declarator(s.type, p.name, p.tok, nullptr, true);
      if (t->isVoid()) fail(p.tok, "parameter declared void");
      if (t->isStruct()) fail(p.tok, "structs cannot be passed by value, pass a pointer");
      if (t->isArray()) t = Types::ptr(t->base, t->base->isConst);       // decays
      p.type = t;
      out.push_back(p);
      types.push_back(t);
      if (match(COMMA)) continue;
      break;
    }
    expect(RIGHT_BRACE, "')' after parameters");
  }

  // -------------------------------------------------------------------------- declarations
  void top(std::vector<stmt_t>& out) {
    if (match(SEMICOLON)) return;
    if (check(ASM_BLOCK)) { out.push_back(asmBlock()); return; }
    if (check(W_ASM)) { out.push_back(asmString()); return; }
    declaration(out, true);
  }

  void declaration(std::vector<stmt_t>& out, bool global) {
    token_t start = cur();
    Spec s = spec();

    if (check(SEMICOLON)) {                 // `struct foo { ... };`  `enum { A, B };`
      advance();
      if (s.node) out.push_back(s.node);
      return;
    }
    if (s.node) out.push_back(s.node);

    for (;;) {
      std::string name; token_t nameTok = cur();
      std::vector<Decl::Param> params;
      Type* t = declarator(s.type, name, nameTok, global ? &params : nullptr, false);

      if (s.isTypedef) {
        if (typedefs.count(name)) {
          if (!Types::same(typedefs[name], t)) fail(nameTok, "conflicting typedef '" + name + "'");
        }
        if (enumConsts.count(name)) fail(nameTok, "'" + name + "' is an enumerator");
        typedefs[name] = t;
        auto n = std::make_shared<Decl::Typedef>();
        n->tok = nameTok; n->name = name; n->type = t;
        out.push_back(n);
      } else if (t->isFunc()) {
        function(out, s, name, nameTok, t, params);
        return;                              // a function ends the declaration
      } else {
        variable(out, s, name, nameTok, t, global);
      }
      if (match(COMMA)) continue;
      break;
    }
    expect(SEMICOLON, "';' after declaration");
    (void)start;
  }

  void variable(std::vector<stmt_t>& out, Spec& s, const std::string& name, const token_t& nameTok, Type* t, bool global) {
    if (t->isVoid()) fail(nameTok, "variable '" + name + "' declared void");
    if (enumConsts.count(name)) fail(nameTok, "'" + name + "' is an enumerator");
    if (typedefs.count(name)) fail(nameTok, "'" + name + "' is a type name");
    if (t->isStruct() && !t->sdef->complete) fail(nameTok, "variable '" + name + "' has incomplete type " + Types::str(t));

    auto v = std::make_shared<Decl::Var>();
    v->tok = nameTok;
    v->name = name;
    v->isStatic = s.isStatic;
    v->isExtern = s.isExtern;
    if (s.isExtern && !global) fail(nameTok, "local extern declarations are not supported");

    if (isIdent("__at")) {
      advance();
      expect(LEFT_BRACE, "'('");
      int32_t a = constExpr();
      expect(RIGHT_BRACE, "')'");
      if (a < 0 || a > 0xFFFF) fail(nameTok, "address out of range");
      if (!global) fail(nameTok, "__at is only allowed on globals");
      v->hasAddr = true; v->addr = (uint32_t)a;
    }

    if (match(ASSIGN)) {
      if (s.isExtern) fail(nameTok, "extern variable cannot have an initializer");
      if (v->hasAddr) fail(nameTok, "a variable with __at cannot have an initializer");
      v->init = initializer();
      // `uint8_t a[] = {1,2,3}`  /  `char s[] = "text"`
      if (t->isArray() && t->count == 0) {
        uint32_t n = 0;
        if (auto* arr = dynamic_cast<Expr::Array*>(v->init.get())) n = (uint32_t)arr->elems.size();
        else if (auto* lit = dynamic_cast<Expr::Literal*>(v->init.get())) {
          if (lit->kind == Expr::Literal::STR) n = (uint32_t)lit->bytes.size() + 1;
        }
        if (n == 0) fail(nameTok, "cannot deduce the size of array '" + name + "'");
        t = Types::array(t->base, n);
      }
    } else if (t->isArray() && t->count == 0 && !s.isExtern) {
      fail(nameTok, "array '" + name + "' needs a size or an initializer");
    }

    v->type = t;
    v->isConst = Types::readOnly(t);
    out.push_back(v);
  }

  expr_t initializer() {
    if (!check(LEFT_CURLY_BRACE)) return assignment();
    token_t open = advance();
    std::vector<expr_t> elems;
    while (!check(RIGHT_CURLY_BRACE) && !check(T_EOF)) {
      elems.push_back(initializer());
      if (!match(COMMA)) break;
    }
    expect(RIGHT_CURLY_BRACE, "'}'");
    auto a = std::make_shared<Expr::Array>(std::move(elems));
    a->tok = open;
    return a;
  }

  static bool isReg(const std::string& r) {
    static const char* regs[] = {"A", "B", "C", "D", "E", "H", "L", "AF", "BC", "DE", "HL", "IX", "F"};
    for (auto x : regs) if (r == x) return true;
    return false;
  }

  std::vector<std::string> regList() {
    std::vector<std::string> r;
    expect(LEFT_BRACE, "'('");
    if (!check(RIGHT_BRACE)) {
      do {
        token_t t = expect(IDENTIFIER, "register name");
        std::string n = Upper(t->lexeme);
        if (!isReg(n)) fail(t, "unknown register '" + t->lexeme + "'");
        r.push_back(n);
      } while (match(COMMA));
    }
    expect(RIGHT_BRACE, "')'");
    return r;
  }

  void bindingClause(Decl::Func* f) {
    // asm("@LABEL", in(HL, A), out(HL), clobber(AF, BC))
    advance();                                           // asm
    expect(LEFT_BRACE, "'('");
    token_t l = expect(STRING, "routine label string");
    f->bind.bound = true;
    f->bind.label = l->literal;
    while (match(COMMA)) {
      token_t kw = expect(IDENTIFIER, "in / out / clobber");
      if (kw->lexeme == "in") f->bind.in = regList();
      else if (kw->lexeme == "out") {
        auto r = regList();
        if (r.size() != 1) fail(kw, "out(...) takes exactly one register");
        f->bind.out = r[0];
      } else if (kw->lexeme == "clobber") { f->bind.clobber = regList(); f->bind.hasClobber = true; }
      else fail(kw, "expected in / out / clobber, found '" + kw->lexeme + "'");
    }
    expect(RIGHT_BRACE, "')'");
  }

  void function(std::vector<stmt_t>& out, Spec& s, const std::string& name, const token_t& nameTok, Type* ft, std::vector<Decl::Param>& params) {
    if (ft->base->isStruct()) fail(nameTok, "functions cannot return structs, use a pointer");
    if (ft->base->isArray()) fail(nameTok, "functions cannot return arrays");

    auto f = std::make_shared<Decl::Func>();
    f->tok = nameTok;
    f->name = name;
    f->ret = ft->base;
    f->type = ft;
    f->params = params;
    f->isStatic = s.isStatic;
    f->isExtern = s.isExtern;
    f->isNaked = s.isNaked;
    f->isInterrupt = s.isInterrupt;

    if (check(W_ASM)) {
      bindingClause(f.get());
      // a definition may only name its label: `void tick() asm("@TIMER_CT1_EXEC") { ... }`
      if (!s.isExtern && (!f->bind.in.empty() || !f->bind.out.empty() || f->bind.hasClobber))
        fail(nameTok, "in(...) / out(...) / clobber(...) are only for extern declarations");
      if (!s.isExtern && !check(LEFT_CURLY_BRACE)) fail(nameTok, "asm(\"label\") on a function that is not extern needs a body");
    }

    if (check(LEFT_CURLY_BRACE)) {
      if (s.isExtern) fail(nameTok, "extern function cannot have a body");
      for (auto& p : f->params) {
        if (p.name.empty()) fail(p.tok, "parameter name omitted in function definition");
        if (enumConsts.count(p.name)) fail(p.tok, "'" + p.name + "' is an enumerator");
      }
      f->body = block();
    } else {
      expect(SEMICOLON, "';' or function body");
    }
    out.push_back(f);
  }

  // -------------------------------------------------------------------------- statements
  std::shared_ptr<Stmt::Block> block() {
    auto b = std::make_shared<Stmt::Block>();
    b->tok = expect(LEFT_CURLY_BRACE, "'{'");
    while (!check(RIGHT_CURLY_BRACE) && !check(T_EOF)) {
      try { statement(b->body); }
      catch (ParseError&) { syncInBlock(); }
    }
    expect(RIGHT_CURLY_BRACE, "'}'");
    return b;
  }

  void syncInBlock() {
    while (!check(T_EOF)) {
      TokenT t = cur()->token;
      if (t == SEMICOLON) { advance(); return; }
      if (t == RIGHT_CURLY_BRACE) return;
      if (t == LEFT_CURLY_BRACE) { int d = 0; do { if (check(LEFT_CURLY_BRACE)) d++; if (check(RIGHT_CURLY_BRACE)) d--; advance(); } while (d > 0 && !check(T_EOF)); return; }
      advance();
    }
  }

  stmt_t asmBlock() {
    token_t t = advance();
    auto a = std::make_shared<Stmt::Asm>(t->literal);
    a->tok = t;
    return a;
  }

  stmt_t asmString() {
    token_t t = advance();
    expect(LEFT_BRACE, "'('");
    std::string text;
    token_t s = expect(STRING, "asm string");
    text = s->literal;
    while (check(STRING)) text += advance()->literal;
    expect(RIGHT_BRACE, "')'");
    expect(SEMICOLON, "';'");
    auto a = std::make_shared<Stmt::Asm>(text);
    a->tok = t;
    return a;
  }

  // a single statement, local declarations may add several
  void statement(std::vector<stmt_t>& out) {
    if (isDeclStart()) { declaration(out, false); return; }
    out.push_back(single());
  }

  stmt_t single() {
    token_t t = cur();
    switch (t->token) {
      case SEMICOLON: { advance(); auto b = std::make_shared<Stmt::Block>(); b->tok = t; return b; }
      case LEFT_CURLY_BRACE: return block();
      case ASM_BLOCK: return asmBlock();
      case W_ASM: return asmString();
      case W_IF: {
        advance();
        auto n = std::make_shared<Stmt::If>(); n->tok = t;
        expect(LEFT_BRACE, "'(' after if");
        n->cond = expression();
        expect(RIGHT_BRACE, "')'");
        n->then = body();
        if (match(W_ELSE)) n->els = body();
        return n;
      }
      case W_WHILE: {
        advance();
        auto n = std::make_shared<Stmt::While>(); n->tok = t;
        expect(LEFT_BRACE, "'(' after while");
        n->cond = expression();
        expect(RIGHT_BRACE, "')'");
        n->body = body();
        return n;
      }
      case W_DO: {
        advance();
        auto n = std::make_shared<Stmt::Do>(); n->tok = t;
        n->body = body();
        expect(W_WHILE, "'while' after do body");
        expect(LEFT_BRACE, "'('");
        n->cond = expression();
        expect(RIGHT_BRACE, "')'");
        expect(SEMICOLON, "';'");
        return n;
      }
      case W_FOR: {
        advance();
        auto n = std::make_shared<Stmt::For>(); n->tok = t;
        expect(LEFT_BRACE, "'(' after for");
        if (!check(SEMICOLON)) {
          if (isDeclStart()) {
            std::vector<stmt_t> d;
            declaration(d, false);                 // eats the ';'
            if (d.size() == 1) n->init = d[0];
            else { auto b = std::make_shared<Stmt::Block>(); b->tok = t; b->body = d; n->init = b; }
          } else {
            auto es = std::make_shared<Stmt::ExprStmt>(expression()); es->tok = t;
            n->init = es;
            expect(SEMICOLON, "';'");
          }
        } else advance();
        if (!check(SEMICOLON)) n->cond = expression();
        expect(SEMICOLON, "';'");
        if (!check(RIGHT_BRACE)) n->step = expression();
        expect(RIGHT_BRACE, "')'");
        n->body = body();
        return n;
      }
      case W_RETURN: {
        advance();
        auto n = std::make_shared<Stmt::Return>(); n->tok = t;
        if (!check(SEMICOLON)) n->value = expression();
        expect(SEMICOLON, "';' after return");
        return n;
      }
      case W_BREAK: { advance(); expect(SEMICOLON, "';'"); auto n = std::make_shared<Stmt::Break>(); n->tok = t; return n; }
      case W_CONTINUE: { advance(); expect(SEMICOLON, "';'"); auto n = std::make_shared<Stmt::Continue>(); n->tok = t; return n; }
      case W_GOTO: {
        advance();
        token_t l = expect(IDENTIFIER, "label name");
        expect(SEMICOLON, "';'");
        auto n = std::make_shared<Stmt::Goto>(l->lexeme); n->tok = t;
        return n;
      }
      case W_SWITCH: {
        advance();
        auto n = std::make_shared<Stmt::Switch>(); n->tok = t;
        expect(LEFT_BRACE, "'(' after switch");
        n->cond = expression();
        expect(RIGHT_BRACE, "')'");
        if (!check(LEFT_CURLY_BRACE)) fail(cur(), "switch needs a { } body");
        n->body = block();
        return n;
      }
      case W_CASE: {
        advance();
        auto n = std::make_shared<Stmt::Case>(); n->tok = t;
        n->cv = constExpr();
        expect(COLON, "':' after case");
        return n;
      }
      case W_DEFAULT: {
        advance();
        expect(COLON, "':' after default");
        auto n = std::make_shared<Stmt::Case>(); n->tok = t; n->isDefault = true;
        return n;
      }
      case IDENTIFIER:
        if (checkAt(1, COLON)) {
          advance(); advance();
          auto n = std::make_shared<Stmt::Label>(t->lexeme); n->tok = t;
          return n;
        }
        break;
      default: break;
    }
    auto es = std::make_shared<Stmt::ExprStmt>(expression());
    es->tok = t;
    expect(SEMICOLON, "';' after expression");
    return es;
  }

  // body of if / while / for: a lone declaration is not allowed, wrap anything into a scope
  stmt_t body() {
    if (isDeclStart()) fail(cur(), "a declaration is not allowed here, use a { } block");
    return single();
  }

  // -------------------------------------------------------------------------- expressions
  expr_t mark(expr_t e, const token_t& t) { e->tok = t; return e; }

  expr_t expression() {
    expr_t e = assignment();
    while (check(COMMA)) {
      token_t t = advance();
      e = mark(std::make_shared<Expr::Comma>(e, assignment()), t);
    }
    return e;
  }

  expr_t assignment() {
    expr_t l = ternary();
    switch (cur()->token) {
      case ASSIGN: case ASSIGN_PLUS: case ASSIGN_MINUS: case ASSIGN_MUL: case ASSIGN_DIV: case ASSIGN_MOD:
      case ASSIGN_BIT_OR: case ASSIGN_BIT_AND: case ASSIGN_BIT_XOR: case ASSIGN_SHL: case ASSIGN_SHR: {
        token_t t = advance();
        expr_t r = assignment();
        return mark(std::make_shared<Expr::Assign>(t->token, l, r), t);
      }
      default: return l;
    }
  }

  expr_t ternary() {
    expr_t c = logicOr();
    if (check(QUESTION_MARK)) {
      token_t t = advance();
      expr_t a = expression();
      expect(COLON, "':' in ?:");
      expr_t b = ternary();
      return mark(std::make_shared<Expr::Ternary>(c, a, b), t);
    }
    return c;
  }

  template <class Next>
  expr_t leftAssoc(std::initializer_list<TokenT> ops, Next next) {
    expr_t l = (this->*next)();
    for (;;) {
      bool hit = false;
      for (TokenT o : ops) if (check(o)) { hit = true; break; }
      if (!hit) return l;
      token_t t = advance();
      expr_t r = (this->*next)();
      l = mark(std::make_shared<Expr::Binary>(t->token, l, r), t);
    }
  }

  expr_t logicOr()  { return leftAssoc({OP_OR}, &Parser::logicAnd); }
  expr_t logicAnd() { return leftAssoc({OP_AND}, &Parser::bitOr); }
  expr_t bitOr()    { return leftAssoc({BIT_OR}, &Parser::bitXor); }
  expr_t bitXor()   { return leftAssoc({BIT_XOR}, &Parser::bitAnd); }
  expr_t bitAnd()   { return leftAssoc({AMPERSAND}, &Parser::equality); }
  expr_t equality() { return leftAssoc({OP_EQUAL, OP_NOT_EQUAL}, &Parser::relational); }
  expr_t relational() { return leftAssoc({OP_LESS, OP_LESS_OR_EQUAL, OP_MORE, OP_MORE_OR_EQUAL}, &Parser::shift); }
  expr_t shift()    { return leftAssoc({BIT_LEFT_SHIFT, BIT_RIGHT_SHIFT}, &Parser::additive); }
  expr_t additive() { return leftAssoc({OP_PLUS, OP_MINUS}, &Parser::multiplicative); }
  expr_t multiplicative() { return leftAssoc({STAR, OP_DIV, OP_MOD}, &Parser::unary); }

  // `(` type `)` : a type name at the current position?
  bool castAhead() const { return check(LEFT_BRACE) && isTypeStart(1); }

  Type* typeName() {
    Spec s = spec();
    if (s.isStatic || s.isExtern || s.isTypedef) fail(cur(), "storage class in a type name");
    std::string n; token_t nt = cur();
    return declarator(s.type, n, nt, nullptr, true);
  }

  expr_t unary() {
    token_t t = cur();
    switch (t->token) {
      case OP_MINUS: case OP_PLUS: case OP_NOT: case BIT_NOT: case STAR: case AMPERSAND: case OP_INC: case OP_DEC: {
        advance();
        expr_t e = unary();
        return mark(std::make_shared<Expr::Unary>(t->token, e), t);
      }
      case W_SIZEOF: {
        advance();
        if (castAhead()) {
          advance();
          Type* ty = typeName();
          expect(RIGHT_BRACE, "')'");
          return mark(std::make_shared<Expr::Sizeof>(ty, nullptr), t);
        }
        return mark(std::make_shared<Expr::Sizeof>(nullptr, unary()), t);
      }
      case LEFT_BRACE:
        if (castAhead()) {
          advance();
          Type* ty = typeName();
          expect(RIGHT_BRACE, "')' after cast type");
          return mark(std::make_shared<Expr::Cast>(ty, unary()), t);
        }
        break;
      default: break;
    }
    return postfix();
  }

  expr_t postfix() {
    expr_t e = primary();
    for (;;) {
      token_t t = cur();
      if (t->token == LEFT_SQUARE_BRACE) {
        advance();
        expr_t i = expression();
        expect(RIGHT_SQUARE_BRACE, "']'");
        e = mark(std::make_shared<Expr::Index>(e, i), t);
      } else if (t->token == LEFT_BRACE) {
        advance();
        std::vector<expr_t> args;
        if (!check(RIGHT_BRACE)) {
          do args.push_back(assignment()); while (match(COMMA));
        }
        expect(RIGHT_BRACE, "')' after arguments");
        e = mark(std::make_shared<Expr::Invoke>(e, args), t);
      } else if (t->token == OP_DOT || t->token == OP_ARROW) {
        advance();
        token_t n = expect(IDENTIFIER, "member name");
        e = mark(std::make_shared<Expr::Field>(e, n->lexeme, t->token == OP_ARROW), t);
      } else if (t->token == OP_INC || t->token == OP_DEC) {
        advance();
        e = mark(std::make_shared<Expr::Suffix>(t->token, e), t);
      } else return e;
    }
  }

  expr_t primary() {
    token_t t = cur();
    switch (t->token) {
      case NUMBER: {
        advance();
        long long v = strtoll(t->literal.c_str(), nullptr, 10);
        if (v > 0xFFFFFFFFLL) fail(t, "number too large");
        auto n = std::make_shared<Expr::Literal>(Expr::Literal::NUM, (int32_t)v);
        char last = t->lexeme.empty() ? 0 : t->lexeme.back();
        n->forceUnsigned = (last == 'u' || last == 'U');
        return mark(n, t);
      }
      case CHAR: {
        advance();
        return mark(std::make_shared<Expr::Literal>(Expr::Literal::CHR, atoi(t->literal.c_str())), t);
      }
      case STRING: {
        advance();
        auto n = std::make_shared<Expr::Literal>(Expr::Literal::STR, 0);
        n->bytes = t->literal;
        while (check(STRING)) n->bytes += advance()->literal;      // "a" "b"
        return mark(n, t);
      }
      case IDENTIFIER: {
        advance();
        auto it = enumConsts.find(t->lexeme);
        if (it != enumConsts.end()) return mark(std::make_shared<Expr::Literal>(Expr::Literal::NUM, it->second), t);
        return mark(std::make_shared<Expr::Var>(t->lexeme), t);
      }
      case LEFT_BRACE: {
        advance();
        expr_t e = expression();
        expect(RIGHT_BRACE, "')'");
        return mark(std::make_shared<Expr::Group>(e), t);
      }
      default:
        fail(t, "expected an expression, found '" + (t->token == T_EOF ? std::string("end of file") : t->lexeme) + "'");
    }
  }

  // -------------------------------------------------------------------------- constant folding
  int32_t constExpr() {
    token_t t = cur();
    expr_t e = ternary();
    int32_t v = 0;
    if (!fold(e, v)) fail(t, "expected a constant expression");
    return v;
  }

  static int32_t truncTo(int32_t v, const Type* t) {
    switch (t->kind) {
      case Type::U8:  return v & 0xFF;
      case Type::I8:  return (int8_t)v;
      case Type::U16: return v & 0xFFFF;
      case Type::I16: return (int16_t)v;
      case Type::PTR: return v & 0xFFFF;
      default: return v;
    }
  }

  bool fold(const expr_t& e, int32_t& v) {
    if (auto* l = dynamic_cast<Expr::Literal*>(e.get())) {
      if (l->kind == Expr::Literal::STR) return false;
      v = l->value; return true;
    }
    if (auto* g = dynamic_cast<Expr::Group*>(e.get())) return fold(g->e, v);
    if (auto* c = dynamic_cast<Expr::Cast*>(e.get())) {
      if (!c->to->isScalar() || !fold(c->e, v)) return false;
      v = truncTo(v, c->to);
      return true;
    }
    if (auto* s = dynamic_cast<Expr::Sizeof*>(e.get())) {
      if (!s->of) return false;
      v = (int32_t)s->of->size();
      return true;
    }
    if (auto* u = dynamic_cast<Expr::Unary*>(e.get())) {
      int32_t a;
      if (!fold(u->e, a)) return false;
      switch (u->op) {
        case OP_MINUS: v = -a; return true;
        case OP_PLUS:  v = a; return true;
        case BIT_NOT:  v = ~a; return true;
        case OP_NOT:   v = !a; return true;
        default: return false;
      }
    }
    if (auto* t = dynamic_cast<Expr::Ternary*>(e.get())) {
      int32_t c, a, b;
      if (!fold(t->cond, c) || !fold(t->a, a) || !fold(t->b, b)) return false;
      v = c ? a : b;
      return true;
    }
    if (auto* b = dynamic_cast<Expr::Binary*>(e.get())) {
      int32_t x, y;
      if (!fold(b->l, x) || !fold(b->r, y)) return false;
      switch (b->op) {
        case OP_PLUS: v = x + y; return true;
        case OP_MINUS: v = x - y; return true;
        case STAR: v = x * y; return true;
        case OP_DIV: case OP_MOD:
          if (y == 0) { Diag::error(b->tok->line, b->tok->col, "division by zero in constant expression"); throw ParseError(); }
          v = b->op == OP_DIV ? x / y : x % y; return true;
        case BIT_LEFT_SHIFT: v = (y < 0 || y > 31) ? 0 : (int32_t)((uint32_t)x << y); return true;
        case BIT_RIGHT_SHIFT: v = (y < 0 || y > 31) ? 0 : x >> y; return true;
        case AMPERSAND: v = x & y; return true;
        case BIT_OR: v = x | y; return true;
        case BIT_XOR: v = x ^ y; return true;
        case OP_AND: v = x && y; return true;
        case OP_OR: v = x || y; return true;
        case OP_EQUAL: v = x == y; return true;
        case OP_NOT_EQUAL: v = x != y; return true;
        case OP_LESS: v = x < y; return true;
        case OP_LESS_OR_EQUAL: v = x <= y; return true;
        case OP_MORE: v = x > y; return true;
        case OP_MORE_OR_EQUAL: v = x >= y; return true;
        default: return false;
      }
    }
    return false;
  }
};
};
