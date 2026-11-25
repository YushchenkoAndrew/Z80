#pragma once
#include <map>
#include <set>
#include <filesystem>
#include "src/Utils/Diag.h"

namespace Zazy {

/**
 * Text level preprocessor, runs before the lexer. It knows
 *
 *   #include "file" / <file>        (searched next to the including file, then in -I dirs)
 *   #include "x.z80"                (not read: remembered and emitted as INCLUDE in the asm output)
 *   #define NAME body               object like macro
 *   #define NAME(a, b, ...) body    function like macro, with `#a`, `a ## b` and __VA_ARGS__
 *   #undef  #if #ifdef #ifndef #elif #else #endif  (with defined(X) and C integer expressions)
 *   #pragma once  #error  #warning
 *   __FILE__ __LINE__ __ZAZY__ __Z80__
 *
 * Comments are stripped here (newlines are kept), so line numbers still match. Every output line
 * is recorded in Diag::origins() so later stages can report the real file and line.
 * Text inside `asm { ... }` is never macro expanded.
 */
class Preprocessor {
public:
  struct Macro {
    bool fn = false;
    bool variadic = false;
    std::vector<std::string> params;
    std::string body;
  };

public:
  Preprocessor() {
    define("__ZAZY__", "1");
    define("__Z80__", "1");
  }

  std::vector<std::string> includeDirs;
  std::vector<std::string> asmIncludes;        // .z80 files to INCLUDE in the generated asm
  std::vector<std::string> preludes;           // files processed before the main file (the standard header)
  std::map<std::string, Macro> macros;

  void define(const std::string& name, const std::string& body = "1") {
    Macro m; m.body = body; macros[name] = m;
  }

  bool processFile(const std::string& path, std::string& text) {
    bool ok = false;
    std::string src = ReadFile(path, &ok);
    if (!ok) { Diag::plain(path + ": error: cannot read file"); Diag::errorCount()++; return false; }
    return processText(path, src, text);
  }

  bool processText(const std::string& name, const std::string& src, std::string& text) {
    int before = Diag::errorCount();
    Diag::origins().clear();
    text.clear();
    conds.clear();
    once.clear();
    for (auto& p : preludes) {
      bool ok = false;
      std::string psrc = ReadFile(p, &ok);
      if (!ok) { Diag::plain(p + ": error: cannot read file"); Diag::errorCount()++; continue; }
      file(p, psrc, text, 0);
    }
    file(name, src, text, 0);
    return Diag::errorCount() == before;
  }

private:
  struct Cond { bool active; bool taken; bool seenElse; bool parent; };

  std::vector<Cond> conds;
  std::set<std::string> once;
  std::string curFile;
  int curLine = 0;
  int asmDepth = 0;

  bool active() const { return conds.empty() || conds.back().active; }

  void err(const std::string& f, int line, const std::string& msg) {
    Diag::plain(f + ":" + std::to_string(line) + ": error: " + msg);
    Diag::errorCount()++;
  }
  void warn(const std::string& f, int line, const std::string& msg) {
    Diag::plain(f + ":" + std::to_string(line) + ": warning: " + msg);
  }

  void emit(std::string& out, const std::string& s, const std::string& f, int line) {
    out += s; out += '\n';
    Diag::origins().push_back({f, line});
  }

  // ---------------------------------------------------------------------------------- comments
  static std::string strip(const std::string& s, bool& inBlock) {
    std::string r;
    size_t i = 0;
    while (i < s.size()) {
      if (inBlock) {
        if (s[i] == '*' && i + 1 < s.size() && s[i + 1] == '/') { inBlock = false; i += 2; r += ' '; }
        else i++;
        continue;
      }
      char c = s[i];
      if (c == '"' || c == '\'') {
        char q = c; r += c; i++;
        while (i < s.size()) {
          char d = s[i]; r += d; i++;
          if (d == '\\' && i < s.size()) { r += s[i++]; continue; }
          if (d == q) break;
        }
        continue;
      }
      if (c == '/' && i + 1 < s.size() && s[i + 1] == '/') break;
      if (c == '/' && i + 1 < s.size() && s[i + 1] == '*') { inBlock = true; i += 2; continue; }
      r += c; i++;
    }
    return r;
  }

