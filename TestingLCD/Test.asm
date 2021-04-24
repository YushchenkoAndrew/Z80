;#################################
;	PROGRAM NAME: - TestLCD
;	DISCRIPTION:  - Display some simple words in 7-display
;	DATE: 27.02.21
;	AUTOR: Andrew Yushchenko
;#################################


	org 0x2000
	JP setup

number:
	db "Number: ", 0

setup:
	; Set Memory Paging RAM
	LD SP, 0x1FFF

  ; Reset LCD
	LD A, 0x00
	OUT (0x30), A

	LD BC, 0x00FF
	XOR A
	CALL delay

	JP main


main:
  ; Set Cursor to HOME
	LD A, 0x05
	OUT (0x30), A

	LD BC, 0x00FF
	XOR A
	CALL delay


	; Print "Number"
	LD BC, number
	CALL print


	; Show Number
	IN A, (0x00)
	OUT (0x10), A

	ADD A, '0'
	; Send to Display
	OUT (0x30), A

	LD BC, 0x0FFF
	XOR A
	CALL delay
	JP main


delay:
	DEC BC
	CP B
	JP NZ, delay
	CP C
	JP NZ, delay
	RET


print:
	LD A, (BC)

	; Send to Display
	OUT (0x30), A
	INC BC

	; Save BC value in the RAM
	PUSH BC
	LD BC, 0x00FF
	XOR A
	CALL delay
	POP BC

	LD A, (BC)
	CP 0x00
	JP NZ, print
	RET
