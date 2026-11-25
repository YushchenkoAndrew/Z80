// Minimal headless Z80 emulator used by the zazy test suite.
//
//   z80run [-m max_steps] [-p port0_file] image.bin
//
// * image is loaded at 0x0000, 0x0000-0x3FFF is ROM (writes are ignored), the rest is RAM
//   (same split as the real computer, see Bus.h)
// * OUT (0x00),A appends the byte to stdout, so a program can "print" through RST 0x10
// * HALT ends the run; the exit code is 0 for HALT, 3 when max_steps ran out
// * `--dump ADDR,LEN` prints LEN bytes from ADDR as hex on stderr after the run
// * documented flags only (the undocumented X/Y bits are not emulated)
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

struct Z80 {
  uint8_t mem[65536];
  uint8_t A = 0xFF, F = 0xFF, B = 0, C = 0, D = 0, E = 0, H = 0, L = 0;
  uint8_t A2 = 0, F2 = 0, B2 = 0, C2 = 0, D2 = 0, E2 = 0, H2 = 0, L2 = 0;
  uint16_t IX = 0, IY = 0, SP = 0xFFFF, PC = 0;
  bool halted = false;
  uint64_t steps = 0;
  std::string out;

  enum { FS = 0x80, FZ = 0x40, FH = 0x10, FP = 0x04, FN = 0x02, FC = 0x01 };

  uint8_t rd(uint16_t a) { return mem[a]; }
  void wr(uint16_t a, uint8_t v) { if (a >= 0x4000) mem[a] = v; }
  uint16_t rd16(uint16_t a) { return rd(a) | (rd(a + 1) << 8); }
  void wr16(uint16_t a, uint16_t v) { wr(a, v & 0xFF); wr(a + 1, v >> 8); }
  uint8_t fetch() { return mem[PC++]; }
  uint16_t fetch16() { uint16_t v = rd16(PC); PC += 2; return v; }
  void push(uint16_t v) { SP -= 2; wr16(SP, v); }
  uint16_t pop() { uint16_t v = rd16(SP); SP += 2; return v; }

  uint16_t BC() { return (B << 8) | C; }
  uint16_t DE() { return (D << 8) | E; }
  uint16_t HL() { return (H << 8) | L; }
  uint16_t AF() { return (A << 8) | F; }
  void setBC(uint16_t v) { B = v >> 8; C = v; }
  void setDE(uint16_t v) { D = v >> 8; E = v; }
  void setHL(uint16_t v) { H = v >> 8; L = v; }
  void setAF(uint16_t v) { A = v >> 8; F = v; }

  static bool parity(uint8_t v) { v ^= v >> 4; v ^= v >> 2; v ^= v >> 1; return !(v & 1); }

  // ---- register access by the 3 bit code, with the index register substituted for HL
  uint16_t* idx = nullptr;            // IX / IY when a DD / FD prefix is active
  int8_t disp = 0;

  uint16_t hlAddr() { return idx ? (uint16_t)(*idx + disp) : HL(); }

  uint8_t getR(int r) {
    switch (r) {
      case 0: return B; case 1: return C; case 2: return D; case 3: return E;
      case 4: return H; case 5: return L; case 6: return rd(hlAddr()); default: return A;
    }
  }
  void setR(int r, uint8_t v) {
    switch (r) {
      case 0: B = v; break; case 1: C = v; break; case 2: D = v; break; case 3: E = v; break;
      case 4: H = v; break; case 5: L = v; break; case 6: wr(hlAddr(), v); break; default: A = v;
    }
  }
  // 16 bit register pairs: 0 BC 1 DE 2 HL (or IX/IY) 3 SP
  uint16_t getRP(int p) {
    switch (p) { case 0: return BC(); case 1: return DE(); case 2: return idx ? *idx : HL(); default: return SP; }
  }
  void setRP(int p, uint16_t v) {
    switch (p) { case 0: setBC(v); break; case 1: setDE(v); break; case 2: if (idx) *idx = v; else setHL(v); break; default: SP = v; }
  }
  uint16_t getRP2(int p) { return p == 3 ? AF() : getRP(p); }
  void setRP2(int p, uint16_t v) { if (p == 3) setAF(v); else setRP(p, v); }

  bool cond(int c) {
    switch (c) {
      case 0: return !(F & FZ); case 1: return F & FZ; case 2: return !(F & FC); case 3: return F & FC;
      case 4: return !(F & FP); case 5: return F & FP; case 6: return !(F & FS); default: return F & FS;
    }
  }

