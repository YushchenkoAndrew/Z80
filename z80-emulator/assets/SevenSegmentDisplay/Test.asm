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
  IN A, (0x00)
  OUT (0x00), A
  CALL DISPLAY_BYTE


  LD D, 0xFF
  CALL DELAY
  JP MAIN

GET_NUM:
  LD BC, 0x00        ;; Reset reg BC value
  LD HL, NUM        ;; Get addr of NUM
  AND 0x0F          ;; Get only low bites
  LD C, A            ;; Set addr offset for NUM
  ADD HL, BC        ;; Get the correct pointer position
  LD A, (HL)        ;; Get the value
  RET


DISPLAY_BYTE:
  LD D, A            ;; Save reg 'A' value in reg 'D'

  CALL GET_NUM      ;; Input/Output value is located in reg 'A'
  OUT (0x10), A      ;; Show LOW bites

  LD A, D            ;; Restore save value

  ;; Shift Reg by 4 bit
  RR A
  RR A
  RR A
  RR A

  CALL GET_NUM      ;; Input/Output value is located in reg 'A'
  OR 0x80            ;; Set to show in second display
  OUT (0x10), A      ;; Show HIGHT bites

  LD A, D
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

NUM:      ; This array of values need for display HEX number
  db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
