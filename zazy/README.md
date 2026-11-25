# zazy

A small C compiler for your Z80 computer. It reads a C-like language and writes a `.z80` file in
the dialect of your own assembler (`@LABEL:`, `JR cc, label-$`, `INCLUDE`, ...), plugged into
your `Init.z80`. The output is meant to read like something you wrote by hand: same register
conventions, `RST 0x10` for characters, your `@MUL16x8` / `@DIV8` where they are exact, and every
function keeps its `func name(...) -> type;` header comment.

```
 foo.zz --> Preprocessor --> Lexer/Parser --> Sema (types, folding) --> CodeGen --> foo.z80
           #include #define      AST            width rules, lowering      peephole + JR/JP relaxation
```

## Build and use

```
make                       # builds ./zazy and tools/z80run (a tiny headless Z80 used by the tests)
./zazy -o hello.z80 hello.zz                       # normal mode: INCLUDEs your Init.z80
./zazy --bare -o t.z80 t.zz                        # test mode: own reset code, RST 0x10 -> OUT (0x00),A
```

| option | meaning |
|---|---|
| `--init PATH` | `INCLUDE` path of your `Init.z80` (default `lib/Init.z80`, relative to where the *assembler* runs) |
| `--math PATH` | `INCLUDE` path of `Math.z80`, added automatically when the code calls `@MUL16x8` / `@DIV8` / `@DIV16x8` |
| `--data-base ADDR` | RAM address of the globals (default `0x8000`, SRAM) |
| `--no-comments` | plain asm without the explanatory comments |
| `--no-stdlib` | do not read `lib/zazy.zh` |
| `-I DIR`, `-D NAME=VAL` | include folder, macro |
| `--tokens`, `--ast`, `--repl` | debugging aids |

`lib/zazy.zh` (read automatically) holds the `extern` bindings to your routines: `putc`, `puts`,
`lcd_init`, `lcd_putc`, `bin_bcd`, `hex_display`, `event_push`, `inb`, `outb`, ... Add your own the same way.

### How a program meets `Init.z80`

* `void setup()` becomes `SETUP`, `void main()` becomes `MAIN` (your event loop re-runs it).
* When there are globals, `SETUP` starts with `CALL @ZZ_INIT`: it zeroes the bss and copies the
  initial values (kept in ROM) to SRAM with `LDIR`. If you write no `setup()`, a stub is emitted.
* `Init.z80` itself needs the libraries it calls and a few hooks; put them once in an asm file and
  `#include "app.z80"` it (see `examples/hello_lcd/app.z80`). `#include "x.z80"` is passed through
  to the output as `INCLUDE "x.z80"`, `#include "x.zh"` is a normal zazy header.

## The language

C, reduced to what is useful here.

* Types: `void uint8_t uint16_t int8_t int16_t` (also `char` = `uint8_t`), pointers, arrays
  (multi-dimensional too), `struct` (nested), `enum`, `typedef`, `const`, `static`, `sizeof`.
* Statements: `if else while do for switch case default break continue goto return`, blocks with
  local declarations anywhere, initializers `= {..}` / `= "text"` (also for locals).
* Expressions: all C operators (`?:`, `,`, compound assignment, `++ --`, casts, `&x`, `*p`, `p->f`,
  `a[i]`, pointer arithmetic and pointer difference).
* Preprocessor: `#include #define` (also function-like, `#`, `##`, `__VA_ARGS__`) `#undef #if #ifdef
  #ifndef #elif #else #endif #pragma once #error #warning`.
* Function pointers: `uint8_t (*op)(uint8_t, uint8_t) = add;`, `typedef void (*task_t)(void);`, arrays and
  struct members of them, `op(1, 2)` / `(*op)(1, 2)`, `&add`, `== 0`, globals initialised with function
  names. A call loads the target into `IY` and does `CALL @ZZ_ICALL` (`JP (IY)`), so the normal
  argument registers stay free: up to **3 parameters**, same as a direct call. Functions bound to
  their own registers with `in(...)`/`out(...)` cannot be pointed to.
* `interrupt void f() { ... }` saves every register pair the body can touch (it looks at the generated
  code: a leaf handler pushes just `AF`/`HL`, a bound routine's `clobber(...)` list is honoured, raw
  asm or an unbound call means all) and restores them before `RET`. It is meant for your `RST38`
  dispatcher and event loop, which call handlers with `JP (HL)`/`RET` and only save `AF BC HL`.
  `void f() asm("@RTC_ALARM_EXEC") { ... }` gives a definition a label of your choosing (the entry
  of your vector table); such a function is always emitted.
* `__at(0x7000)` pins a global to an address (memory mapped I/O): `uint8_t port __at(0x7000);`
* Raw asm anywhere: `asm { LD A, ({x}) ; ADD A, ({gv}) ; LD ({x}), A }`. `{name}` expands to the
  operand text of a variable: `IX+3` for a local (so write `({x})`), `_name` for a global; a function
  name gives its label. `naked` functions have no prologue/epilogue.
