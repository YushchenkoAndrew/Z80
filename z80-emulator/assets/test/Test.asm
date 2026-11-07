;####################################################
;  PROGRAM NAME: - Test
;  DISCRIPTION:  - Test NEW version of Keyboard
;  DATE: 30.10.23
;  AUTHOR: Andrew Yushchenko
;####################################################
#include "../lib/Init.asm"
ALLOWED_INTERUPTS EQU IM_KEYBOARD


  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
; ORG 0x0060
SETUP:
  RET

#RTC_ALARM_EXEC:
_RTC_INIT:
  RET


  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (0x00)
  OUT (0x00), A

  ; RST 0x20

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
#include "../lib/EventLoop.asm"
#include "../lib/Keyboard.asm"
; #include "../lib/Buffer.asm"
#include "../lib/Lcd.asm"

#include "../lib/Timer.asm"
#include "../lib/Utils.asm"
#include "../lib/FileSystem.asm"
#include "../lib/utils/Math.asm"
#include "../lib/utils/String.asm"
#include "../lib/utils/Message.asm"