;####################################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Boot Loader used for loading program into RAM through UART
;	DATE: 26.03.21
;	AUTHOR: Andrew Yushchenko
;####################################################

	ORG 0x0000

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
	LD SP, STACK      ; Set Memory Paging RAM
  LD HL, PROGRAM    ; Load PTR for the RAM Program
  PUSH HL

	;; Reset MMU VALUE
  XOR A
	LD (MMU), A
	OUT (MMU_PORT), A
	LD (UART_INPUT), A
  LD B, A


  ;; Enable Interrupts
  LD A, 0b01100000                          ; KEY - DIS | Rx - EN | Tx - EN | RTC - DIS | TIMER - DIS
	LD (VECTOR_VAR), A
	OUT (VECTOR_PORT), A



	IM 1              ; Use interrupt Mode 1
	EI                ; Enable interrupts
  CALL UART_INIT
  JP MAIN

  ; ###################################################
  ; ############  INTERRUPT HANDLER    ################
  ; ###################################################
	ORG 0x0038
	DI                ; Disable interrupts
	EX AF, AF'        ; Save prev state
	EXX               ; Save prev state

  ; ###################################################
  ; ############  INTERRUPT VECTORS    ################
  ; ###################################################
	; IN A, (VECTOR_PORT)			; Get inturrupt vector
	; BIT KEY_BIT, A
	; CALL Z, KEY_INT		; If it's true then call this "func"

	IN A, (VECTOR_PORT)			; Get inturrupt vector
	BIT UART_Rx_BIT, A
	CALL Z, UART_Rx_INT		; If it's true then call this "func"

	IN A, (VECTOR_PORT)			; Get inturrupt vector
	BIT UART_Tx_BIT, A
	CALL Z, UART_Tx_INT		; If it's true then call this "func"

	EX AF, AF'        ; Restore prev state
	EXX               ; Restore prev state
	EI                ; Enable interrupts
	RET



  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
  IN A, (UART_SR_PORT)
  OUT (LED_PORT), A

  IN A, (SWITCH_PORT)
  OR A
  JP NZ, PROGRAM        ; If Switch is assign not into bootLoad mode then it will start program from the RAM

	LD A, (UART_INPUT)		; Get UART Input var from UART_REG
	OR A									; Get the flag based on reg 'A'
	JR Z, MAIN						; Return if Value is empty

	;; Wait until char UART will change Rx_RDY flag
CHKIO2:
	IN A, (UART_PORT)				; Get the Char which need to send
	IN A, (UART_SR_PORT)			; Get Status Reg
	BIT UART_Rx_RDY, A				; Check If Uart is holding data
	JR NZ, CHKIO2

	;; Restore Interrupt Vector value
	LD A, (VECTOR_VAR)
	OUT (VECTOR_PORT), A

	LD A, (UART_INPUT)			; And save it in Stack
  CP "0"                  ; Check the data which we recieved we are interesting in
	JR C, CN2								; If it less then "0" = 48 then skip


  CALL CHAR_INTO_NUM
  PUSH AF

  LD A, B
  OR A
  JP NZ, CN1

  POP AF
	OR 0x80								; Set mark that indicate that data is writen in reg 'B'
  LD B, A
  JR CN2

CN1:
	AND 0x0F							; Get only low bits without the mark

  ;; Shift Reg by 4 bit
	RL A
	RL A
	RL A
	RL A

  POP BC
  OR B

  ;; Write recieved byte into RAM
  POP HL
  LD (HL), A
  INC HL
  PUSH HL

  CALL HEX_DISPLAY
  LD B, 0x00

CN2:
  ;; Reset UART_INPUT var
  XOR A
  LD (UART_INPUT), A
	JP MAIN


CHAR_INTO_NUM:
  CP ":"                  ; Check if it's more then "9" char
	JR NC, CIN1 					; Check if more or qual then 58
  AND 0x0F                ; Get the num form NUMBER_CHAR
  RET

CIN1:
  AND 0x0F                ; Get the num form NUMBER_CHAR
  ADD A, 0x09             ; Add 0x09 becase "a" = 0x61
  RET

	;; ########################################################
	;; ################### UART Interface #####################
	;; ########################################################
UART_INIT:
  ; Load MR1
  LD A, UART_MR1
	OUT (UART_MR_PORT), A

  ; Load MR2
  LD A, UART_MR2
	OUT (UART_MR_PORT), A

  ; Load CR
  LD A, UART_CR
	OUT (UART_CR_PORT), A
	RET


UART_Rx_INT:
	; Disable interrupt from Rx
	LD A, (VECTOR_VAR)
	RES UART_Rx_BIT, A
	OUT (VECTOR_PORT), A

	IN A, (UART_PORT)				; Get the Char which need to send
	LD (UART_INPUT), A				; And save it in Stack
	RET


UART_Tx_INT:
	; Disable interrupt from Tx
	LD A, (VECTOR_VAR)
	RES UART_Tx_BIT, A
	OUT (VECTOR_PORT), A

	LD A, (UART_OUTPUT)				; Get the Char which need to send
	OUT (UART_PORT), A							; Write char into UART Holding reg
	RET

