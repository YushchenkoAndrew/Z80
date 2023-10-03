;####################################################
;  PROGRAM NAME: - Test
;  DISCRIPTION:  - Test NEW version of Keyboard
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

  ; ;; Set MODE 0;  A: OUTPUT; B: OUTPUT; C: OUTPUT
  ; LD A, 0x80
  ; OUT (0x23), A

  ; ;; SET Port A (LCD) = 0
  ; LD A, 0x00
  ; OUT (0x20), A

  ; ;; SET Port B (MMU) = 0
  ; LD A, 0x00
  ; OUT (0x21), A

  ; CALL _SCAN_CODE_INIT

  ; IN A, (0x00)
  ; OUT (0x50), A
  IM 1              ; Use interrupt Mode 1
  EI                ; Enable interrupts
  JP MAIN

;; Interrupt handler
ORG 0x0038
  PUSH AF     ; Save Acc & flags
  IN A, (0x30); Reset RS-Trigger (Reset Initerrupt)
  CALL #SCAN_CODE_IM
  POP AF      ; Restore AF reg
  EI          ; Restore interrupts
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
  CALL _SCAN_CODE_HANDLE
  JP MAIN

#include "../lib/Hex.asm"
#include "Keyboard.asm"
