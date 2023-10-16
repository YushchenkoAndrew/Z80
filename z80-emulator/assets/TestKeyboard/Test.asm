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

;; Interrupt handler
ORG 0x0038
  EXX         ; Save reg in alt regs
  PUSH AF     ; Save Acc & flags
  IN A, (0x30); Reset RS-Trigger (Reset Initerrupt)
  CALL #SCAN_CODE_IM
  POP AF      ; Restore AF reg
  EXX         ; Restore reg from alt regs
  ; EI          ; Restore interrupts
  RET

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

  LD BC, .SUPER_BLOCK_ED-.SUPER_BLOCK
  LD HL, .SUPER_BLOCK
  LD DE, SUPER_BLOCK_MAP
  LDIR

  CALL _LCD_INIT

  LD A, ">"   ; Print ">" which means that curr state is cmd mode
  LD (PTR_INPUT_STATE), A
  RST 0x10    ; Output the char

  ; IN A, (0x00)
  ; OUT (0x50), A
TEMP:
  EI          ; Restore interrupts
  JP MAIN



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




; #include "../lib/Hex.asm"
; #include "../lib/Printf.asm"
; #include "../lib/Utils.asm"
#include "../lib/Keyboard.asm"
#include "../lib/Buffer.asm"
#include "../lib/Lcd.asm"