; ###################################################
; ##############    HEX DISPLAY   ###################
; ###################################################
GET_NUM:
	LD BC, 0x00						;; Reset reg BC value
	LD HL, NUM						;; Get addr of NUM
	AND 0x0F							;; Get only low bites
	LD C, A								;; Set addr offset for NUM
	ADD HL, BC						;; Get the correct pointer position
	LD A, (HL)						;; Get the value
	RET


HEX_DISPLAY:
	LD D, A								;; Save reg 'A' value in reg 'D'

	CALL GET_NUM					;; Input/Output value is located in reg 'A'
	OUT (HEX_PORT), A			;; Show LOW bites

	LD A, D						;; Restore save value

	;; Shift Reg by 4 bit
	RR A
	RR A
	RR A
	RR A

	CALL GET_NUM					;; Input/Output value is located in reg 'A'
	OR 0x80								;; Set to show in second display
	OUT (HEX_PORT), A			;; Show HIGHT bites
	RET





; Variables name
STACK 						EQU 0x1FFF
PROGRAM           EQU 0x2000

;; Stack Variables
MMU								EQU 0x1000
VECTOR_VAR				EQU 0x1001
UART_INPUT:				EQU 0x1002
UART_OUTPUT:			EQU 0x1003

;; Interrupt Vector Bits
KEY_BIT:					EQU 7
UART_Rx_BIT:			EQU 6
UART_Tx_BIT:			EQU 5
RTC_BIT 					EQU 4
TIMER_BIT 				EQU 3

;; Ports
SWITCH_PORT     	EQU 0x00
LED_PORT        	EQU 0x10
HEX_PORT        	EQU 0x20
DISPLAY_PORT    	EQU 0x30
KEYBOARD_PORT   	EQU 0x40
VECTOR_PORT     	EQU 0x50

UART_PORT:				EQU 0x60
UART_SR_PORT:			EQU 0x61
UART_MR_PORT:			EQU 0x62
UART_CR_PORT:			EQU 0x63

MMU_PORT        	EQU 0x70

TIMER_C0_PORT   	EQU 0x80
TIMER_C1_PORT   	EQU 0x81
TIMER_C2_PORT   	EQU 0x82
TIMER_MODE_PORT 	EQU 0x83


;; UART
UART_ERR:					EQU 0b00111000
UART_MR1:					EQU 0b01111110			;; 01 - one stop bit | 1 - Even | 1 - Parity EN | 11 - 8 bit | 10 - async 16X
UART_MR2:					EQU 0b01111110			;; 111 = I I 16X 16X | 1110 - Baud Rate 9600
; UART_CR:					EQU 0b00100110			;; 00 - Normal Mode | 1 - RTS Output Low | 0 - Normal on Error | 0 - Normal | 1 - RxEN Enable | 1 - DTR Data Terminal Ready | 0 - TxEN Disable
UART_CR:					EQU 0b00100110			;; 00 - Normal Mode | 1 - RTS Output Low | 0 - Normal on Error | 0 - Normal | 1 - RxEN Enable | 1 - DTR Data Terminal Ready | 0 - TxEN Disable
UART_CR_OUT_MODE:	EQU 0b00100111			;; 00 - Normal Mode | 1 - RTS Output Low | 0 - Normal on Error | 0 - Normal | 1 - RxEN Enable | 1 - DTR Data Terminal Ready | 0 - TxEN Disable

;; UART Command Reg Mask
UART_Tx_EN				EQU 0b00000001
UART_DATA_READY		EQU 0b00000010
UART_Rx_EN				EQU 0b00000100
UART_FORCE_BREAK	EQU 0b00001000
UART_ERR_RES			EQU 0b00010000			;; Mask Reset error (FE, OF, PE/DLE)
UART_REQ_SEND			EQU 0b00100000

;; UART Status Reg Mask
UART_Tx_RDY				EQU 0
UART_Rx_RDY				EQU 1
UART_Tx_EMT				EQU 2
UART_PE						EQU 3								;; Parity Error
UART_OVERRUN			EQU 4
UART_FE						EQU 5								;; Framing Error
UART_DATA_DETECT  EQU 6

;; ASCII SYMOBLS
SPACE           	EQU 0x20             ; Space
TAB             	EQU 0x09             ; HORIZONTAL TAB
CTRLC           	EQU 0x03             ; Control "C"
CTRLG           	EQU 0x07             ; Control "G"
BKSP            	EQU 0x08             ; Back space
LF              	EQU 0x0A             ; Line feed
CS              	EQU 0x0C             ; Clear screen
CR              	EQU 0x0D             ; Carriage return
CTRLO           	EQU 0x0F             ; Control "O"
CTRLQ	        		EQU 0x11	           ; Control "Q"
CTRLR           	EQU 0x12             ; Control "R"
CTRLS           	EQU 0x13             ; Control "S"
CTRLU           	EQU 0x15             ; Control "U"
ESC             	EQU 0x1B             ; Escape
DEL             	EQU 0x7F             ; Delete
DELLN            	EQU 0x7D             ; Delete Line
NULL	           	EQU 0x00             ; NULL CHAR
BKSL	          	EQU 0x5C             ; Backslash
UPARR	          	EQU 0x5E             ; UP-Arrow




NUM:			; This array of values need for display HEX number
	db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E