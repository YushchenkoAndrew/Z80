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
;;   reg C  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- ptr to number
#HEX_ASCII:
  LD A, (HL) ;; Load value from ptr
  PUSH AF    ;; Store curr value in stack
  LD B, 0x02 ;; Aka display high & low bits
#HEX_ASCII_lp:
  XOR A      ;; Reset reg A
  RLD        ;; Load hight bits from (HL) -> A
  ADD A, 0x90;; Acc = 90h - 9Fh
  DAA        ;; Acc = 90h - 99h OR 00h - 15h
  ADC A, 0x40;; Add char 'A'
  DAA        ;; Acc = 30h - 39h (aka ascii 0 - 9) OR 40h - 45h (aka ascii 'A' - 'F')
  RST 0x10   ;; Display char
  DJNZ #HEX_ASCII_lp-$
  POP AF     ;; Restore reg A mem value
  LD (HL), A ;; Undo any changes
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
;;  LD A, "F"
;;  CALL #IS_HEX
;;
;; number: db 0x25
;;
;; proc IS_HEX() -> reg F(Z);
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#IS_HEX:
  PUSH BC     ; Save reg C in stack
  LD C, A     ; Save curr char in reg C
  CALL #ASCII_HEX; Convert Acc to hex if char is valid
  CP C        ; Check if char has changed
  POP BC      ; Restore reg C
  RET

;;
;; Example:
;;
;; proc STR_HEX(...regs) -> reg F(Z);
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- as defiend
;;   reg IX -- unaffected
;;
#STR_HEX:
  PUSH IX     ; Save reg IX in stack
  XOR A       ; Reset Acc
  LD (HL), A  ; Reset low byte of the addr
  PUSH HL     ; Temp save calculated addr 
  POP IX      ; Load this addr in reg IX

#STR_HEX_bgn:
  LD A, (HL)  ; Get low byte of the addr
  LD (IX+1), A; Shift low byte -> high byte
  XOR A       ; Reset inner loop counter, aka bit offset counter

#STR_HEX_lp:
  PUSH AF     ; Save bit offset cnt
  LD A, (DE)  ; Get buf char
  LD C, A     ; Save curr char in reg C
  CALL #ASCII_HEX; Convert Acc to hex if char is valid
  CP C        ; Check if char has changed
  JR Z, #STR_HEX_ret-$
  RLD         ; Save result memory
  INC DE      ; Move buf ptr to the next char
  POP AF      ; Get bit offset count
  OR A        ; Check if loop STR_HEX_lp happend once
  JR NZ, #STR_HEX_esc-$ ; If so then reset 
  INC A       ; Increment counter
  DJNZ #STR_HEX_lp-$
#STR_HEX_adj:
  INC HL      ; Get ptr to high byte addr
  XOR A       ; Reset Acc
  RRD         ; Shift value pointed by reg HL by 4, uses when there are 3 digit
  JR #STR_HEX_end-$

#STR_HEX_esc:
  LD A, (DE)  ; Get buf char
  CALL #IS_HEX; Check if the next char is hex or not
  JR Z, #STR_HEX_ret_end-$
  DJNZ  #STR_HEX_bgn-$
#STR_HEX_end:
  OR 0xFF     ; Reset Z flag
  POP IX      ; Restore reg IX
  RET

#STR_HEX_ret:
  POP AF      ; Get bit offset count
  OR A        ; Check if loop STR_HEX_lp happend once
  JR Z, #STR_HEX_ret_end-$
  INC HL      ; Get ptr to high byte addr
  XOR A       ; Reset Acc
  RRD         ; Shift value pointed by reg HL by 4, uses when there are 3 digit

#STR_HEX_ret_end:
  XOR A       ; Set flag Z
  POP IX      ; Restore reg IX
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

