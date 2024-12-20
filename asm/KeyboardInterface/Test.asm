;#################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Keyboard interface (Input Keyboard I2c) and displaying to SevenSegment and LCD
;	DATE: 05.02.21
;	AUTHOR: Andrew Yushchenko
;#################################

	org 0x0000
	JP setup


; Variables name
STACK: EQU 0x1FFF
INPUT: EQU 0x1000

NUM_DELAY: EQU 0x05
LCD_DELAY: EQU 0xFF
INIT_DELAY: EQU 0x1F

KEYBOARD_INPUT: EQU 0x1001
KEYBOARD_INIT: EQU 0x03
UPSTROKE: EQU 0xF0

setup:
	LD SP, STACK      ; Set Memory Paging RAM
	EI                ; Enable interrupts
	IM 1              ; Use interrupt Mode 1

  ; Reset LCD
	LD A, 0x00
	OUT (0x30), A

  ; Create a small delay before start
  LD D, LCD_DELAY
  CALL delay
	JP main


	;; Interrupt Handler
	org 0x0038
	DI                ; Disable interrupts
	EX AF, AF'        ; Save prev state
	EXX               ; Save prev state


	CALL KEY_INIT			; If it's true then call this "func"


END_INIT:
	EX AF, AF'        ; Restore prev state
	EXX               ; Restore prev state
	EI                ; Enable interrupts
	RET

KEY_INIT:
	LD D, INIT_DELAY
	CALL delay

	;; Get interrupt data form Keyboard
	IN A, (0x40)
  LD D, A           ; Save reg 'A' value in reg 'D'

	;; Save input data into variable
	LD HL, KEYBOARD_INPUT			; Set location for variable position
	LD E, (HL)				; Restore prev value
	LD (HL), A				; Save Input value

  CP UPSTROKE       ; Check on upstroke
  JP Z, END_KEY_INT     ; if it's an upstroke then end interrupt

  CP E              ; Check if prev value equal to curr
  JP Z, END_KEY_INT     ; Then end interrupt

  LD A, E           ; Move Prev Code Value to reg 'A'
  CP UPSTROKE       ; Check on upstroke
  JP Z, RES_KEY     ; If it was an upstroke then clear prev value and end interrupt

  ;; Run next on DOWNSTROKE
  LD HL, KEYBOARD   ; Load Pointer to KEYBOARD-ASCII Table
  LD BC, 0x0010     ; Set Pointer step for new row

TABLE_ROW:
  LD A, D           ; Restore reg 'A' prev Value
  SUB 0x10          ; Start from 0 instead of 1
  LD D, A           ; Restore value
  AND 0xF0          ; Get High bits
  JP Z, END_ROW     ; Check if it equal to zero
  ADD HL, BC        ; If no then shift pointer to next row
  JP TABLE_ROW

END_ROW:
  LD C, D           ; Restore Col num to reg 'C'
  ADD HL, BC        ; Shift pointer to the col num

  LD A, (HL)        ; Get ASCII character
	OUT (0x30), A     ; Send char to LCD

RES_KEY:
  LD A, 0x00        ; Reset Key Code
  LD (HL), A        ; Reset Key Code in Stack

END_KEY_INT:
	RET

	;; Start Program
	org 0x0085

main:
	IN A, (0x00)
	OUT (0x10), A
	JP main

delay:
	DEC D
	JP NZ, delay
	RET

NUM:			; This array of values need for display HEX number
	db 0x40, 0xF9, 0x24, 0x30, 0x19, 0x12, 0x02, 0xF8, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E

KEYBOARD: ;; Key sequence:
  db 0x00, "????Q1???ZSAW2", 0x00     ;; 0x1X -> ALT SHIFT ? CTRL Q 1 ?  ?     ?     Z   S A W 2
  db 0x00, "CXDE43?? VFTR5", 0x00     ;; 0x2X ->  C   X    D  E   4 3 ?  ?           V   F T R 5
  db 0x00, "NBHGY6???MJU78", 0x00     ;; 0x3X ->  N   B    H  G   Y 6 ?  ?     ?     M   J U 7 8
  db 0x00, ",KIO09??./L;P-", 0x00     ;; 0x4X ->  ,   K    I  O   0 9 ?  ?     .     /   L ; P -
  db 0x00, "?'?[=?????]?\?", 0x00     ;; 0x5X ->  ?   '    ?  [   = ? ? CAPS SHIFT ENTER ] ? \ ?
