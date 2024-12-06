;####################################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Test Note Generation with a Programmable Timer Device
;	DATE: 18.11.24
;	AUTHOR: Andrew Yushchenko
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
  ; OUT (LED_PORT), A
	; PUSH AF

  ; RST 0x20

  ; POP AF
  LD HL, PTR_TEMP_WORD
  LD (HL), A
  CALL #HEX

	RET

#include "../../lib/EventLoop.asm"
#include "../../lib/Timer.asm"
#include "../../lib/Hex.asm"