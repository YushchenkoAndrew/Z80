;####################################################
;  PROGRAM NAME: - Test
;  DISCRIPTION:  - Test NEW version of Seven-Segment Display
;  DATE: 14.02.21
;  AUTHOR: Andrew Yushchenko
;####################################################

  ORG 0x0000
  JP SETUP

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
  LD SP, STACK      ; Set Memory Paging RAM

  ; LD A, 0x80
  ; OUT (0x23), A

  ; LD A, 0x00
  ; OUT (0x21), A

  ; LD A, 0xF0
  ; OUT (0x20), A


  JP MAIN


  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (LED_PORT)
  OUT (LED_PORT), A

  LD HL, 0x5000
  LD (HL), A

  PUSH HL     ;; Send ptr to func
  CALL _HEX   ;; Display value in hex display
  JP MAIN

;; func HEX(byte*) -> void; exptects ptr to byte to display 
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


DELAY:
  NOP
  NOP
  NOP
  NOP
  NOP
  NOP
  DEC D
  JP NZ, DELAY
  RET

; Variables name
STACK          EQU 0x5FFF
LED_PORT       EQU 0x00
HEX_PORT       EQU 0x10

_HEX_DB:      ; This array of values need for display HEX number
  db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
