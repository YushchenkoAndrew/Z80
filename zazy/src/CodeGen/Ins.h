#pragma once
#include "src/Utils/Utils.h"

namespace Zazy {

/**
 * One line of the generated function body. Kept structured (instead of plain text) so the
 * peephole pass can look at operands and the branch relaxation can compute real addresses.
 */
struct Ins {
  enum Kind { NORMAL, LABEL, BRANCH, RAW };

  Kind kind = NORMAL;
  std::string op;          // mnemonic, label name (LABEL) or raw text (RAW)
  std::string a, b;        // operands; BRANCH: a = condition ("" for none), b = target label
  std::string cmt;         // trailing comment
  bool isLong = false;     // BRANCH: JP instead of JR
};

namespace InsUtil {

inline bool isReg8(const std::string& s) {
  return s == "A" || s == "B" || s == "C" || s == "D" || s == "E" || s == "H" || s == "L";
}
inline bool isReg16(const std::string& s) {
  return s == "BC" || s == "DE" || s == "HL" || s == "SP" || s == "AF" || s == "IX" || s == "IY";
}
inline bool isIdx(const std::string& s) { return s.size() > 4 && s[0] == '(' && (s.compare(1, 2, "IX") == 0 || s.compare(1, 2, "IY") == 0); }
inline bool isMem(const std::string& s) { return s.size() >= 2 && s[0] == '(' && s.back() == ')'; }
inline bool isRegInd(const std::string& s) { return s == "(HL)" || s == "(BC)" || s == "(DE)" || s == "(SP)" || s == "(C)"; }

// size in bytes of a NORMAL instruction, -1 when unknown
inline int size(const std::string& op, const std::string& a, const std::string& b) {
  const std::string& o = op;
  if (o == "NOP" || o == "HALT" || o == "DI" || o == "EI" || o == "CPL" || o == "SCF" || o == "CCF" || o == "DAA" ||
      o == "RLA" || o == "RRA" || o == "RLCA" || o == "RRCA" || o == "EXX" || o == "RET") return 1;
  if (o == "RETI" || o == "RETN" || o == "NEG" || o == "LDIR" || o == "LDDR" || o == "LDI" || o == "LDD" || o == "CPIR" || o == "CPDR" ||
      o == "RLD" || o == "RRD" || o == "INIR" || o == "OTIR") return 2;
  if (o == "RST") return 1;
  if (o == "EX") return (a == "DE" || a == "(SP)" || a == "AF") && !isIdx(b) ? 1 : -1;
  if (o == "PUSH" || o == "POP") return (a == "IX" || a == "IY") ? 2 : 1;
  if (o == "CALL") return 3;
  if (o == "JP") return (a == "(HL)" && b.empty()) ? 1 : 3;
  if (o == "IN" || o == "OUT") return (a == "(C)" || b == "(C)") ? 2 : 2;
  if (o == "RL" || o == "RR" || o == "RLC" || o == "RRC" || o == "SLA" || o == "SRA" || o == "SRL" || o == "SLL")
    return isIdx(a) ? 4 : 2;
  if (o == "BIT" || o == "SET" || o == "RES") return isIdx(b) ? 4 : 2;
  if (o == "INC" || o == "DEC") {
    if (isIdx(a)) return 3;
    if (a == "IX" || a == "IY") return 2;
    return 1;
  }
  if (o == "ADD" || o == "ADC" || o == "SBC") {
    if (a == "HL") return (o == "ADD") ? 1 : 2;
    if (a == "IX" || a == "IY") return 2;
    if (a == "A") {
      if (isIdx(b)) return 3;
      if (isReg8(b) || b == "(HL)") return 1;
      return 2;
    }
    return -1;
  }
  if (o == "SUB" || o == "AND" || o == "OR" || o == "XOR" || o == "CP") {
    if (isIdx(a)) return 3;
    if (isReg8(a) || a == "(HL)") return 1;
    return 2;
  }
  if (o == "LD") {
    // 8 bit
    if (isReg8(a) || a == "(HL)" || a == "(BC)" || a == "(DE)") {
      if (isReg8(b) || b == "(HL)" || b == "(BC)" || b == "(DE)") return 1;
      if (isIdx(b)) return 3;
      if (isMem(b)) return 3;                        // LD A,(nn)
      return 2;                                      // LD r,n  LD (HL),n
    }
    if (isIdx(a)) return isReg8(b) ? 3 : 4;
    if (isMem(a)) {                                  // LD (nn),A / (nn),HL / (nn),DE ...
      if (b == "A" || b == "HL") return 3;
      return 4;
    }
    if (a == "SP" && b == "HL") return 1;
    if (a == "SP" && (b == "IX" || b == "IY")) return 2;
    if (a == "IX" || a == "IY") return 4;
    if (a == "HL") return 3;
    if (a == "BC" || a == "DE" || a == "SP") return isMem(b) ? 4 : 3;
    return -1;
  }
  return -1;
}

// can this condition be used with JR
inline bool jrOk(const std::string& cc) { return cc.empty() || cc == "Z" || cc == "NZ" || cc == "C" || cc == "NC"; }

};

/**
 * Peephole pass, runs on the structured list until nothing changes.
 */
inline void peephole(std::vector<Ins>& v) {
  bool changed = true;
  int rounds = 0;
  while (changed && rounds++ < 8) {
    changed = false;
    std::vector<Ins> o;
    o.reserve(v.size());
    bool dead = false;                                   // after an unconditional jump / ret
    for (size_t i = 0; i < v.size(); i++) {
      Ins& c = v[i];

      if (dead) {
        if (c.kind == Ins::LABEL || c.kind == Ins::RAW) dead = false;
        else { changed = true; continue; }
      }

      if (c.kind == Ins::NORMAL) {
        // PUSH r / POP r
        if (c.op == "PUSH" && i + 1 < v.size() && v[i + 1].kind == Ins::NORMAL && v[i + 1].op == "POP" && v[i + 1].a == c.a) { i++; changed = true; continue; }
        // EX DE,HL twice
        if (c.op == "EX" && c.a == "DE" && c.b == "HL" && i + 1 < v.size() && v[i + 1].kind == Ins::NORMAL && v[i + 1].op == "EX" && v[i + 1].a == "DE" && v[i + 1].b == "HL") { i++; changed = true; continue; }
        // LD r,r
        if (c.op == "LD" && c.a == c.b && InsUtil::isReg8(c.a)) { changed = true; continue; }
        // LD L,A / LD A,L
        if (c.op == "LD" && i + 1 < v.size() && v[i + 1].kind == Ins::NORMAL && v[i + 1].op == "LD" &&
            InsUtil::isReg8(c.a) && InsUtil::isReg8(c.b) && v[i + 1].a == c.b && v[i + 1].b == c.a) {
          o.push_back(c); i++; changed = true; continue;
        }
        // LD L,A ; LD H,0 ; LD A,L   (the middle one does not touch A or L)
        if (c.op == "LD" && c.b == "A" && InsUtil::isReg8(c.a) && c.a != "A" && i + 2 < v.size() &&
            v[i + 1].kind == Ins::NORMAL && v[i + 1].op == "LD" && InsUtil::isReg8(v[i + 1].a) && v[i + 1].a != "A" && v[i + 1].a != c.a &&
            !v[i + 1].b.empty() && !InsUtil::isReg8(v[i + 1].b) && v[i + 1].b[0] != '(' && v[i + 1].b[0] != '@' && v[i + 1].b[0] != '_' &&
            v[i + 2].kind == Ins::NORMAL && v[i + 2].op == "LD" && v[i + 2].a == "A" && v[i + 2].b == c.a) {
          o.push_back(c); o.push_back(v[i + 1]); i += 2; changed = true; continue;
        }
        // LD (nn),A ; LD A,(nn)  (same address)
        if (c.op == "LD" && c.b == "A" && InsUtil::isMem(c.a) && !InsUtil::isRegInd(c.a) && i + 1 < v.size() &&
            v[i + 1].kind == Ins::NORMAL && v[i + 1].op == "LD" && v[i + 1].a == "A" && v[i + 1].b == c.a) {
          o.push_back(c); i++; changed = true; continue;
        }
        // LD (IX+d),r ; LD r,(IX+d)
        if (c.op == "LD" && InsUtil::isIdx(c.a) && InsUtil::isReg8(c.b) && i + 1 < v.size() &&
            v[i + 1].kind == Ins::NORMAL && v[i + 1].op == "LD" && v[i + 1].a == c.b && v[i + 1].b == c.a) {
          o.push_back(c); i++; changed = true; continue;
        }
        // INC HL ; DEC HL
        if ((c.op == "INC" || c.op == "DEC") && i + 1 < v.size() && v[i + 1].kind == Ins::NORMAL && v[i + 1].a == c.a &&
            ((c.op == "INC" && v[i + 1].op == "DEC") || (c.op == "DEC" && v[i + 1].op == "INC")) && !InsUtil::isMem(c.a) && c.a.size() == 2) {
          i++; changed = true; continue;
        }
        // LD HL,n ; ADD HL,... : nothing, keep
      }

      if (c.kind == Ins::BRANCH && c.a.empty() && c.op == "JP") {
        // JP L ; L:   (also through a chain of labels)
        size_t j = i + 1;
        bool hit = false;
        while (j < v.size() && v[j].kind == Ins::LABEL) { if (v[j].op == c.b) hit = true; j++; }
        if (hit) { changed = true; continue; }
        dead = true;
      }
      if (c.kind == Ins::BRANCH && !c.a.empty()) {
        // JP cc,L ; L:
        size_t j = i + 1;
        bool hit = false;
        while (j < v.size() && v[j].kind == Ins::LABEL) { if (v[j].op == c.b) hit = true; j++; }
        if (hit) { changed = true; continue; }
      }
      if (c.kind == Ins::NORMAL && (c.op == "RET" && c.a.empty())) dead = true;
      if (c.kind == Ins::NORMAL && c.op == "JP" && c.a == "(HL)") dead = true;

      o.push_back(c);
    }

    // drop labels nobody jumps to (keeps the output readable); function level labels survive
    std::set<std::string> used;
    for (auto& c : o) if (c.kind == Ins::BRANCH) used.insert(c.b);
    for (auto& c : o) if (c.kind == Ins::NORMAL || c.kind == Ins::RAW) {
      // labels referenced as data (LD HL,label) must stay
      for (const std::string* s : {&c.a, &c.b}) {
        size_t p = 0;
        while ((p = s->find("@", p)) != std::string::npos) {
          size_t e = p + 1;
          while (e < s->size() && (IsAlphaDigit((*s)[e]))) e++;
          used.insert(s->substr(p, e - p));
          p = e;
        }
      }
      if (c.kind == Ins::RAW) used.insert("*");
    }
    if (!used.count("*")) {
      std::vector<Ins> o2;
      for (auto& c : o) {
        if (c.kind == Ins::LABEL && c.op.find("__") != std::string::npos && !used.count(c.op) && c.op.compare(c.op.size() - 3, 3, "RET") != 0) { changed = true; continue; }
        o2.push_back(c);
      }
      o.swap(o2);
    }
    v.swap(o);
  }
}

/**
 * Decides for every branch whether a short JR / DJNZ reaches its target, or a JP is needed.
 * Needs exact instruction sizes: any region containing something of unknown size (raw asm) is
 * treated as unreachable for JR.
 */
inline void relax(std::vector<Ins>& v) {
  const size_t n = v.size();
  std::map<std::string, size_t> at;
  for (size_t i = 0; i < n; i++) if (v[i].kind == Ins::LABEL) at[v[i].op] = i;

  std::vector<int> unknownPrefix(n + 1, 0);
  for (size_t i = 0; i < n; i++) {
    bool unk = v[i].kind == Ins::RAW || (v[i].kind == Ins::NORMAL && InsUtil::size(v[i].op, v[i].a, v[i].b) < 0);
    unknownPrefix[i + 1] = unknownPrefix[i] + (unk ? 1 : 0);
  }

  for (size_t i = 0; i < n; i++) {
    if (v[i].kind != Ins::BRANCH) continue;
    bool dj = v[i].op == "DJNZ";
    v[i].isLong = !(dj || (v[i].op == "JP" || v[i].op == "JR")) ? true : !InsUtil::jrOk(v[i].a) && !dj;
    if (!at.count(v[i].b)) v[i].isLong = true;           // target outside of the function
  }

  for (int round = 0; round < 32; round++) {
    std::vector<int> addr(n + 1, 0);
    int pc = 0;
    for (size_t i = 0; i < n; i++) {
      addr[i] = pc;
      switch (v[i].kind) {
        case Ins::LABEL: break;
        case Ins::RAW: break;
        case Ins::NORMAL: { int s = InsUtil::size(v[i].op, v[i].a, v[i].b); pc += s < 0 ? 0 : s; break; }
        case Ins::BRANCH:
          if (v[i].op == "DJNZ") pc += v[i].isLong ? 4 : 2;
          else pc += v[i].isLong ? 3 : 2;
          break;
      }
    }
    addr[n] = pc;

    bool changed = false;
    for (size_t i = 0; i < n; i++) {
      if (v[i].kind != Ins::BRANCH || v[i].isLong) continue;
      size_t t = at[v[i].b];
      size_t lo = std::min(i, t), hi = std::max(i, t);
      bool unk = unknownPrefix[hi + 1] - unknownPrefix[lo] > 0;
      int off = addr[t] - (addr[i] + 2);
      if (unk || off < -126 || off > 129) { v[i].isLong = true; changed = true; }
    }
    if (!changed) break;
  }
}

inline std::string insText(const Ins& c) {
  std::string s;
  switch (c.kind) {
    case Ins::LABEL: return c.op + ":";
    case Ins::RAW: return c.op;
    case Ins::BRANCH:
      if (c.op == "DJNZ") {
        if (c.isLong) s = "  DEC B\n  JP NZ, " + c.b;
        else s = "  DJNZ " + c.b + "-$";
      } else if (c.isLong) {
        s = c.a.empty() ? "  JP " + c.b : "  JP " + c.a + ", " + c.b;
      } else {
        s = c.a.empty() ? "  JR " + c.b + "-$" : "  JR " + c.a + ", " + c.b + "-$";
      }
      break;
    default:
      s = "  " + c.op;
      if (!c.a.empty()) s += " " + c.a;
      if (!c.b.empty()) s += ", " + c.b;
  }
  if (!c.cmt.empty()) {
    size_t pad = s.size() < 26 ? 26 - s.size() : 1;
    // multi line text (DJNZ long): comment goes on the first line only
    s += std::string(pad, ' ') + "; " + c.cmt;
  }
  return s;
}

};
