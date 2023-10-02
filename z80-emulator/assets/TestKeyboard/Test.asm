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



  ; IN A, (0x00)
  ; OUT (0x50), A
  IM 1              ; Use interrupt Mode 1
  EI                ; Enable interrupts
  JP MAIN


  ; ###################################################
  ; ############  INTERRUPT HANDLER    ################
  ; ###################################################
  org 0x0038
  EX AF, AF'        ; Save prev state
  EXX               ; Save prev state

  ; LD D, 0x05
  ; CALL DELAY

  IN A, (0x30)      ;; Reset RS-Trigger (Reset Initerrupt)
  LD HL, PTR_FUNC_ARGS

  LD (HL), A
	PUSH HL

	CALL _HEX

END_INIT:
  EX AF, AF'        ; Restore prev state
  EXX               ; Restore prev state
  EI                ; Enable interrupts
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
  JP MAIN

#include "../lib/Hex.asm"

