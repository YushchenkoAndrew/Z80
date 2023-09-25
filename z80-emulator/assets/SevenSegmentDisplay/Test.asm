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

  LD A, 0x80
  OUT (0x23), A

  LD A, 0x00
  OUT (0x21), A

  LD A, 0xF0
  OUT (0x20), A


  JP MAIN


  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (LED_PORT)
  OUT (LED_PORT), A

  PUSH AF     ;; Send arg to func
  CALL _HEX   ;; Display value in hex display
  JP MAIN

;; Funcion HEX(arg) exptects value to display to be in high byte
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
  JP C, _HEX_lp
  RET


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
