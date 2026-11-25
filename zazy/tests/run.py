#!/usr/bin/env python3
"""zazy test runner.

  python3 tests/run.py [-v] [-k substring] [--asm PATH_TO_ASSEMBLER]

Every tests/cases/*.zz is compiled with --bare, assembled, executed on tools/z80run and the
captured output is compared with the `//> text` lines inside the source.

The assembler is your own one (the interpreter project); point to it with --asm or $ZAZY_ASM.
It is called as   ASM in.z80 out.bin [out.sym]   with the current directory containing lib/.
"""
import os, subprocess, sys, tempfile, shutil, glob

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ZAZY = os.path.join(ROOT, "zazy")
RUN = os.path.join(ROOT, "tools", "z80run")

def expected(path):
    out = []
    for line in open(path, encoding="utf-8"):
        if line.startswith("//>"):
            out.append(line[3:].lstrip(" ").rstrip("\n") if line[3:4] == " " else line[3:].rstrip("\n"))
    return "".join(l + "\n" for l in out)

def build_and_run(src, work, asm, steps=5_000_000, extra=()):
    base = os.path.splitext(os.path.basename(src))[0]
    z80 = os.path.join(work, base + ".z80")
    binf = os.path.join(work, base + ".bin")
    r = subprocess.run([ZAZY, "--bare", "-I", os.path.dirname(src), *extra, "-o", z80, src], capture_output=True, text=True)
    if r.returncode:
        return "COMPILE", r.stdout + r.stderr
    r = subprocess.run([asm, base + ".z80", base + ".bin", base + ".sym"], cwd=work, capture_output=True, text=True)
    if r.returncode or not os.path.exists(binf):
        return "ASM", r.stdout + r.stderr
    r = subprocess.run([RUN, "-m", str(steps), binf], capture_output=True)
    return ("OK" if r.returncode == 0 else "RUN%d" % r.returncode), r.stdout.decode("latin1"), r.stderr.decode()

def main():
    args = sys.argv[1:]
    verbose = "-v" in args
    key = args[args.index("-k") + 1] if "-k" in args else ""
    asm = args[args.index("--asm") + 1] if "--asm" in args else os.environ.get("ZAZY_ASM", "/home/claude/work/harness/zasm")
    if not (os.path.exists(ZAZY) and os.path.exists(RUN)):
        sys.exit("build first: make && make tools")
    work = tempfile.mkdtemp(prefix="zazy-test-")
    shutil.copytree(os.path.join(ROOT, "tests", "fixtures"), work, dirs_exist_ok=True)   # lib/ (your Math.z80) and asm/ (helpers)
    ok = bad = 0
    for src in sorted(glob.glob(os.path.join(ROOT, "tests", "cases", "*.zz"))):
        name = os.path.basename(src)
        if key and key not in name:
            continue
        res = build_and_run(src, work, asm)
        want = expected(src)
        if res[0] == "OK" and res[1] == want:
            ok += 1
            if verbose: print("ok    ", name)
        else:
            bad += 1
            print("FAIL  ", name, res[0])
            if res[0] in ("COMPILE", "ASM"):
                print(res[1])
            else:
                import difflib
                for l in difflib.unified_diff(want.splitlines(), res[1].splitlines(), "expected", "got", lineterm="", n=1):
                    print("      " + l)
                if res[0] != "OK": print("     ", res[2].strip())
    print("%d passed, %d failed" % (ok, bad))
    shutil.rmtree(work, ignore_errors=True)
    sys.exit(1 if bad else 0)

main()
