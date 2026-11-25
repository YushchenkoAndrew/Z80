#pragma once
#include "src/Utils/Utils.h"

namespace Zazy {

/**
 * Small asm routines the generated code calls when an operation has no inline form. They are
 * written in the same style as lib/utils/Math.z80 and only emitted when something uses them.
 *
 * Why not just call @MUL16x8 / @DIV8 / @DIV16x8 from the existing Math.z80? Those are used
 * wherever they are correct (see CodeGen::genMul / genDivMod), but:
 *   - @MUL16x8 adds the carry of every partial sum into its multiplier register, so a product
 *     that does not fit 16 bits corrupts the remaining multiplier bits (C `*` must wrap),
 *   - @DIV16x8 keeps the running remainder in an 8 bit register, which overflows when the divisor
 *     is 128 or more (@DIV8 is exact for the whole 8 bit range).
 */
struct Runtime {
  struct Routine { std::string text; std::vector<std::string> needs; };

  static const std::map<std::string, Routine>& table() {
    static const std::map<std::string, Routine> t = {
      { "@ZZ_MUL8", { R"(;;
;; proc ZZ_MUL8(...regs) -> reg A;
;;   reg A  -- as defined, A * E (low 8 bits)
;;   reg BC -- B destroyed
;;   reg DE -- destroyed
;;   reg HL -- unaffected
@ZZ_MUL8:
  LD D, A    ; Save multiplicand in reg D
  XOR A      ; Reset product
  LD B, 8    ; Load amount of bites to shift
@ZZ_MUL8_lp:
  SRL E      ; Move lowest bit of multiplier to carry
  JR NC, @ZZ_MUL8_lp_end-$; Skip if bit is not set
  ADD A, D   ; Add multiplicand to product
@ZZ_MUL8_lp_end:
  SLA D      ; Multiplicand << 1
  DJNZ @ZZ_MUL8_lp-$
  RET
)", {} } },

      { "@ZZ_MUL16", { R"(;;
;; proc ZZ_MUL16(...regs) -> reg HL;
;;   reg A  -- destroyed
;;   reg BC -- destroyed
;;   reg DE -- destroyed
;;   reg HL -- as defined, HL * DE (low 16 bits)
@ZZ_MUL16:
  LD B, D
  LD C, E    ; Move multiplier to reg BC
  EX DE, HL  ; Move multiplicand to reg DE
  LD HL, 0   ; Reset product
@ZZ_MUL16_lp:
  SRL B      ; Move lowest bit of multiplier to carry
  RR C
  JR NC, @ZZ_MUL16_lp_end-$; Skip if bit is not set
  ADD HL, DE ; Add multiplicand to product
@ZZ_MUL16_lp_end:
  SLA E      ; Multiplicand << 1
  RL D
  LD A, B
  OR C       ; Check if there are bits left in multiplier
  JR NZ, @ZZ_MUL16_lp-$
  RET
)", {} } },

      { "@ZZ_DIVU16", { R"(;;
;; proc ZZ_DIVU16(...regs) -> reg HL & reg DE;
;;   reg A  -- destroyed
;;   reg BC -- destroyed
;;   reg DE -- as defined, remainder of HL / DE
;;   reg HL -- as defined, quotient of HL / DE
@ZZ_DIVU16:
  LD B, H
  LD C, L    ; Move dividend to reg BC, it will turn into the quotient
  LD HL, 0   ; Reset remainder
  LD A, 16   ; Load amount of bites to shift
@ZZ_DIVU16_lp:
  SLA C      ; Shift next bit of dividend
  RL B
  ADC HL, HL ; into the remainder
  JR C, @ZZ_DIVU16_sb-$; Remainder overflowed, it is for sure bigger than divisor
  SBC HL, DE ; Carry is clear here
  JR NC, @ZZ_DIVU16_st-$; Divisor fits, set bit of quotient
  ADD HL, DE ; Restore remainder
  JR @ZZ_DIVU16_nx-$
@ZZ_DIVU16_sb:
  OR A
  SBC HL, DE
@ZZ_DIVU16_st:
  INC C      ; Set bit of quotient
@ZZ_DIVU16_nx:
  DEC A
  JR NZ, @ZZ_DIVU16_lp-$
  EX DE, HL  ; Move remainder to reg DE
  LD H, B
  LD L, C    ; Move quotient to reg HL
  RET
)", {} } },

      { "@ZZ_DIVS16", { R"(;;
;; proc ZZ_DIVS16(...regs) -> reg HL & reg DE;
;;   reg A  -- destroyed
;;   reg BC -- destroyed
;;   reg DE -- as defined, remainder of HL / DE (sign of the dividend)
;;   reg HL -- as defined, quotient of HL / DE (truncated to zero)
@ZZ_DIVS16:
  LD A, H
  XOR D
  PUSH AF    ; Save sign of the quotient
  LD A, H
  PUSH AF    ; Save sign of the remainder
  BIT 7, H
  JR Z, @ZZ_DIVS16_a-$
  XOR A      ; Dividend = -Dividend
  SUB L
  LD L, A
  SBC A, A
  SUB H
  LD H, A
@ZZ_DIVS16_a:
  BIT 7, D
  JR Z, @ZZ_DIVS16_b-$
  XOR A      ; Divisor = -Divisor
  SUB E
  LD E, A
  SBC A, A
  SUB D
  LD D, A
@ZZ_DIVS16_b:
  CALL @ZZ_DIVU16
  POP AF     ; Restore sign of the remainder
  ADD A, A
  JR NC, @ZZ_DIVS16_c-$
  XOR A      ; Remainder = -Remainder
  SUB E
  LD E, A
  SBC A, A
  SUB D
  LD D, A
@ZZ_DIVS16_c:
  POP AF     ; Restore sign of the quotient
  ADD A, A
  RET NC
  XOR A      ; Quotient = -Quotient
  SUB L
  LD L, A
  SBC A, A
  SUB H
  LD H, A
  RET
)", {"@ZZ_DIVU16"} } },

      { "@ZZ_ICALL", { R"(;;
;; proc ZZ_ICALL() -> void;
;;   reg IY -- address of the function to call (call through a function pointer)
;;   the other registers carry the arguments, as for a normal call
@ZZ_ICALL:
  JP (IY)
)", {} } },
      { "@ZZ_MEMSET", { R"(;;
;; proc ZZ_MEMSET(...regs) -> void;
;;   reg A  -- value to fill with
;;   reg BC -- amount of bytes, at least 1
;;   reg HL -- ptr to the memory
@ZZ_MEMSET:
  LD (HL), A ; Set the first byte
  DEC BC
  LD A, B
  OR C       ; Check if that was the only one
  RET Z
  LD D, H
  LD E, L
  INC DE
  LDIR       ; Spread it over the rest
  RET
)", {} } },
    };
    return t;
  }
};

};
