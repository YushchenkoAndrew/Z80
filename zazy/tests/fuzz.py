#!/usr/bin/env python3
"""Differential fuzzer for the arithmetic of zazy.

  python3 tests/fuzz.py [-n programs] [-s seed] [--asm PATH]

  --w8 switches to 8 bit variables (every intermediate result is cast back to 8 bit).

Random expression programs over uint16_t / int16_t variables (8 bit variables only enter
through casts) are compiled, executed on tools/z80run and compared with a Python model of the
documented width rule (16 bit operands, mixed signedness -> unsigned, result truncated to 16 bit).
"""
import os, random, subprocess, sys, tempfile, shutil

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ZAZY, RUN = os.path.join(ROOT, "zazy"), os.path.join(ROOT, "tools", "z80run")

def wrap(v, signed):
    v &= 0xFFFF
    return v - 0x10000 if signed and v & 0x8000 else v

class N:  # node: text + python value (v, signed)
    def __init__(s, text, v, signed): s.text, s.v, s.signed = text, v, signed

def boolean(text, v, rnd):
    # comparisons / logical operators yield uint8_t in zazy (not int): widen them explicitly
    t = rnd.choice(["uint16_t", "int16_t"])
    return N("((%s)%s)" % (t, text), v, t == "int16_t")

