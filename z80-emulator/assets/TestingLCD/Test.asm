;#################################
;	PROGRAM NAME: - TestLCD
;	DISCRIPTION:  - Display some simple words in 7-display
;	DATE: 27.02.21
;	AUTOR: Andrew Yushchenko
;#################################


	org 0x0000
	JP setup

number:
	db "Number: '%s'", 0

number2:
	db "123", 0

setup:
	; Set Memory Paging RAM
	LD SP, 0x5FFF

  ; Reset LCD
	; LD A, 0x00
	; OUT (0x30), A

	; LD BC, 0x00FF
	; XOR A
	; CALL delay

	; Print "Number"
	LD BC, number2 ;; args
	PUSH BC

	LD BC, 0x20 ;; PORT
	PUSH BC

	LD BC, number ;; string
	PUSH BC

	CALL _PRINTF

	JP main


main:
	; Show Number
	IN A, (0x00)
	OUT (0x00), A
	JP main



_PRINTF:
  POP DE     ;; Get return addr
  POP HL     ;; Get string addr
	POP BC     ;; Get output port

_PRINTF_lp:
	LD A, (HL) ;; Get curr char
	INC HL		 ;; Inc string pointer
	OR A			 ;; Check if line is ended (Set flag Z)
	JP Z, _PRINTF_esc
	CP "%"		 ;; Check if char need to be replaced
	JP Z, _PRINTF_chg
_PRINTF_out:
	OUT (C), A ;; Display char
	JP _PRINTF_lp

_PRINTF_chg:
	LD A, (HL) ;; Get char parametter
	INC HL		 ;; Inc string pointer

_PRINTF_chg_c:
	CP "c"		 ;; Check if parm is char
	JP NZ, _PRINTF_chg_s 
	POP AF     ;; Get char in reg F
	JP _PRINTF_out

_PRINTF_chg_s:
	CP "s"		 ;; Check if parm is string
	JP NZ, _PRINTF_chg_s 
	POP AF     ;; Get string addr
	PUSH HL    ;; Save current string addr
	PUSH DE    ;; Save return addr
	PUSH AF		 ;; Put string addr back to stack
	POP HL     ;; Get str addr to display
	LD DE, _PRINTF_chg_s_esc ;; Load to reg DE return addr
	JP _PRINTF_lp
_PRINTF_chg_s_esc:
	POP DE    ;; Restore return addr
	POP HL    ;; Restore string addr
	JP _PRINTF_lp

_PRINTF_chg_d:
	CP "d"		 ;; Check if parm is decimal
	JP NZ, _PRINTF_chg_x 
	POP AF     ;; Get dec number in reg A
	; ADD 0x00   ;; Add 0 to reg A only to set flags
	; DAA				 ;; Convert reg A from hex to number
	; TODO: impl this with DAA

_PRINTF_chg_x:
	CP "x"
	JP  NZ, _PRINTF_out
	;; TODO: Figure out code bellow
	; POP AF     ;; Get string addr
	; AND 0x0E 	 ;; Get only number from 0-7
  ; ADD A, 0x90;; Acc 90h-97h
  ; DAA        ;; 
  ; ADC A, 0x40;; Add '0' to reg A
  ; DAA

_PRINTF_esc:	
  PUSH DE    ;; Restore return addr
	RET
