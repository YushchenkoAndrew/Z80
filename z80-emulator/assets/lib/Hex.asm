#include "Defs.asm"

;;
;; Example:
;;  LD A, 0x24
;;  CALL #HEX   ;; Display value in hex display
;;
;; func HEX() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#HEX:
  PUSH HL    ;; Save reg HL in stack
  PUSH BC    ;; Save reg BC in stack
  PUSH AF    ;; Save reg AF in stack, for not modifing the original data

  LD BC, 0x80 ;; Reset reg B & Set count & mask at the same time reg C
  LD HL, PTR_HEX_BYTE ;; Load ptr to the memory where displayed byte is saved
  LD (HL), A ;; Save in memory what is displayed on hex display
  INC HL     ;; Move ptr to the modified byte, it will be used for hex calculation
  LD (HL), A ;; Save in memory what is displayed on hex display, will be use for hex calculation

#HEX_lp:
  PUSH HL    ;; Save in stack ptr to number
  XOR A      ;; Reset reg A
  RLD        ;; Get high bits from ptr (HL)
  LD B, A    ;; Load high bit
  LD HL, .HEX_DB ;; Load start addr of db
  INC B      ;; Start cnt from 1
#HEX_lp_lp:
  INC HL     ;; Addr + offset
  DJNZ #HEX_lp_lp-$
  LD A, (HL) ;; Load appropriet value
  OR C       ;; Apply mask to the value
  OUT (HEX_PORT), A ;; Display segment
  POP HL     ;; Restore ptr to number
  SLA C      ;; Dec a loop counter
  JP C, #HEX_lp

  POP AF     ; Restore reg AF from stack, aka the original value
  POP BC     ; Restore reg BC from stack
  POP HL     ; Restore reg HL from stack
  RET

.HEX_DB:     ;; This array of values need for display HEX number
  db 0x00, 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