def gen(rnd, env, depth):
    if depth == 0 or rnd.random() < 0.2:
        k = rnd.random()
        if k < 0.7:
            name = rnd.choice(list(env))
            v, sg, ty = env[name]
            if ty in ("uint8_t", "int8_t"):
                tgt = rnd.choice(["uint16_t", "int16_t"])
                return N("(%s)%s" % (tgt, name), wrap(v, tgt == "int16_t"), tgt == "int16_t")
            return N(name, v, sg)
        tgt = rnd.choice(["uint16_t", "int16_t"])
        c = rnd.choice([0, 1, 2, 3, 7, 10, 100, 255, 256, 1000, 32767, rnd.randrange(0, 32768)])
        if tgt == "int16_t" and rnd.random() < .3: c = -c
        return N("((%s)%d)" % (tgt, c), wrap(c, tgt == "int16_t"), tgt == "int16_t")
    r = rnd.random()
    if r < 0.12:
        a = gen(rnd, env, depth - 1); op = rnd.choice(["-", "~", "!"])
        if op == "-": return N("(-%s)" % a.text, wrap(-a.v, a.signed), a.signed)
        if op == "~": return N("(~%s)" % a.text, wrap(~a.v, a.signed), a.signed)
        return boolean("(!%s)" % a.text, int(a.v == 0), rnd)
    a = gen(rnd, env, depth - 1)
    op = rnd.choice(["+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>", "==", "!=", "<", ">", "<=", ">=", "&&", "||"])
    if op in ("<<", ">>"):
        k = rnd.randrange(0, 16)
        v = wrap(a.v << k, a.signed) if op == "<<" else wrap(a.v >> k, a.signed)   # python >> is arithmetic on negatives
        return N("(%s %s %d)" % (a.text, op, k), v, a.signed)
    if op in ("/", "%"):
        d = gen(rnd, env, depth - 1)
        dv = (d.v & 0x7FFF) | 1                                   # positive, never zero, same type as d
        dt = "(%s & 0x7FFF | 1)" % d.text
        sg = a.signed and d.signed
        # division in the common type: if unsigned, both operands as unsigned 16 bit
        x = wrap(a.v, sg); y = wrap(dv, sg)
        if sg:
            q = abs(x) // abs(y) * (1 if (x < 0) == (y < 0) else -1)
            v = wrap(q if op == "/" else x - q * y, True)
        else:
            x &= 0xFFFF; y &= 0xFFFF
            v = (x // y) if op == "/" else (x % y)
        return N("(%s %s %s)" % (a.text, op, dt), v, sg)
    b = gen(rnd, env, depth - 1)
    sg = a.signed and b.signed
    x, y = wrap(a.v, sg), wrap(b.v, sg)
    if op in ("+", "-", "*", "&", "|", "^"):
        v = {"+": x + y, "-": x - y, "*": x * y, "&": x & y, "|": x | y, "^": x ^ y}[op]
        return N("(%s %s %s)" % (a.text, op, b.text), wrap(v, sg), sg)
    if op == "&&": return boolean("(%s && %s)" % (a.text, b.text), int(a.v != 0 and b.v != 0), rnd)
    if op == "||": return boolean("(%s || %s)" % (a.text, b.text), int(a.v != 0 or b.v != 0), rnd)
    v = {"==": x == y, "!=": x != y, "<": x < y, ">": x > y, "<=": x <= y, ">=": x >= y}[op]
    return boolean("(%s %s %s)" % (a.text, op, b.text), int(v), rnd)

# ---------------------------------------------------------------------------- 8 bit mode
def w8(v, signed):
    v &= 0xFF
    return v - 256 if signed and v & 0x80 else v

def cast8(text, v, rnd):
    t = rnd.choice(["uint8_t", "int8_t"])
    return N("((%s)%s)" % (t, text), w8(v, t == "int8_t"), t == "int8_t")

def gen8(rnd, env, depth):
    if depth == 0 or rnd.random() < 0.2:
        if rnd.random() < 0.75:
            name = rnd.choice(list(env)); v, sg, ty = env[name]
            return N(name, v, sg)
        t = rnd.choice(["uint8_t", "int8_t"]); c = rnd.choice([0, 1, 2, 7, 100, 127, rnd.randrange(128)])
        if t == "int8_t" and rnd.random() < .3: c = -c
        return N("((%s)%d)" % (t, c), w8(c, t == "int8_t"), t == "int8_t")
    if rnd.random() < 0.12:
        a = gen8(rnd, env, depth - 1); op = rnd.choice(["-", "~", "!"])
        if op == "-": return cast8("(-%s)" % a.text, -a.v, rnd)
        if op == "~": return cast8("(~%s)" % a.text, ~a.v, rnd)
        return cast8("(!%s)" % a.text, int(a.v == 0), rnd)
    a = gen8(rnd, env, depth - 1)
    op = rnd.choice(["+", "-", "*", "/", "%", "&", "|", "^", "<<", ">>", "==", "!=", "<", ">", "<=", ">=", "&&", "||"])
    if op in ("<<", ">>"):
        k = rnd.randrange(0, 8)
        return cast8("(%s %s %d)" % (a.text, op, k), (a.v << k) if op == "<<" else (a.v >> k), rnd)
    b = gen8(rnd, env, depth - 1)
    if op in ("/", "%"):
        dt = "(%s & 0x7F | 1)" % b.text
        dv = (b.v & 0x7F) | 1
        if a.signed or b.signed:       # int16 (mixed) or signed division
            q = abs(a.v) // abs(dv) * (1 if (a.v < 0) == (dv < 0) else -1)
            v = q if op == "/" else a.v - q * dv
        else:
            v = (a.v // dv) if op == "/" else (a.v % dv)
        return cast8("(%s %s %s)" % (a.text, op, dt), v, rnd)
    if op == "&&": return cast8("(%s && %s)" % (a.text, b.text), int(a.v != 0 and b.v != 0), rnd)
    if op == "||": return cast8("(%s || %s)" % (a.text, b.text), int(a.v != 0 or b.v != 0), rnd)
    if op in ("+", "-", "*", "&", "|", "^"):
        v = {"+": a.v + b.v, "-": a.v - b.v, "*": a.v * b.v, "&": a.v & b.v, "|": a.v | b.v, "^": a.v ^ b.v}[op]
        return cast8("(%s %s %s)" % (a.text, op, b.text), v, rnd)
    v = {"==": a.v == b.v, "!=": a.v != b.v, "<": a.v < b.v, ">": a.v > b.v, "<=": a.v <= b.v, ">=": a.v >= b.v}[op]
    return cast8("(%s %s %s)" % (a.text, op, b.text), int(v), rnd)

def program8(seed, nexpr=25):
    rnd = random.Random(seed)
    env = {}
    for i, ty in enumerate(["uint8_t", "uint8_t", "uint8_t", "int8_t", "int8_t", "int8_t"]):
        v = rnd.choice([0, 1, 127, 128, 255, rnd.randrange(256)]) if ty == "uint8_t" else rnd.choice([0, 1, -1, 127, -128, rnd.randrange(-128, 128)])
        env["v%d" % i] = (v, ty.startswith("int"), ty)
    src = ["void ph(uint8_t v) { for (uint8_t i = 0; i < 2; i++) { uint8_t n = v >> 4; putc(n < 10 ? '0' + n : 'A' + n - 10); v <<= 4; } putc('\\n'); }"]
    for n, (v, sg, ty) in env.items(): src.append("%s %s;" % (ty, n))
    src.append("void main() {")
    for n, (v, sg, ty) in env.items(): src.append("  %s = %d;" % (n, v))
    want = []
    for _ in range(nexpr):
        e = gen8(rnd, env, rnd.choice([2, 3, 3, 4]))
        src.append("  ph((uint8_t)%s);" % e.text)
        want.append("%02X" % (e.v & 0xFF))
    src.append("}")
    return "\n".join(src) + "\n", want

def program(seed, nexpr=25):
    rnd = random.Random(seed)
    env = {}
    names = []
    for i, ty in enumerate(["uint16_t", "uint16_t", "int16_t", "int16_t", "uint8_t", "int8_t"]):
        if ty == "uint8_t": v = rnd.choice([0, 1, 127, 128, 255, rnd.randrange(256)])
        elif ty == "int8_t": v = rnd.choice([0, 1, -1, 127, -128, rnd.randrange(-128, 128)])
        elif ty == "uint16_t": v = rnd.choice([0, 1, 255, 256, 32768, 65535, rnd.randrange(65536)])
        else: v = rnd.choice([0, 1, -1, 32767, -32768, 1000, rnd.randrange(-32768, 32768)])
        n = "v%d" % i
        env[n] = (v, ty.startswith("int"), ty)
    src = ["void ph(uint16_t v) { for (uint8_t i = 0; i < 4; i++) { uint8_t n = v >> 12; putc(n < 10 ? '0' + n : 'A' + n - 10); v <<= 4; } putc('\\n'); }"]
    for n, (v, sg, ty) in env.items():
        src.append("%s %s;" % (ty, n))
    src.append("void main() {")
    for n, (v, sg, ty) in env.items():
        src.append("  %s = %d;" % (n, v))
    want = []
    for _ in range(nexpr):
        e = gen(rnd, env, rnd.choice([2, 3, 3, 4]))
        src.append("  ph((uint16_t)%s);" % e.text)
        want.append("%04X" % (e.v & 0xFFFF))
    src.append("}")
    return "\n".join(src) + "\n", want

def main():
    a = sys.argv[1:]
    n = int(a[a.index("-n") + 1]) if "-n" in a else 100
    seed0 = int(a[a.index("-s") + 1]) if "-s" in a else 1
    asm = a[a.index("--asm") + 1] if "--asm" in a else os.environ.get("ZAZY_ASM", "/home/claude/work/harness/zasm")
    work = tempfile.mkdtemp(prefix="zazy-fuzz-")
    shutil.copytree(os.path.join(ROOT, "tests", "fixtures", "lib"), os.path.join(work, "lib"))
    bad = 0
    for seed in range(seed0, seed0 + n):
        src, want = (program8 if "--w8" in a else program)(seed)
        zz = os.path.join(work, "f.zz")
        open(zz, "w").write(src)
        r = subprocess.run([ZAZY, "--bare", "-o", os.path.join(work, "f.z80"), zz], capture_output=True, text=True)
        if r.returncode:
            print("seed", seed, "COMPILE FAIL\n", r.stdout, r.stderr); bad += 1; continue
        r = subprocess.run([asm, "f.z80", "f.bin", "f.sym"], cwd=work, capture_output=True, text=True)
        if r.returncode:
            print("seed", seed, "ASM FAIL", r.stdout, r.stderr); bad += 1; continue
        r = subprocess.run([RUN, "-m", "20000000", os.path.join(work, "f.bin")], capture_output=True)
        got = r.stdout.decode("latin1").split()
        if got != want or r.returncode:
            bad += 1
            keep = "/tmp/zazy-fuzz-fail-%d.zz" % seed
            shutil.copy(zz, keep)
            diff = [(i, w, g) for i, (w, g) in enumerate(zip(want, got + [None] * len(want))) if w != g]
            print("seed %d MISMATCH (%d of %d) first: expr #%d want %s got %s  [%s] -> %s" % (seed, len(diff), len(want), diff[0][0] if diff else -1, diff[0][1] if diff else "", diff[0][2] if diff else "", "timeout" if r.returncode else "", keep))
    print("fuzz: %d programs, %d failed" % (n, bad))
    shutil.rmtree(work, ignore_errors=True)
    sys.exit(1 if bad else 0)

main()