  // ---------------------------------------------------------------------------------- files
  void file(const std::string& name, const std::string& src, std::string& out, int depth) {
    if (depth > 64) { err(name, 1, "#include nested too deep"); return; }

    std::vector<std::string> lines;
    {
      std::string cur;
      for (char c : src) {
        if (c == '\n') { lines.push_back(cur); cur.clear(); }
        else if (c != '\r') cur += c;
      }
      lines.push_back(cur);
    }

    const size_t condBase = conds.size();
    const std::string savedFile = curFile;
    const int savedLine = curLine;
    const int savedAsm = asmDepth;
    curFile = name;
    asmDepth = 0;
    bool inBlock = false;

    for (size_t i = 0; i < lines.size(); i++) {
      const int lineNo = (int)i + 1;
      std::string raw = lines[i];
      while (!raw.empty() && raw.back() == '\\' && i + 1 < lines.size()) { raw.pop_back(); raw += lines[++i]; }
      curLine = lineNo;

      std::string line = strip(raw, inBlock);
      std::string t = Trim(line);

      if (!t.empty() && t[0] == '#' && asmDepth == 0) {
        directive(t.substr(1), name, lineNo, out, depth);
        continue;
      }
      if (!active()) continue;
      emit(out, expandLine(line), name, lineNo);
    }

    if (inBlock) err(name, (int)lines.size(), "unterminated /* comment");
    if (conds.size() != condBase) {
      err(name, (int)lines.size(), "unterminated #if");
      conds.resize(condBase);
    }
    curFile = savedFile; curLine = savedLine; asmDepth = savedAsm;
  }

  bool resolve(const std::string& name, bool angle, const std::string& from, std::string& found) {
    namespace fs = std::filesystem;
    std::vector<fs::path> dirs;
    fs::path here = fs::path(from).parent_path();
    if (!angle) dirs.push_back(here);
    for (auto& d : includeDirs) dirs.push_back(d);
    if (angle) dirs.push_back(here);
    for (auto& d : dirs) {
      fs::path p = d / name;
      std::error_code ec;
      if (fs::is_regular_file(p, ec)) { found = p.lexically_normal().string(); return true; }
    }
    return false;
  }

  static bool isAsmFile(const std::string& n) {
    auto dot = n.rfind('.');
    if (dot == std::string::npos) return false;
    std::string e = Upper(n.substr(dot));
    return e == ".Z80" || e == ".ASM" || e == ".INC";
  }

