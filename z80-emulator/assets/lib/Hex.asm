#include "Defs.asm"
;;
;; Example:
;;  LD HL, 0x5000
;;  LD (HL), A
;;
;;  PUSH HL     ;; Send ptr to func
;;  CALL _HEX   ;; Display value in hex display
;;
;;
;; func HEX(byte*) -> void;
;;   byte -- ptr to byte to display 
;;
_HEX:
  POP HL     ;; Get return addr
  EX (SP), HL;; Restore return addr & get ptr to value
  LD A, (HL) ;; Get value to display

  LD B, 0    ;; Reset reg B
  LD D, 0x80 ;; Set count & mask at the same time
  PUSH HL    ;; Save in stack ptr to number

_HEX_lp:
  XOR A      ;; Reset reg A
  RLD        ;; Get high bits from ptr (HL)
  LD C, A    ;; Load high bit
  LD HL, _HEX_DB ;; Load start addr of db
  ADD HL, BC ;; Addr + offset
  LD A, (HL) ;; Load appropriet value
  OR D       ;; Apply mask to the value
  OUT (HEX_PORT), A ;; Display segment
  POP HL     ;; Restore ptr to number / On last cycle have ret ptr
  SLA D      ;; Dec a loop counter
  JP C, _HEX_lp
  JP (HL)    ;; Aka return

_HEX_DB:     ;; This array of values need for display HEX number
  db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
