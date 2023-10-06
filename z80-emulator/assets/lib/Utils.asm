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
;;  LD HL, number ;; Number ptr
;;  LD C, 0x20    ;; Load pot
;;  CALL #HEX_ASCII
;;
;; number: db 0x25
;;
;; proc ASCII_HEX(...regs) -> reg A;
;;   reg A  -- as defined
;;   reg B  -- unaffected
;;   reg C  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#ASCII_HEX:
  CP "0"     ; Check the range of the received char
  RET C      ; If it less then "0" = 48 then return
  CP ":"     ; After a "9" goest char ":"
  JR C, #ASCII_HEX_0-$
  CP "A"     ; Check the range of the received char
  RET C      ; If it less then "A" = 65 then return
  CP "G"     ; After a "f" goest char "G"
  JR C, #ASCII_HEX_A-$
  CP "a"     ; Check the range of the received char
  RET C      ; If it less then "A" = 97 then return
  CP "g"     ; After a "f" goest char "G"
  JR C, #ASCII_HEX_a-$
  RET

#ASCII_HEX_0:
  SUB "0"    ; Convert '0'-'9' to 0h-9h
  RET

#ASCII_HEX_A:
  SUB "7"    ; Convert 'A'-'F' to Ah-Fh, example sub 0x41 - 0x37 = A
  RET

#ASCII_HEX_a:
  SUB "W"    ; Convert 'a'-'f' to Ah-Fh, example sub 0x61 - 0x57 = A
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


;;
;; Example:
;;  LD HL, number ;; string
;;  CALL #PRINT
;;
;; func PRINT() -> void;
;;   reg A  -- as defined
;;   reg B  -- unaffected
;;   reg C  -- as deined
;;   reg DE -- unaffected
;;   reg HL -- as defined
#PRINT:
  LD A, (HL) ;; Get curr char
  INC HL     ;; Inc arg pointer
  OR A       ;; Check if line is ended (Set flag Z)
  RET Z      ;; Return if str is ended 
  OUT (C), A ;; Display char
  JR #PRINT-$

