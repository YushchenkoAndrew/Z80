; #include "Utils.asm"
;;
;; Example:
;;  LD HL, PTR_HEX_VAL
;;  LD (HL), A
;;  PUSH HL
;;  CALL _HEX   ;; Display value in hex display
;;
;;
;; func HEX(byte*) -> void;
;;    byte -- ptr to byte to display
;;
;; func #HEX() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#HEX:
  ; POP HL     ;; Get return ptr
  ; EX (SP), HL;; Restore return ptr & Get ptr to the value
  PUSH BC
  PUSH AF

  LD B, 0x08 ;; Run reverse loop 8 times
  LD A, (HL) ;; Get value to display in hex
  LD C, A    ;; Copy the value to reg C
#HEX_reverse:
  RL C       ;; Store last bit into carry flag
  RRA        ;; Put carry flag at the end of reg A, and store lower bit into carry flag
  DJNZ #HEX_reverse-$
  LD (HL), A ;; Save reversed byte in memory
  LD BC, 0x80;; Reset reg B & Set count & mask at the same time reg C

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

  POP AF
  POP BC
  RET

.HEX_DB:     ;; This array of values need for display HEX number
  db 0x00, 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