* Hand written routines are bound with `extern`:

  ```c
  extern uint16_t triple(uint8_t v) asm("@TRIPLE", in(A), out(HL), clobber(AF, DE));
  extern uint8_t  inb(uint8_t port) asm("$in");     // intrinsics: $in $out $ldir, "RST 0x10" emits that instruction
  ```
  `in(...)` lists the registers the arguments travel in, `out(R)` the result register,
  `clobber(...)` what the routine destroys (everything else survives the call).

### Code generation in one page

* 8 bit values live in `A`, 16 bit values in `HL`; the second operand goes to `E` / `DE`.
* Locals and parameters sit at positive offsets from `IX` (`PUSH IX; LD IX,-N; ADD IX,SP; LD SP,IX`
  ... `POP IX; RET`). `IX` is callee-saved, everything else is clobbered.
* Calls: first arguments in `A`/`HL`, then `E`/`DE`, then `C`/`BC` (8/16 bit), result in `A` / `HL`.
* Reused from your libraries (checked exact): `u8 * u8 -> u16` calls `@MUL16x8`; unsigned `/` and `%`
  by a constant call `@DIV8` (8 bit, any divisor) or `@DIV16x8` (16 bit, constant 1..127); powers of
  two become shifts / `AND`. Everything else uses small routines emitted on demand (`@ZZ_MUL8`,
  `@ZZ_MUL16`, `@ZZ_DIVU16`, `@ZZ_DIVS16`), written in your style.
* `JR` is used when the target is within reach, `JP` otherwise (the assembler silently truncates a
  too-long `JR`, so the compiler computes exact instruction sizes itself).
* 16 bit loads through a pointer use your `RST 0x20`.

## Where it differs from ISO C (on purpose)

1. **Width rule.** An operation is done in the *wider operand type*, not in `int`. Constants adopt
   the other operand's type when they fit. `uint8_t + uint8_t` is an 8 bit add (cheap on a Z80);
   it is widened only when the result goes to a wider type, so `uint16_t c = a + b;` keeps the
   carry, while `(a + b) > 300` with two bytes does not. Same width, mixed signedness: 8 bit ->
   `int16_t`, 16 bit -> `uint16_t`. Comparisons, `/`, `%` and `>>` are never widened after the fact.
2. **Comparisons and `!`, `&&`, `||` yield `uint8_t`** (0 or 1), not `int`; so `(a < b) << 8` is 0.
   Cast first when you need more: `(uint16_t)(a < b) << 8`.
3. No `float`/`double`/`long`, no `varargs`, no bit-fields, no `union`, no function returning a function pointer.
4. Functions take **at most 3 register parameters** and cannot take or return a struct by value
   (pass a pointer). Struct assignment and `struct T a = b;` are fine (`LDIR`).
5. Compound assignment whose target has side effects (`a[i++] += 2`) is rejected with an error.
6. `interrupt` returns with `RET` (what your dispatcher expects), not `RETI`/`RETN`; for a raw hardware
   vector write the `EI`/`RETI` yourself in a `naked` function.

## Tests

```
ZAZY_ASM=/path/to/your/assembler make test      # default: the harness used while writing this
```

* `tests/cases/*.zz` - 19 feature programs (functions, pointers, arrays, strings, structs, macros,
  control flow, compound ops, width rules, asm, long jumps, function pointers, interrupt handlers...). Expected output sits in `//> ` lines.
* `tests/fuzz.py` - random expression programs (16 bit, and `--w8` for 8 bit) compared with a Python
  model of the width rules. Several thousand expressions pass.
* They run on `tools/z80run`, a ~300 line documented-flags-only Z80 core. It is a test aid, not a
  replacement for your emulator.

## Things found in your assembler and libraries

These were found while testing; the compiler works around all of them.

* **JR offsets are silently truncated** when the target is out of range (no error). zazy emits `JP` where needed.
* The reader treats the substrings `INCLUDE` / `MACRO` / `ENDM` as directives *even inside labels and
  strings*: zazy renames such labels and splits such strings.
* Duplicate labels silently override each other.
* `LD SP,(nn)` and `ADD IX,HL` are mis-encoded (zazy never emits them); there is no `SLL`.
* `@DIV16x8` returns wrong results for divisors >= 128 (the running remainder is kept in `A`).
  `@DIV8` is exact for all divisors. `@MUL16x8` is exact only while the product fits 16 bits (it adds
  the carry of a partial sum into its multiplier); that is why zazy uses it for `u8 * u8` only.
* `Init.z80` needs `_FS_EXEC` and `@SCAN_CODE_IM` to exist even when no keyboard or file system is
  used; `examples/hello_lcd/app.z80` has stubs.

## Layout

```
src/Lexer src/Preprocessor src/Parser src/Sema src/CodeGen   the compiler (header only, plus Lexer.cpp)
src/Compiler.h src/Main.cpp src/Repl.h                      driver
lib/zazy.zh                                                 bindings of your asm routines
tools/z80run.cpp                                            test Z80 core
tests/ examples/
```
