
;; Example:
;;  IN A, (SWITCH_PORT)
;;  CALL #REVERSE; Will reverse reg A byte, mostly is used for SWITCH_PORT
;;
;; func INTR_MASK_SET() -> reg A;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#REVERSE:
  PUSH BC    ; Save reg BC in stack
  LD B, 8    ; Run reverse loop 8 times
  LD C, A    ; Copy the value to reg C
#REVERSE_lp:
  RL C       ; Store last bit into carry flag
  RRA        ; Put carry flag at the end of reg A, and store lower bit into carry flag
  DJNZ #REVERSE_lp-$
  POP BC     ; Restore reg BC
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
;;  LD A, 0x10  ;; Load number in BCD
;;  CALL #BCD_BIN
;;
;; proc BCD_BIN(...regs) -> reg A;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#BCD_BIN:
  PUSH BC    ;; Save reg BC value in stack
  LD C, A    ;; Store value in reg C
  AND 0xF0   ;; Get only high byte of the BCD value
  SRL A      ;; Shift left Acc
  LD B, A    ;; Save shifted Acc in reg B
  SRL A      ;; Shift left Acc
  SRL A      ;; Shift left Acc
  ADD A, B   ;; Add reg A and reg B, if reg A was intially 0x10(BCD), then reg A will be 0x0A
	LD	B, A   ;; Temporary save reg A in reg B
	LD	A, C   ;; Restore the original value of reg A
	AND	0x0F   ;; Get only low byte from the value
	ADD	A, B   ;; Add low byte with adjusted high byte
	POP	BC     ;; Restore reg BC
  RET