  // ---- ALU
  void alu(int op, uint8_t v) {
    uint16_t r;
    switch (op) {
      case 0: case 1: {   // ADD ADC
        int c = (op == 1 && (F & FC)) ? 1 : 0;
        r = A + v + c;
        F = (r & 0x80) | ((r & 0xFF) ? 0 : FZ) | (((A ^ v ^ r) & 0x10)) | ((~(A ^ v) & (A ^ r) & 0x80) ? FP : 0) | (r > 0xFF ? FC : 0);
        A = r; break;
      }
      case 2: case 3: case 7: {   // SUB SBC CP
        int c = (op == 3 && (F & FC)) ? 1 : 0;
        r = A - v - c;
        F = (r & 0x80) | ((r & 0xFF) ? 0 : FZ) | (((A ^ v ^ r) & 0x10)) | (((A ^ v) & (A ^ r) & 0x80) ? FP : 0) | FN | (r > 0xFF ? FC : 0);
        if (op != 7) A = r;
        break;
      }
      case 4: A &= v; F = (A & 0x80) | (A ? 0 : FZ) | FH | (parity(A) ? FP : 0); break;
      case 5: A ^= v; F = (A & 0x80) | (A ? 0 : FZ) | (parity(A) ? FP : 0); break;
      case 6: A |= v; F = (A & 0x80) | (A ? 0 : FZ) | (parity(A) ? FP : 0); break;
    }
  }

  uint8_t inc8(uint8_t v) { uint8_t r = v + 1; F = (F & FC) | (r & 0x80) | (r ? 0 : FZ) | ((v & 0xF) == 0xF ? FH : 0) | (v == 0x7F ? FP : 0); return r; }
  uint8_t dec8(uint8_t v) { uint8_t r = v - 1; F = (F & FC) | (r & 0x80) | (r ? 0 : FZ) | ((v & 0xF) == 0 ? FH : 0) | (v == 0x80 ? FP : 0) | FN; return r; }

  uint16_t add16(uint16_t a, uint16_t b) {
    uint32_t r = a + b;
    F = (F & (FS | FZ | FP)) | (((a ^ b ^ r) & 0x1000) ? FH : 0) | (r > 0xFFFF ? FC : 0);
    return r;
  }
  uint16_t adc16(uint16_t a, uint16_t b) {
    int c = F & FC;
    uint32_t r = a + b + c;
    F = ((r & 0x8000) ? FS : 0) | ((r & 0xFFFF) ? 0 : FZ) | (((a ^ b ^ r) & 0x1000) ? FH : 0) | ((~(a ^ b) & (a ^ r) & 0x8000) ? FP : 0) | (r > 0xFFFF ? FC : 0);
    return r;
  }
  uint16_t sbc16(uint16_t a, uint16_t b) {
    int c = F & FC;
    uint32_t r = a - b - c;
    F = ((r & 0x8000) ? FS : 0) | ((r & 0xFFFF) ? 0 : FZ) | (((a ^ b ^ r) & 0x1000) ? FH : 0) | (((a ^ b) & (a ^ r) & 0x8000) ? FP : 0) | FN | (r > 0xFFFF ? FC : 0);
    return r;
  }

  uint8_t rot(int op, uint8_t v) {
    uint8_t c, r;
    switch (op) {
      case 0: c = v >> 7; r = (v << 1) | c; break;                     // RLC
      case 1: c = v & 1; r = (v >> 1) | (c << 7); break;               // RRC
      case 2: c = v >> 7; r = (v << 1) | (F & FC); break;              // RL
      case 3: c = v & 1; r = (v >> 1) | ((F & FC) << 7); break;        // RR
      case 4: c = v >> 7; r = v << 1; break;                           // SLA
      case 5: c = v & 1; r = (v >> 1) | (v & 0x80); break;             // SRA
      case 6: c = v >> 7; r = (v << 1) | 1; break;                     // SLL
      default: c = v & 1; r = v >> 1; break;                           // SRL
    }
    F = (r & 0x80) | (r ? 0 : FZ) | (parity(r) ? FP : 0) | c;
    return r;
  }

