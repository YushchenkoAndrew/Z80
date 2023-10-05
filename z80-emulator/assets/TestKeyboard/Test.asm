;####################################################
;  PROGRAM NAME: - Test
;  DISCRIPTION:  - Test NEW version of Keyboard
;  DATE: 14.02.21
;  AUTHOR: Andrew Yushchenko
;####################################################

  ORG 0x0000
  JP SETUP

ORG 0x0008
RST8:
  ; TODO: Think about this
  CALL _ACK_BUFFERS; Check and reset state of the buf
  EI          ; Restore interrupts
  RET
  

ORG 0x0010
RST10:
  ; FIXME: Temp solution
  OUT (0x21), A
  RET

ORG 0x0018
RST18:
  CALL #TEMP_CMD
  RET

  ; JP SETUP

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
  LD SP, STACK      ; Set Memory Paging RAM
  IM 1              ; Use interrupt Mode 1

  ; ;; Set MODE 0;  A: OUTPUT; B: OUTPUT; C: OUTPUT
  ; LD A, 0x80
  ; OUT (0x23), A

  ; ;; SET Port A (LCD) = 0
  ; LD A, 0x00
  ; OUT (0x20), A

  ; ;; SET Port B (MMU) = 0
  ; LD A, 0x00
  ; OUT (0x21), A

  ;; Preinitilize ptr values
  LD A, 0xF1
  LD (SCAN_KEY_BUF), A ; Set scan code buff offset
  XOR A

  LD (PTR_PREV_SCAN_CODE), A
  LD (PTR_TEXT_BUFF_BGN), A
  LD (PTR_TEXT_BUFF_END), A
  ; CALL _SCAN_CODE_INIT

  ; IN A, (0x00)
  ; OUT (0x50), A
TEMP:
  EI          ; Restore interrupts
  JP MAIN

;; Interrupt handler
ORG 0x0038
  PUSH AF     ; Save Acc & flags
  IN A, (0x30); Reset RS-Trigger (Reset Initerrupt)
  CALL #SCAN_CODE_IM
  POP AF      ; Restore AF reg
  ; EI          ; Restore interrupts
  RET



  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (0x00)
  OUT (0x00), A

  ; LD HL, KEY_CODE
  ; LD A, (HL)
  ; CALL DISPLAY_BYTE

  ; LD D, 0xFF
  ; CALL DELAY
  ; CALL _SCAN_CODE_HANDLE

  RST 0x08
  JP MAIN
  ; JP MAIN

;; TODO: Fix bug with double char when shift is pressed !!


OK_MSG:
  db " OK", 0

#TEMP_CMD:
  PUSH HL
  PUSH BC

  LD HL, OK_MSG
  LD C, 0x21
  CALL #PRINT

  POP BC
  POP HL
  RET


; #include "../lib/Hex.asm"
; #include "../lib/Printf.asm"
#include "../lib/Utils.asm"
#include "Keyboard.asm"
#include "Buffer.asm"