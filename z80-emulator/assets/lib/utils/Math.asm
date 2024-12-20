
;;
;; Example:
;;  LD D, 6    ; Dividend value
;;  LD E, 4    ; Divisior value
;;  CALL #DIV8 ; Will return result of 6 / 4  in reg D = 1 (aka quotient) & in reg A = 2 (aka remainder)
;;
;; proc DIV8(...regs) -> reg D & reg A;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- as defined
;;   reg HL -- unaffected
#DIV8:
  PUSH BC    ; Save reg BC in stack
  LD B, 8    ; Load amount of bites to shift
  XOR A      ; Reset reg A

#DIV8_lp:
  SLA D      ; Shift reg D to the left, aka D << 1
  RLA        ; Load carry bit, aka first bite of reg E in reg A
  CP E       ; Check if reg A < reg E
  JR C, #DIV8_lp_end-$; If reg A < reg E, then jump
  INC D      ; Get the result of division
  SUB E      ; Create a remainder in reg A
#DIV8_lp_end:
  DJNZ #DIV8_lp-$
  POP BC     ; Restore reg BC from stack
  RET

;;
;; Example:
;;  LD HL, 3602 ; Dividend value
;;  LD A, 60    ; Divisior value
;;  CALL #DIV16x8 ; Will return result of 3602 / 60 in reg HL = 60 (aka quotient)  & in reg A = 2 (aka remainder) 
;;
;; proc DIV16x8(...regs) -> reg HL & reg A;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#DIV16x8:
  PUSH BC    ; Save reg BC in stack
  LD C, A    ; Load the value of divisor to the reg C
  LD B, 16   ; Load amount of bites to shift
  XOR A      ; Reset reg A

#DIV16x8_lp:
  ADD HL, HL ; Shift reg HL to the left, aka HL << 1
  RLA        ; Load carry bit, aka first bite of reg HL in reg A
  CP C       ; Check if reg A < reg C
  JR C, #DIV16x8_lp_end-$; If reg A < reg C, then jump
  INC HL     ; Get the result of division
  SUB C      ; Create a remainder in reg A
#DIV16x8_lp_end:
  DJNZ #DIV16x8_lp-$
  POP BC     ; Restore reg BC from stack
  RET


;;
;; Example:
;;  LD HL, 60   ; Multiplicand value
;;  LD A, 60    ; Multiplier value
;;  CALL #MUL16x8 ; Will return result of 60 * 60 in reg HL = 3600 (aka product)
;;
;; proc MUL16x8(...regs) -> reg A & reg HL;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#MUL16x8:
  PUSH BC    ; Save reg BC in stack
  PUSH DE    ; Save reg DE in stack
  LD BC, 0x0800; Load amount of bites to shift
  LD DE, 0   ; Reset reg DE
  EX DE, HL  ; Move multiplicand value to reg DE

#MUL16x8_lp:
  ADD HL, HL ; Shift reg HL to the left, aka HL << 1
  RLA        ; Load carry bit, aka first bite of reg HL in reg A
  JR NC, #MUL16x8_lp_end-$; Check if carry bite is not set, aka last bite of reg A
  ADD HL, DE ; Get the result of multiplication
  ADC A, C   ; Add to Acc carry bit
#MUL16x8_lp_end:
  DJNZ #MUL16x8_lp-$
  POP DE     ; Restore reg HL from stack
  POP BC     ; Restore reg BC from stack
  RET