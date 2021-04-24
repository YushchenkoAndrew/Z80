;#################################
;	PROGRAM NAME: - Testing RAM
;	DISCRIPTION:  - Testing RAM, trying to write and read from RAM
;	DATE: 28.02.21
;	AUTOR: Andrew Yushchenko
;#################################

	ORG 0x2000

setup:
	DI
	LD SP, STACK      ; Set Memory Paging RAM

	; ;; Reset MMU reg
	; LD A, 0x00
	; OUT (0x70), A

	; Reset value in the BC reg
	LD BC, 0x0000
	JP main

;It`s main loop where all happend
main:
	IN A, (0x00)

	; Save value in the memory
	PUSH AF

	; Change value in the reg
	LD A, 0xFF


	; Restore from value from memory
	POP AF

	OUT (0x10), A

	JP main

STACK:          EQU 0x1FFF
