;#################################
;  PROGRAM NAME: - TestLCD
;  DISCRIPTION:  - Display some simple words in 7-display
;  DATE: 30.10.23
;  AUTOR: Andrew Yushchenko
;#################################


ORG 0x0000
  JP SETUP

ORG 0x0010
RST10:
  CALL #LCD_OUT
  RET


ORG 0x0018
RST18:       ;; aka PRINT
  LD A, (HL) ;; Get curr char
  OR A       ;; Check if line is ended (Set flag Z)
  RET Z      ;; Return if str is ended 
  INC HL     ;; Inc arg pointer
  RST 0x10   ;; Output the char
  JR RST18-$

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
  LD SP, STACK      ; Set Memory Paging RAM
  IM 1              ; Use interrupt Mode 1

  LD A, 0x80  ; Set MODE 0;  A: OUTPUT; B: OUTPUT; C: OUTPUT
  OUT (PPI_PORT_CTRL), A ; Send instruction to PPI
  XOR A       ; Reset reg Acc
  OUT (PPI_PORT_A), A ; Reset PPI reg A
  OUT (PPI_PORT_B), A ; Reset PPI reg B, (MMU = 0)
  OUT (PPI_PORT_C), A ; Reset PPI reg C

  CALL _LCD_INIT

  LD HL, MSG_HELLO ; Load string ptr to reg HL
  RST 0x18    ; Print the string

  ; IN A, (0x00)
  ; OUT (0x50), A
TEMP:
  ; EI          ; Restore interrupts
  JP MAIN



  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (0x00)
  OUT (0x00), A

  LD HL, PTR_TEMP_WORD
  LD (HL), A
  PUSH HL
  CALL _HEX

  ; LD HL, KEY_CODE
  ; LD A, (HL)
  ; CALL DISPLAY_BYTE

  ; LD D, 0xFF
  ; CALL DELAY
  ; CALL _SCAN_CODE_HANDLE

  JP MAIN
  ; JP MAIN


#include "../lib/Hex.asm"
; #include "../lib/Printf.asm"
; #include "../lib/Utils.asm"
; #include "../lib/Keyboard.asm"
; #include "../lib/Buffer.asm"
#include "../lib/Lcd.asm"

MSG_HELLO:
  db "Hello world :)", 0