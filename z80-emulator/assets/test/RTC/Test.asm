;####################################################
; PROGRAM NAME: - Test
; DISCRIPTION:  - Test Real-Time Clock
;	DATE: 13.12.24
; AUTHOR: Andrew Yushchenko
;####################################################
#include "../../lib/Init.asm"
ALLOWED_INTERUPTS EQU 0

#SCAN_CODE_IM			EQU RST38_end
#LCD_OUT 					EQU RST38_end


  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
	RET

  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (SWITCH_PORT)
  OUT (LED_PORT), A
	; PUSH AF

  ; RST 0x20

  ; POP AF
  ; LD HL, PTR_TEMP_WORD
  ; LD (HL), A
  LD A, (RTC_SECONDS)
  ; CALL #BIN_BCD

  ; LD D, A
  ; LD E, 10
  ; LD hl, 5
  ; CALL #MUL16x8
  ; LD A, L

  CALL #RTC_CURRENT_TIME
  LD A, L
  CALL #BIN_BCD


  CALL #HEX

	RET

#include "../../lib/EventLoop.asm"
#include "../../lib/Timer.asm"
#include "../../lib/RTC.asm"
#include "../../lib/Hex.asm"

#include "../../lib/utils/Adapter.asm"
#include "../../lib/utils/Math.asm"