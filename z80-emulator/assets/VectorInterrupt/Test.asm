;####################################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Test defining Interrrupt Vector
;	DATE: 18.02.21
;	AUTHOR: Andrew Yushchenko
;####################################################

	ORG 0x0000
	JP SETUP

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
	LD SP, STACK      ; Set Memory Paging RAM
	JP MAIN



  ; ###################################################
  ; ############  INTERRUPT HANDLER    ################
  ; ###################################################
  ORG 0x0038
	DI                ;; Disable interrupts
	EX AF, AF'        ;; Save prev state
	EXX               ;; Save prev state

  ;; Create a small delay, this need for the "init_vector_reg" to save the value
  LD D, INIT_DELAY
  CALL DELAY

  IN A, (0x50)
  IN A, (0x40)

  LD HL, INIT_VECTOR ; Load the address for the varaible that will save Interrupt Vector value
  LD (HL), A        ;; Save the value in the STACK

  ; IN A, (0x40)      ;; Reset RS-Trigger (Reset Initerrupt)

	EX AF, AF'        ;; Restore prev state
	EXX               ;; Restore prev state
	EI                ;; Enable interrupts
  RET



  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
	IN A, (0x00)
	OUT (0x10), A

  LD HL, INIT_VECTOR ;; Load the address for the varaible that will save Interrupt Vector value
  LD A, (HL)        ;; Get value from the variable INIT_VECTOR
	CALL DISPLAY_BYTE ;; Display INIT_VECTOR value
	JP MAIN

GET_NUM:
	LD BC, 0x00				;; Reset reg BC value
	LD HL, NUM				;; Get addr of NUM
	AND 0x0F					;; Get only low bites
	LD C, A						;; Set addr offset for NUM
	ADD HL, BC				;; Get the correct pointer position
	LD A, (HL)				;; Get the value
	RET


DISPLAY_BYTE:
	LD D, A						;; Save reg 'A' value in reg 'D'

	CALL GET_NUM			;; Input/Output value is located in reg 'A'
	OUT (0x20), A			;; Show LOW bites

	LD A, D						;; Restore save value

	;; Shift Reg by 4 bit
	RR A
	RR A
	RR A
	RR A

	CALL GET_NUM			;; Input/Output value is located in reg 'A'
	OR 0x80						;; Set to show in second display
	OUT (0x20), A			;; Show HIGHT bites
	RET


DELAY:
	DEC D
	JP NZ, DELAY
	RET


;; Variables name
STACK:          EQU 0x1FFF
INIT_DELAY:     EQU 0x0F
INIT_VECTOR:    EQU 0x1000

NUM:			;; This array of values need for display HEX number
	db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E
