;####################################################
;  PROGRAM NAME: - Test
;  DISCRIPTION:  - Test NEW version of Keyboard
;  DATE: 30.10.23
;  AUTHOR: Andrew Yushchenko
;####################################################
#include "../../lib/Init.asm"
ALLOWED_INTERUPTS EQU IM_KEYBOARD


  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
; ORG 0x0060
SETUP:
  ; LD SP, STACK      ; Set Memory Paging RAM

  LD (PTR_PREV_SCAN_CODE), A
  LD (PTR_TEXT_BUFF_BGN), A
  LD (PTR_TEXT_BUFF_END), A
  LD (TEXT_BUF_MAP), A

  ;; Preinitilize ptr values
  LD A, 0xF1
  LD (SCAN_KEY_BUF), A ; Set scan code buff offset
  XOR A

  ; CALL _SCAN_CODE_INIT

  LD BC, .SUPER_BLOCK_ED-.SUPER_BLOCK
  LD HL, .SUPER_BLOCK
  LD DE, SUPER_BLOCK_MAP
  LDIR

  LD BC, .INODE_BLOCK_ED-.INODE_BLOCK
  LD HL, .INODE_BLOCK
  LD DE, INODE_MAP
  LDIR

  LD BC, .FILENAME_BLOCK_ED-.FILENAME_BLOCK
  LD HL, .FILENAME_BLOCK
  LD DE, DATA_ZONE_MAP
  LDIR

  CALL _LCD_INIT

  LD A, ">"   ; Print ">" which means that curr state is cmd mode
  LD (PTR_INPUT_STATE), A
  RST 0x10    ; Output the char

  RET


  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (0x00)
  OUT (0x00), A

  RST 0x20

  ; LD HL, KEY_CODE
  ; LD A, (HL)
  ; CALL DISPLAY_BYTE

  ; LD D, 0xFF
  ; CALL DELAY
  ; CALL _SCAN_CODE_HANDLE

  ; RST 0x08
  ; LD A, EVENT_PRIO_BG
  ; LD HL, MAIN
  ; CALL _EVENT_PUSH
  RET

  ; JP MAIN
  ; JP MAIN

;; TODO: Fix bug with double char when shift is pressed !!




; #include "../lib/Hex.asm"
; #include "../lib/Printf.asm"
#include "../../lib/EventLoop.asm"
#include "../../lib/Keyboard.asm"
#include "../../lib/Buffer.asm"
#include "../../lib/Lcd.asm"

#include "../../lib/Timer.asm"
#include "../../lib/Utils.asm"