  // ---------------------------------------------------------------------------------- directives
  void directive(const std::string& d, const std::string& f, int line, std::string& out, int depth) {
    size_t i = 0;
    while (i < d.size() && (d[i] == ' ' || d[i] == '\t')) i++;
    size_t s = i;
    while (i < d.size() && IsAlpha(d[i])) i++;
    std::string word = d.substr(s, i - s);
    std::string rest = Trim(d.substr(i));

    // conditionals are tracked even inside inactive regions
    if (word == "if" || word == "ifdef" || word == "ifndef") {
      Cond c; c.parent = active(); c.seenElse = false;
      bool v = false;
      if (c.parent) {
        if (word == "if") v = evalIf(rest, f, line) != 0;
        else {
          std::string n = firstIdent(rest);
          if (n.empty()) err(f, line, "#" + word + " needs a name");
          v = macros.count(n) > 0;
          if (word == "ifndef") v = !v;
        }
      }
      c.active = c.parent && v; c.taken = c.active;
      conds.push_back(c);
      return;
    }
    if (word == "elif") {
      if (conds.empty() || conds.back().seenElse) { err(f, line, "#elif without #if"); return; }
      Cond& c = conds.back();
      if (c.taken || !c.parent) c.active = false;
      else { c.active = evalIf(rest, f, line) != 0; c.taken = c.active; }
      return;
    }
    if (word == "else") {
      if (conds.empty() || conds.back().seenElse) { err(f, line, "#else without #if"); return; }
      Cond& c = conds.back();
      c.seenElse = true;
      c.active = c.parent && !c.taken;
      c.taken = true;
      return;
    }
    if (word == "endif") {
      if (conds.empty()) { err(f, line, "#endif without #if"); return; }
      conds.pop_back();
      return;
    }

    if (!active()) return;

    if (word == "define") { define_(rest, f, line); return; }
    if (word == "undef") { macros.erase(firstIdent(rest)); return; }
    if (word == "error") { err(f, line, "#error " + rest); return; }
    if (word == "warning") { warn(f, line, rest); return; }
    if (word == "pragma") {
      if (rest == "once") {
        std::error_code ec;
        once.insert(std::filesystem::weakly_canonical(f, ec).string());
      }
      return;
    }
    if (word == "line" || word.empty()) return;

    if (word == "include") {
      std::string e = expandLine(rest);
      e = Trim(e);
      if (e.size() < 3 || (e[0] != '"' && e[0] != '<')) { err(f, line, "bad #include"); return; }
      bool angle = e[0] == '<';
      auto close = e.find(angle ? '>' : '"', 1);
      if (close == std::string::npos) { err(f, line, "bad #include"); return; }
      std::string nm = e.substr(1, close - 1);

      if (isAsmFile(nm)) {
        if (std::find(asmIncludes.begin(), asmIncludes.end(), nm) == asmIncludes.end()) asmIncludes.push_back(nm);
        return;
      }
      std::string found;
      if (!resolve(nm, angle, f, found)) { err(f, line, "cannot find include file '" + nm + "'"); return; }
      std::error_code ec;
      if (once.count(std::filesystem::weakly_canonical(found, ec).string())) return;
      bool ok = false;
      std::string src = ReadFile(found, &ok);
      if (!ok) { err(f, line, "cannot read '" + found + "'"); return; }
      file(found, src, out, depth + 1);
      return;
    }
    err(f, line, "unknown directive #" + word);
  }

  static std::string firstIdent(const std::string& s) {
    size_t i = 0;
    while (i < s.size() && (s[i] == ' ' || s[i] == '\t')) i++;
    size_t b = i;
    while (i < s.size() && IsAlphaDigit(s[i])) i++;
    return s.substr(b, i - b);
  }

  void define_(const std::string& rest, const std::string& f, int line) {
    size_t i = 0;
    while (i < rest.size() && IsAlphaDigit(rest[i])) i++;
    std::string name = rest.substr(0, i);
    if (name.empty() || IsDigit(name[0])) { err(f, line, "#define needs a name"); return; }
    Macro m;
    if (i < rest.size() && rest[i] == '(') {
      m.fn = true;
      size_t close = rest.find(')', i);
      if (close == std::string::npos) { err(f, line, "missing ) in #define"); return; }
      std::string plist = rest.substr(i + 1, close - i - 1);
      size_t p = 0;
      while (p <= plist.size()) {
        size_t c = plist.find(',', p);
        std::string a = Trim(plist.substr(p, c == std::string::npos ? std::string::npos : c - p));
        if (a == "...") { m.variadic = true; m.params.push_back("__VA_ARGS__"); }
        else if (!a.empty()) m.params.push_back(a);
        if (c == std::string::npos) break;
        p = c + 1;
      }
      i = close + 1;
    }
    m.body = Trim(rest.substr(i));
    macros[name] = m;
  }

