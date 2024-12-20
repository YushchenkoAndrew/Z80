;#################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - This program test PPI on working
;	DATE: 14.02.20
;	AUTOR: Andrew Yushchenko
;#################################


	org 0x0000




setup:
	LD A, 82H
	OUT (53H), A

	LD B, 00h
	CALL TEST_INCLUDE

	JP main

main:
	IN A, (00H)
	OUT (10H), A


	OUT (50H), A

	IN A, (51H)
	OUT (40H), A

	LD A, B

	ADD 01H
	CP 10H
	JP NZ, elseJP
	LD A, 00H
	LD B, 00H

elseJP:
	LD B, A
	OUT (53H), A

	JP main

  org 0x1000
#include "TestInclude.asm"