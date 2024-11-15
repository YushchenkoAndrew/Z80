;####################################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Test UART Communication
;	DATE: 08.03.21
;	AUTHOR: Andrew Yushchenko
;####################################################

	ORG 0x0000
	JP SETUP

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
	LD SP, STACK      ; Set Memory Paging RAM
	IM 1              ; Use interrupt Mode 1
	EI                ; Enable interrupts

	; Configure UART interface
	CALL CONF_UART
	JP MAIN

  ; ###################################################
  ; ##############  CONFIGURE UART  ###################
  ; ###################################################
CONF_UART:
  ; Load MR1
  LD A, UART_MR1
	OUT (0x62), A

  ; Load MR2
  LD A, UART_MR2
	OUT (0x62), A

  ; Load CR
  LD A, UART_CR
	OUT (0x63), A
	RET

  ; ###################################################
  ; ############  INTERRUPT HANDLER    ################
  ; ###################################################
	ORG 0x0038
	DI                ; Disable interrupts
	EX AF, AF'        ; Save prev state
	EXX               ; Save prev state

	LD D, INT_DELAY
	CALL DELAY

  ; ###################################################
  ; ############  INTERRUPT VECTORS    ################
  ; ###################################################
	IN A, (0x50)			; Get inturrupt vector
	PUSH AF

	BIT KEY_BIT, A
	CALL Z, KEY_INT		; If it's true then call this "func"

	POP AF
	PUSH AF

	; IN A, (0x50)			; Get inturrupt vector
	BIT UART_Rx_BIT, A
	CALL Z, UART_Rx_INT		; If it's true then call this "func"

	POP AF

	; IN A, (0x50)			; Get inturrupt vector
	BIT UART_Tx_BIT, A
	CALL Z, UART_Tx_INT		; If it's true then call this "func"


END_INT:
	EX AF, AF'        ; Restore prev state
	EXX               ; Restore prev state
	EI                ; Enable interrupts
	RET


KEY_INT:
	;; Get interrupt data form Keyboard
	IN A, (0x40)
  LD D, A           ; Save reg 'A' value in reg 'D'

	;; Save input data into variable
	LD HL, KEYBOARD_INPUT			; Set location for variable position
	LD E, (HL)				; Restore prev value
	LD (HL), A				; Save Input value

	;; ########################################################
	;; ################### KEY Interface ######################
	;; ########################################################
  CP UPSTROKE       ; Check on upstroke
  JP Z, END_KEY_INT ; if it's an upstroke then end interrupt

	CP SPECIAL_COMMAND ; Check key code on E0
	JP Z, END_KEY_INT

  CP E              ; Check if prev value equal to curr
  JP Z, END_KEY_INT ; Then end interrupt

	AND E							; Check on special command after upstroke
	CP SPECIAL_COMMAND ; Check key code on E0
	JP Z, END_KEY_INT

  LD A, E           ; Move Prev Code Value to reg 'A'
  CP UPSTROKE       ; Check on upstroke
  JP Z, RES_KEY     ; If it was an upstroke then clear prev value and end interrupt

  ;; Run next on DOWNSTROKE
  LD HL, KEYBOARD   ; Load Pointer to KEYBOARD-ASCII Table
  LD BC, NEXT_LINE  ; Set Pointer step for new row
  LD A, D           ; Restore reg 'A' prev Value

TABLE_ROW:
  AND 0xF0          ; Get High bits
  JP Z, END_ROW     ; Check if it equal to zero
  LD A, D           ; Restore reg 'A' prev Value
  SUB 0x10          ; Start from 0 instead of 1
  LD D, A           ; Restore value
  ADD HL, BC        ; If no then shift pointer to next row
  JP TABLE_ROW

END_ROW:
  LD C, D           ; Restore Col num to reg 'C'
  ADD HL, BC        ; Shift pointer to the col num

  LD D, (HL)        ; Get ASCII character

  ; LD A, UART_CR			; Load UART Command Reg value
	LD A, UART_Tx_EN			; Reset Mask of Tx Enable (Set Tx to Disable)
	OR UART_REQ_SEND	; Reset Mask of Reqeust Send
	OUT (0x63), A			; Write Command Reg

	LD A, D						; Copy reg 'D' into reg 'A'
	; LD HL, UART_OUTPUT
	; LD (HL), A
	OUT (0x60), A			; Write Command Reg

	CALL HEX_DISPLAY	; Show pointer at the HEX DISPLAY
	JP END_KEY_INT

RES_KEY:
  LD A, 0x00        ; Reset Key Code
  LD (HL), A        ; Reset Key Code in Stack

END_KEY_INT:
	RET