  // ---------------------------------------------------------------------------------- expansion
  std::string expandLine(const std::string& s) {
    std::string r;
    size_t i = 0;
    std::set<std::string> hide;
    while (i < s.size()) {
      char c = s[i];
      if (asmDepth > 0) {
        if (c == ';') { r += s.substr(i); break; }
        if (c == '"' || c == '\'') { copyString(s, i, r); continue; }
        if (c == '{') asmDepth++;
        if (c == '}') asmDepth--;
        r += c; i++;
        continue;
      }
      if (c == '"' || c == '\'') { copyString(s, i, r); continue; }
      if (IsDigit(c)) { while (i < s.size() && IsAlphaDigit(s[i])) r += s[i++]; continue; }
      if (IsAlpha(c)) {
        size_t b = i;
        while (i < s.size() && IsAlphaDigit(s[i])) i++;
        std::string id = s.substr(b, i - b);
        if (id == "asm") {
          size_t j = i;
          while (j < s.size() && (s[j] == ' ' || s[j] == '\t')) j++;
          if (j < s.size() && s[j] == '{') {
            r += s.substr(b, j - b) + "{";
            asmDepth = 1;
            i = j + 1;
            continue;
          }
        }
        i = b;
        tryExpand(s, i, hide, r);
        continue;
      }
      r += c; i++;
    }
    return r;
  }

  static void copyString(const std::string& s, size_t& i, std::string& r) {
    char q = s[i]; r += q; i++;
    while (i < s.size()) {
      char d = s[i]; r += d; i++;
      if (d == '\\' && i < s.size()) { r += s[i++]; continue; }
      if (d == q) break;
    }
  }

  std::string expand(const std::string& s, std::set<std::string>& hide) {
    std::string r;
    size_t i = 0;
    while (i < s.size()) {
      char c = s[i];
      if (c == '"' || c == '\'') { copyString(s, i, r); continue; }
      if (IsDigit(c)) { while (i < s.size() && IsAlphaDigit(s[i])) r += s[i++]; continue; }
      if (IsAlpha(c)) { tryExpand(s, i, hide, r); continue; }
      r += c; i++;
    }
    return r;
  }

  // `i` points at the first char of an identifier
  void tryExpand(const std::string& s, size_t& i, std::set<std::string>& hide, std::string& out) {
    size_t b = i;
    while (i < s.size() && IsAlphaDigit(s[i])) i++;
    std::string id = s.substr(b, i - b);

    if (id == "__LINE__") { out += std::to_string(curLine); return; }
    if (id == "__FILE__") { out += "\"" + curFile + "\""; return; }

    auto it = macros.find(id);
    if (it == macros.end() || hide.count(id)) { out += id; return; }
    const Macro& m = it->second;

    if (!m.fn) {
      hide.insert(id);
      out += expand(m.body, hide);
      hide.erase(id);
      return;
    }

    size_t j = i;
    while (j < s.size() && (s[j] == ' ' || s[j] == '\t')) j++;
    if (j >= s.size() || s[j] != '(') { out += id; return; }

    // collect args
    std::vector<std::string> args;
    std::string cur;
    int depth = 0;
    j++;
    bool closed = false;
    for (; j < s.size(); j++) {
      char c = s[j];
      if (c == '"' || c == '\'') { copyString(s, j, cur); j--; continue; }
      if (c == '(') depth++;
      if (c == ')') {
        if (depth == 0) { closed = true; break; }
        depth--;
      }
      if (c == ',' && depth == 0 && !(m.variadic && args.size() + 1 >= m.params.size())) {
        args.push_back(Trim(cur)); cur.clear(); continue;
      }
      cur += c;
    }
    if (!closed) { err(curFile, curLine, "unterminated argument list for macro " + id); out += id; return; }
    args.push_back(Trim(cur));
    i = j + 1;

    if (m.params.empty() && args.size() == 1 && args[0].empty()) args.clear();
    if (m.variadic && args.size() + 1 == m.params.size()) args.push_back("");
    if (args.size() != m.params.size()) {
      err(curFile, curLine, "macro " + id + " expects " + std::to_string(m.params.size()) + " argument(s), got " + std::to_string(args.size()));
      return;
    }

    // expanded copies of the args (used when not next to # / ##)
    std::vector<std::string> exp(args.size());
    for (size_t k = 0; k < args.size(); k++) exp[k] = expand(args[k], hide);

    auto paramIndex = [&](const std::string& n) -> int {
      for (size_t k = 0; k < m.params.size(); k++) if (m.params[k] == n) return (int)k;
      return -1;
    };

    std::string r;
    const std::string& body = m.body;
    size_t p = 0;
    while (p < body.size()) {
      char c = body[p];
      if (c == '"' || c == '\'') { copyString(body, p, r); continue; }
      if (c == '#' && p + 1 < body.size() && body[p + 1] == '#') {
        while (!r.empty() && (r.back() == ' ' || r.back() == '\t')) r.pop_back();
        p += 2;
        while (p < body.size() && (body[p] == ' ' || body[p] == '\t')) p++;
        pasteNext = true;
        continue;
      }
      if (c == '#') {
        size_t q = p + 1;
        while (q < body.size() && (body[q] == ' ' || body[q] == '\t')) q++;
        size_t e = q;
        while (e < body.size() && IsAlphaDigit(body[e])) e++;
        int k = paramIndex(body.substr(q, e - q));
        if (k >= 0) {
          std::string a = args[k], z = "\"";
          for (char ch : a) { if (ch == '"' || ch == '\\') z += '\\'; z += ch; }
          z += "\"";
          r += z;
          p = e;
          pasteNext = false;
          continue;
        }
      }
      if (IsDigit(c)) { while (p < body.size() && IsAlphaDigit(body[p])) r += body[p++]; pasteNext = false; continue; }
      if (IsAlpha(c)) {
        size_t e = p;
        while (e < body.size() && IsAlphaDigit(body[e])) e++;
        std::string w = body.substr(p, e - p);
        int k = paramIndex(w);
        if (k >= 0) {
          size_t a = e;
          while (a < body.size() && (body[a] == ' ' || body[a] == '\t')) a++;
          bool before = pasteNext;
          bool after = a + 1 < body.size() && body[a] == '#' && body[a + 1] == '#';
          r += (before || after) ? args[k] : exp[k];
        } else r += w;
        pasteNext = false;
        p = e;
        continue;
      }
      r += c; p++;
      pasteNext = false;
    }
    pasteNext = false;

    hide.insert(id);
    out += expand(r, hide);
    hide.erase(id);
  }