  void daa() {
    uint8_t a = A, adj = 0, c = F & FC;
    if ((F & FH) || (a & 0xF) > 9) adj |= 6;
    if (c || a > 0x99) { adj |= 0x60; c = 1; }
    if (F & FN) { F = (F & FN) | (((a & 0xF) < 6 && (F & FH)) ? FH : 0); A = a - adj; }
    else { F = ((a & 0xF) > 9) ? FH : 0; A = a + adj; }
    F |= (A & 0x80) | (A ? 0 : FZ) | (parity(A) ? FP : 0) | c | (F & FN);
  }

  void ioOut(uint8_t port, uint8_t v) { if (port == 0x00) out += (char)v; }
  uint8_t inValue = 0xFF;
  uint8_t ioIn(uint8_t) { return inValue; }

  // ---- one instruction
  void step() {
    steps++;
    idx = nullptr;
    uint8_t op = fetch();
    if (op == 0xDD || op == 0xFD) {
      idx = op == 0xDD ? &IX : &IY;
      op = fetch();
      if (op == 0xDD || op == 0xFD) { PC--; return; }
      if (op == 0xCB) { disp = (int8_t)fetch(); uint8_t o2 = fetch(); indexedCB(o2); return; }
      // (HL) operand becomes (IX+d): fetch the displacement when the instruction uses it
      bool usesMem = (op & 0xC7) == 0x46 || (op & 0xF8) == 0x70 || op == 0x36 || op == 0x34 || op == 0x35 ||
                     ((op & 0xC0) == 0x80 && (op & 7) == 6) || ((op & 0xC0) == 0x40 && ((op & 7) == 6 || ((op >> 3) & 7) == 6));
      if (usesMem) disp = (int8_t)fetch();
    }
    exec(op);
  }

  void indexedCB(uint8_t op) {
    uint16_t a = hlAddr();
    int x = op >> 6, y = (op >> 3) & 7, z = op & 7;
    uint8_t v = rd(a);
    if (x == 0) { v = rot(y, v); wr(a, v); if (z != 6) setRegNoIdx(z, v); }
    else if (x == 1) { bitTest(y, v); }
    else { v = x == 2 ? (v & ~(1 << y)) : (v | (1 << y)); wr(a, v); if (z != 6) setRegNoIdx(z, v); }
  }

  void setRegNoIdx(int r, uint8_t v) { uint16_t* s = idx; idx = nullptr; setR(r, v); idx = s; }
  void bitTest(int b, uint8_t v) { F = (F & FC) | FH | ((v & (1 << b)) ? ((b == 7) ? FS : 0) : (FZ | FP)); }

