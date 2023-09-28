#include "Defs.asm"

;; func HEX(arg) -> void; High byte will be displayed on HEX display
_HEX:
  POP DE     ;; Get return addr
  POP AF     ;; Get value to display
  PUSH DE    ;; Restore return addr

  LD D, 0x80 ;; Set count & mask at the same time
  LD E, A    ;; Save value in reg C
  LD B, 0    ;; Reset reg B

  RRCA       ;; Move A0 -> A7
  RRCA       ;; Move A0 -> A7
  RRCA       ;; Move A0 -> A7
  RRCA       ;; Move A0 -> A7

_HEX_lp:
  AND 0x0F   ;; Now just get low portion
  LD C, A    ;; Load high bit
  LD HL, _HEX_DB ;; Load start addr of db
  ADD HL, BC ;; Addr + offset

  LD A, (HL) ;; Load appropriet value
  OR D       ;; Apply mask to the value
  OUT (HEX_PORT), A ;; Display segment

  LD A, E    ;; Load saved reg C value
  SLA D      ;; Dec a loop counter
  JR C, _HEX_lp-$
  RET

_HEX_DB:     ;; This array of values need for display HEX number
  db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