UART_Rx_INT:
	IN A, (0x61)			; Get Status Reg
  LD D, A           ; Save reg 'A' value in reg 'D'

	BIT UART_Rx_RDY, A	; Check If Uart is holding data
	JP Z, END_UART_Rx_INT ; If not then end interrupt

	IN A, (0x60)			; Get Interrupt data from UART
	LD C, A						; Save reg 'A' value in reg 'C'
	LD A, D						; Restore Status Reg in reg 'A'

	AND UART_ERR			; Check recieved data on any errors
	JP NZ, UART_Rx_ERR_HANDLER		; If Some of the error flags is setted then call error handler func

	LD HL, UART_INPUT	; Set location for variable position
	LD (HL), C				; Save Input value
	LD A, C 					; Restore Data value into reg 'A'

	CALL HEX_DISPLAY	; Show saved value at the HEX DISPLAY
	JP END_UART_Rx_INT

UART_Rx_ERR_HANDLER:
  LD A, UART_CR			; Load UART Command Reg value
	OR UART_ERR_RES		; Set Mask to Reset Error Flags
	OUT (0x63), A			; Write Command Reg

END_UART_Rx_INT:
	RET

UART_Tx_INT:
; 	LD HL, UART_OUTPUT			; Set location for variable position
; 	LD A, (HL)				; Restore prev value

; 	CP 0x00
; 	JP Z, UART_Tx_INT_STOP

; 	OUT (0x60), A			; Write Command Reg
; 	LD (HL), 0x00

; UART_Tx_INT_STOP:
	LD A, UART_CR
	; LD D, UART_CR
	; LD A, UART_Tx_EN	; Reset Mask of Tx Enable (Set Tx to Disable)
	; OR UART_REQ_SEND	; Reset Mask of Reqeust Send
	; XOR 0xFF					; Revers Bits
	; AND D							; Get the final mask
	OUT (0x63), A			; Write Command Reg
	RET


  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
	;; Display UART Status Reg
	IN A, (0x61)			; Get Status Reg
	OUT (0x10), A
	JP MAIN

  ; ###################################################
  ; ##############    HEX DISPLAY   ###################
  ; ###################################################
GET_NUM:
	LD BC, 0x00				;; Reset reg BC value
	LD HL, NUM				;; Get addr of NUM
	AND 0x0F					;; Get only low bites
	LD C, A						;; Set addr offset for NUM
	ADD HL, BC				;; Get the correct pointer position
	LD A, (HL)				;; Get the value
	RET


HEX_DISPLAY:
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
	NOP
	NOP
	NOP
	NOP
	DEC D
	JP NZ, DELAY
	RET

; Variables name
STACK:            EQU 0x1FFF

INT_DELAY: 				EQU 0x0F

;; Interrupt Vector Bits
KEY_BIT:					EQU 7
UART_Rx_BIT:			EQU 6
UART_Tx_BIT:			EQU 5

KEYBOARD_INPUT: 	EQU 0x1021
KEYBOARD_INT: 		EQU 0x03
UPSTROKE: 				EQU 0xF0
SPECIAL_COMMAND: 	EQU 0xE0
NEXT_LINE: 				EQU 0x0010

UART_INPUT:				EQU 0x1022
UART_OUTPUT:			EQU 0x1023
UART_ERR:					EQU 0b00111000
UART_MR1:					EQU 0b01111110			;; 01 - one stop bit | 1 - Even | 1 - Parity EN | 11 - 8 bit | 10 - async 16X
UART_MR2:					EQU 0b01111110			;; 111 = I I 16X 16X | 1110 - Baud Rate 9600
UART_CR:					EQU 0b01000110			;; 01 - Echo Mode | 0 - RTS Output High | 0 - Normal on Error | 0 - Normal | 1 - RxEN Enable | 1 - DTR Data Terminal Ready | 0 - TxEN Disable

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



NUM:			; This array of values need for display HEX number
	db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E

KEYBOARD: ;; Key sequence:												1		2		 3	4		5	6	7	 8		 9		 A	B	 C D E
  db 0x00, "??????????????", 0x00     ;; 0x0X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
  db 0x00, "????Q1???ZSAW2", 0x00     ;; 0x1X -> ALT SHIFT ? CTRL Q 1 ?  ?     ?     Z   S A W 2
  db 0x00, "CXDE43?? VFTR5", 0x00     ;; 0x2X ->  C   X    D  E   4 3 ?  ?           V   F T R 5
  db 0x00, "NBHGY6???MJU78", 0x00     ;; 0x3X ->  N   B    H  G   Y 6 ?  ?     ?     M   J U 7 8
  db 0x00, ",KIO09??./L;P-", 0x00     ;; 0x4X ->  ,   K    I  O   0 9 ?  ?     .     /   L ; P -
  db 0x00, "?'?[=?????]?\?", 0x00     ;; 0x5X ->  ?   '    ?  [   = ? ? CAPS SHIFT ENTER ] ? \ ?
  db 0x00, "??????????????", 0x00     ;; 0x6X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
  db 0x00, "??????????????", 0x00     ;; 0x7X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