  void exec(uint8_t op) {
    int x = op >> 6, y = (op >> 3) & 7, z = op & 7, p = y >> 1, q = y & 1;
    switch (x) {
      case 0:
        switch (z) {
          case 0:
            if (y == 0) {}
            else if (y == 1) { std::swap(A, A2); std::swap(F, F2); }
            else if (y == 2) { int8_t d = fetch(); if (--B) PC += d; }
            else if (y == 3) { int8_t d = fetch(); PC += d; }
            else { int8_t d = fetch(); if (cond(y - 4)) PC += d; }
            break;
          case 1:
            if (!q) setRP(p, fetch16());
            else { uint16_t r = add16(getRP(2), getRP(p)); setRP(2, r); }
            break;
          case 2:
            switch (y) {
              case 0: wr(BC(), A); break;
              case 1: A = rd(BC()); break;
              case 2: wr(DE(), A); break;
              case 3: A = rd(DE()); break;
              case 4: wr16(fetch16(), getRP(2)); break;
              case 5: setRP(2, rd16(fetch16())); break;
              case 6: wr(fetch16(), A); break;
              default: A = rd(fetch16()); break;
            }
            break;
          case 3: setRP(p, q ? getRP(p) - 1 : getRP(p) + 1); break;
          case 4: setR(y, inc8(getR(y))); break;
          case 5: setR(y, dec8(getR(y))); break;
          case 6: setR(y, fetch()); break;
          case 7:
            switch (y) {
              case 0: { uint8_t c = A >> 7; A = (A << 1) | c; F = (F & (FS | FZ | FP)) | c; break; }
              case 1: { uint8_t c = A & 1; A = (A >> 1) | (c << 7); F = (F & (FS | FZ | FP)) | c; break; }
              case 2: { uint8_t c = A >> 7; A = (A << 1) | (F & FC); F = (F & (FS | FZ | FP)) | c; break; }
              case 3: { uint8_t c = A & 1; A = (A >> 1) | ((F & FC) << 7); F = (F & (FS | FZ | FP)) | c; break; }
              case 4: daa(); break;
              case 5: A = ~A; F |= FH | FN; break;
              case 6: F = (F & (FS | FZ | FP)) | FC; break;
              default: F = (F & (FS | FZ | FP)) | ((F & FC) ? FH : FC); break;
            }
            break;
        }
        break;
      case 1:
        if (op == 0x76) { halted = true; PC--; return; }
        if (idx && (y == 6 || z == 6)) {                  // LD r,(IX+d) / LD (IX+d),r use the plain registers
          if (y == 6) { uint16_t* s = idx; idx = nullptr; uint8_t v = getR(z); idx = s; wr(hlAddr(), v); }
          else { uint8_t v = rd(hlAddr()); setRegNoIdx(y, v); }
        } else setR(y, getR(z));
        break;
      case 2: {
        uint8_t v = (idx && z == 6) ? rd(hlAddr()) : getR(z);
        alu(y, v);
        break;
      }
      case 3:
        switch (z) {
          case 0: if (cond(y)) PC = pop(); break;
          case 1:
            if (!q) setRP2(p, pop());
            else switch (p) {
              case 0: PC = pop(); break;
              case 1: { std::swap(B, B2); std::swap(C, C2); std::swap(D, D2); std::swap(E, E2); std::swap(H, H2); std::swap(L, L2); break; }
              case 2: PC = idx ? *idx : HL(); break;
              default: SP = idx ? *idx : HL(); break;
            }
            break;
          case 2: { uint16_t a = fetch16(); if (cond(y)) PC = a; break; }
          case 3:
            switch (y) {
              case 0: PC = fetch16(); break;
              case 1: { uint8_t o = fetch(); cb(o); break; }
              case 2: { uint8_t n = fetch(); ioOut(n, A); break; }
              case 3: { uint8_t n = fetch(); A = ioIn(n); break; }
              case 4: { uint16_t t = rd16(SP); wr16(SP, idx ? *idx : HL()); if (idx) *idx = t; else setHL(t); break; }
              case 5: { uint16_t t = DE(); setDE(HL()); setHL(t); break; }
              default: break;                          // DI / EI
            }
            break;
          case 4: { uint16_t a = fetch16(); if (cond(y)) { push(PC); PC = a; } break; }
          case 5:
            if (!q) push(getRP2(p));
            else if (p == 0) { uint16_t a = fetch16(); push(PC); PC = a; }
            else if (p == 2) { uint8_t o = fetch(); ed(o); }
            break;
          case 6: alu(y, fetch()); break;
          case 7: push(PC); PC = y * 8; break;
        }
        break;
    }
  }

  void cb(uint8_t op) {
    int x = op >> 6, y = (op >> 3) & 7, z = op & 7;
    uint8_t v = getR(z);
    if (x == 0) setR(z, rot(y, v));
    else if (x == 1) bitTest(y, v);
    else setR(z, x == 2 ? (v & ~(1 << y)) : (v | (1 << y)));
  }

  void block(int y) {
    // y: 4 LDI 5 LDD 6 LDIR 7 LDDR (z=0) ; CPI.. handled by caller
    (void)y;
  }

