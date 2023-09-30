#include "Defs.asm"

;;
;; Example:
;;  LD HL, number ;; Number ptr
;;  LD C, 0x20    ;; Load pot
;;  CALL #HEX_ASCII
;;
;; number: db 0x25
;;
;; proc HEX_2_ASCII(...regs) -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg C  -- output port
;;   reg DE -- unaffected
;;   reg HL -- ptr to number
#HEX_ASCII:
  LD B, 0x02 ;; Aka display high & low bits
#HEX_ASCII_lp:
  XOR A      ;; Reset reg A
  RLD        ;; Load hight bits from (HL) -> A
  ADD A, 0x90;; Acc = 90h - 9Fh
  DAA        ;; Acc = 90h - 99h OR 00h - 15h
  ADC A, 0x40;; Add char 'A'
  DAA        ;; Acc = 30h - 39h (aka ascii 0 - 9) OR 40h - 45h (aka ascii 'A' - 'F')
  OUT (C), A ;; Display char
  DJNZ #HEX_ASCII_lp-$
  RET


;;
;; Example:
;;  LD A, 0x0F  ;; Load number
;;  CALL #BIN_BCD
;;
;; proc BIN_BCD(...regs) -> reg A;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#BIN_BCD:
  PUSH BC    ;; Save reg BC value in stack
  LD B, 0x08 ;; Run loop 8 times
  LD C, A    ;; Save reg A value in reg C
  XOR A      ;; Reset reg A
#BIN_BCD_lp:
  SLA C      ;; Get C7 bit as carry flag
  ADC A, A   ;; Each time multiply by 2
  DAA        ;; Adjust Acc to dec
  DJNZ #BIN_BCD_lp-$
  POP BC     ;; Restore reg BC
  RET