  bool pasteNext = false;

  // ---------------------------------------------------------------------------------- #if
  struct Tk { int kind; long long v; std::string op; };   // kind 0 num, 1 op, 2 end

  std::vector<Tk> toks;
  size_t tp = 0;
  bool evalErr = false;

  long long evalIf(const std::string& expr, const std::string& f, int line) {
    // defined(X) / defined X first, then macro expansion, unknown identifiers are 0
    std::string pre;
    for (size_t i = 0; i < expr.size();) {
      if (IsAlpha(expr[i])) {
        size_t b = i;
        while (i < expr.size() && IsAlphaDigit(expr[i])) i++;
        std::string id = expr.substr(b, i - b);
        if (id == "defined") {
          size_t j = i;
          while (j < expr.size() && expr[j] == ' ') j++;
          bool par = j < expr.size() && expr[j] == '(';
          if (par) j++;
          while (j < expr.size() && expr[j] == ' ') j++;
          size_t nb = j;
          while (j < expr.size() && IsAlphaDigit(expr[j])) j++;
          std::string n = expr.substr(nb, j - nb);
          if (par) { while (j < expr.size() && expr[j] == ' ') j++; if (j < expr.size() && expr[j] == ')') j++; }
          pre += macros.count(n) ? " 1 " : " 0 ";
          i = j;
        } else pre += id;
      } else if (IsDigit(expr[i])) {
        while (i < expr.size() && IsAlphaDigit(expr[i])) pre += expr[i++];
      } else pre += expr[i++];
    }
    std::set<std::string> hide;
    std::string e = expand(pre, hide);

    toks.clear(); tp = 0; evalErr = false;
    for (size_t i = 0; i < e.size();) {
      char c = e[i];
      if (c == ' ' || c == '\t') { i++; continue; }
      if (IsDigit(c)) {
        size_t b = i;
        while (i < e.size() && IsAlphaDigit(e[i])) i++;
        std::string n = e.substr(b, i - b);
        while (!n.empty() && (n.back() == 'u' || n.back() == 'U' || n.back() == 'l' || n.back() == 'L')) n.pop_back();
        toks.push_back({0, (long long)strtoull(n.c_str(), nullptr, 0), ""});
        continue;
      }
      if (c == '\'' && i + 2 < e.size() && e[i + 2] == '\'') { toks.push_back({0, (unsigned char)e[i + 1], ""}); i += 3; continue; }
      if (IsAlpha(c)) {
        while (i < e.size() && IsAlphaDigit(e[i])) i++;
        toks.push_back({0, 0, ""});
        continue;
      }
      static const char* two[] = {"&&", "||", "==", "!=", "<=", ">=", "<<", ">>"};
      bool done = false;
      for (auto t : two) if (e.compare(i, 2, t) == 0) { toks.push_back({1, 0, t}); i += 2; done = true; break; }
      if (done) continue;
      toks.push_back({1, 0, std::string(1, c)});
      i++;
    }
    toks.push_back({2, 0, ""});

    long long v = cond();
    if (evalErr || toks[tp].kind != 2) { err(f, line, "bad #if expression"); return 0; }
    return v;
  }

