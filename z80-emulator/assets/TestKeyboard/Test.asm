;####################################################
;  PROGRAM NAME: - Test
;  DISCRIPTION:  - Test NEW version of Keyboard
;  DATE: 14.02.21
;  AUTHOR: Andrew Yushchenko
;####################################################

  ORG 0x0000
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

  LD A, 0xF1
  LD (SCAN_KEY_BUF), A ; Set scan code buff offset
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
  JP PROCESS
  ; JP MAIN

;; TODO: Fix bug with double char when shift is pressed !!

PROCESS:
  LD HL, SCAN_KEY_BUF ; Load scan key buf offset ptr
  LD A, (HL)  ; Load scan key buf offset val
  AND 0x0F    ; Get only low bites
  DEC A       ; Check if offset is reseted, flag Z
  JP Z, TEMP       ; If nothing happend, then return

  LD D, A     ; Save offset value

  RRD         ; Load to reg A only low bit
  LD A, 0x01  ; Reset offset value to 1
  RLD         ; Reset offset value, aka start from 1

  PUSH HL     ;; Arg ptr, rand value

  LD BC, 0x21 ;; PORT
  PUSH BC

  INC HL
  LD C, D

  LD DE, PTR_FUNC_ARGS
  PUSH DE     ;; ptr to string

  LDIR        ; Copy the string to LCD

  XOR A       ; Reset reg A
  LD (DE), A  ; Mark the end of the string

  CALL _PRINTF

  EI          ; Restore interrupts
  JP TEMP

; #include "../lib/Hex.asm"
#include "../lib/Printf.asm"
#include "Keyboard.asm"