  void ed(uint8_t op) {
    int x = op >> 6, y = (op >> 3) & 7, z = op & 7, p = y >> 1, q = y & 1;
    idx = nullptr;
    if (x == 1) {
      switch (z) {
        case 0: { uint8_t v = ioIn(C); if (y != 6) setR(y, v); F = (F & FC) | (v & 0x80) | (v ? 0 : FZ) | (parity(v) ? FP : 0); break; }
        case 1: ioOut(C, y == 6 ? 0 : getR(y)); break;
        case 2: setHL(q ? adc16(HL(), getRP(p)) : sbc16(HL(), getRP(p))); break;
        case 3: if (q) setRP(p, rd16(fetch16())); else wr16(fetch16(), getRP(p)); break;
        case 4: { uint8_t v = A; A = 0; alu(2, v); break; }                       // NEG
        case 5: PC = pop(); break;                                                  // RETN / RETI
        case 6: break;                                                              // IM
        case 7:
          switch (y) {
            case 4: { uint8_t m = rd(HL()); uint8_t n = (A & 0xF); wr(HL(), (m >> 4) | (n << 4)); A = (A & 0xF0) | (m & 0xF); F = (F & FC) | (A & 0x80) | (A ? 0 : FZ) | (parity(A) ? FP : 0); break; }   // RRD
            case 5: { uint8_t m = rd(HL()); wr(HL(), (m << 4) | (A & 0xF)); A = (A & 0xF0) | (m >> 4); F = (F & FC) | (A & 0x80) | (A ? 0 : FZ) | (parity(A) ? FP : 0); break; }   // RLD
            default: break;
          }
          break;
      }
      return;
    }
    if (x == 2 && y >= 4 && z < 4) {
      bool dec = y & 1, rep = y >= 6;
      if (z == 0) {                                                                 // LDI LDD LDIR LDDR
        wr(DE(), rd(HL()));
        setHL(HL() + (dec ? -1 : 1)); setDE(DE() + (dec ? -1 : 1)); setBC(BC() - 1);
        F = (F & (FS | FZ | FC)) | (BC() ? FP : 0);
        if (rep && BC()) PC -= 2;
      } else if (z == 1) {                                                          // CPI CPD CPIR CPDR
        uint8_t m = rd(HL()); uint8_t r = A - m;
        setHL(HL() + (dec ? -1 : 1)); setBC(BC() - 1);
        F = (F & FC) | (r & 0x80) | (r ? 0 : FZ) | (((A ^ m ^ r) & 0x10)) | (BC() ? FP : 0) | FN;
        if (rep && BC() && r) PC -= 2;
      } else if (z == 2) {                                                          // INI IND INIR INDR
        wr(HL(), ioIn(C)); setHL(HL() + (dec ? -1 : 1)); B--; F = (F & FC) | FN | (B ? 0 : FZ);
        if (rep && B) PC -= 2;
      } else {                                                                      // OUTI OUTD OTIR OTDR
        B--; ioOut(C, rd(HL())); setHL(HL() + (dec ? -1 : 1)); F = (F & FC) | FN | (B ? 0 : FZ);
        if (rep && B) PC -= 2;
      }
    }
  }
};

static Z80 cpu;

int main(int argc, char** argv) {
  uint64_t maxSteps = 5000000;
  std::string image;
  std::vector<std::pair<int, int>> dumps;
  bool trace = false;
  for (int i = 1; i < argc; i++) {
    std::string a = argv[i];
    if (a == "-m" && i + 1 < argc) maxSteps = strtoull(argv[++i], nullptr, 0);
    else if (a == "--trace") trace = true;
    else if (a == "--in" && i + 1 < argc) cpu.inValue = (uint8_t)strtol(argv[++i], nullptr, 0);
    else if (a == "--dump" && i + 1 < argc) { int ad, ln; sscanf(argv[++i], "%i,%i", &ad, &ln); dumps.push_back({ad, ln}); }
    else image = a;
  }
  if (image.empty()) { fprintf(stderr, "usage: z80run [-m steps] [--dump addr,len] image.bin\n"); return 2; }
  memset(cpu.mem, 0, sizeof cpu.mem);
  FILE* f = fopen(image.c_str(), "rb");
  if (!f) { fprintf(stderr, "cannot open %s\n", image.c_str()); return 2; }
  size_t n = fread(cpu.mem, 1, 0x10000, f);
  fclose(f);
  (void)n;
  while (!cpu.halted && cpu.steps < maxSteps) {
    if (trace) fprintf(stderr, "%04X op=%02X AF=%04X BC=%04X DE=%04X HL=%04X SP=%04X IX=%04X\n", cpu.PC, cpu.mem[cpu.PC], cpu.AF(), cpu.BC(), cpu.DE(), cpu.HL(), cpu.SP, cpu.IX);
    cpu.step();
  }
  fwrite(cpu.out.data(), 1, cpu.out.size(), stdout);
  for (auto& d : dumps) {
    fprintf(stderr, "@%04X:", d.first);
    for (int i = 0; i < d.second; i++) fprintf(stderr, " %02X", cpu.mem[(d.first + i) & 0xFFFF]);
    fprintf(stderr, "\n");
  }
  fprintf(stderr, "%s steps=%llu PC=%04X SP=%04X\n", cpu.halted ? "HALT" : "TIMEOUT", (unsigned long long)cpu.steps, cpu.PC, cpu.SP);
  return cpu.halted ? 0 : 3;
}