  bool isOp(const char* o) { return toks[tp].kind == 1 && toks[tp].op == o; }

  long long cond() {
    long long c = lor();
    if (isOp("?")) {
      tp++;
      long long a = cond();
      if (!isOp(":")) { evalErr = true; return 0; }
      tp++;
      long long b = cond();
      return c ? a : b;
    }
    return c;
  }
  long long lor()  { long long v = land();  while (isOp("||")) { tp++; long long r = land();  v = (v || r); } return v; }
  long long land() { long long v = bor();   while (isOp("&&")) { tp++; long long r = bor();   v = (v && r); } return v; }
  long long bor()  { long long v = bxor();  while (isOp("|"))  { tp++; v |= bxor(); } return v; }
  long long bxor() { long long v = band();  while (isOp("^"))  { tp++; v ^= band(); } return v; }
  long long band() { long long v = eq();    while (isOp("&"))  { tp++; v &= eq(); } return v; }
  long long eq() {
    long long v = rel();
    while (isOp("==") || isOp("!=")) { bool e = isOp("=="); tp++; long long r = rel(); v = e ? v == r : v != r; }
    return v;
  }
  long long rel() {
    long long v = shf();
    while (isOp("<") || isOp(">") || isOp("<=") || isOp(">=")) {
      std::string o = toks[tp++].op; long long r = shf();
      v = o == "<" ? v < r : o == ">" ? v > r : o == "<=" ? v <= r : v >= r;
    }
    return v;
  }
  long long shf() {
    long long v = add();
    while (isOp("<<") || isOp(">>")) { bool l = isOp("<<"); tp++; long long r = add(); v = l ? v << r : v >> r; }
    return v;
  }
  long long add() {
    long long v = mul();
    while (isOp("+") || isOp("-")) { bool p = isOp("+"); tp++; long long r = mul(); v = p ? v + r : v - r; }
    return v;
  }
  long long mul() {
    long long v = unary();
    while (isOp("*") || isOp("/") || isOp("%")) {
      std::string o = toks[tp++].op; long long r = unary();
      if (o != "*" && r == 0) { evalErr = true; return 0; }
      v = o == "*" ? v * r : o == "/" ? v / r : v % r;
    }
    return v;
  }
  long long unary() {
    if (isOp("!")) { tp++; return !unary(); }
    if (isOp("~")) { tp++; return ~unary(); }
    if (isOp("-")) { tp++; return -unary(); }
    if (isOp("+")) { tp++; return unary(); }
    if (isOp("(")) {
      tp++;
      long long v = cond();
      if (!isOp(")")) { evalErr = true; return 0; }
      tp++;
      return v;
    }
    if (toks[tp].kind == 0) return toks[tp++].v;
    evalErr = true;
    return 0;
  }
};
};
