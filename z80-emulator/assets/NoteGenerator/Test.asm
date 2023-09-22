;####################################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Test Note Generation with a Programmable Timer Device
;	DATE: 17.03.21
;	AUTHOR: Andrew Yushchenko
;####################################################

	ORG 0x0000
	JP SETUP

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
	LD SP, STACK      ; Set Memory Paging RAM

	;; Reset Timer delay
	LD A, 0x00
	LD (TIMER_VAR), A

  ;; Reset MMU Reg (with high bites which is qual to status bites)
  ; LD A, 0x30
  ; OUT (MMU_PORT), A

	CALL INIT_MMU

	;; Reset MUSIC PTR to the start of the song
	LD DE, MUSIC
	LD HL, MUSIC_PTR
	LD (HL), E
	INC HL
	LD (HL), D


	;; Counter0

  ;; Configure Controle world for Timer
  LD A, 0b00110110                ; Select Counter 0 | Low Byte and High Byte | Mode 3 | Bin Counter
  OUT (TIMER_MODE_PORT), A


  ;; Load Counter Word wich if equal to 2793
  ;; Counter have a freq = 4.9152MHz / 4
  ;; So the output square wave will have freq = (4.9152MHz / 4) / 2793 = 439.9HZ
  ;; Load Low Byte
  ; LD A, 0xE9
  LD A, 0x74
  OUT (TIMER_C0_PORT), A

  ;; Load High Byte
  ; LD A, 0x0A
  LD A, 0x05
  OUT (TIMER_C0_PORT), A


	;; Counter1

  ;; Configure Controle world for Timer
  LD A, 0b01111000                ; Select Counter 1 | Low Byte and High Byte | Mode 4 | Bin Counter
  OUT (TIMER_MODE_PORT), A

  ;; Load Counter Word wich if equal to 2793
  ;; Counter have a freq = 4.9152MHz / 4
  ;; So the output square wave will have freq = (4.9152MHz / 4) / 4800 = 256HZ
  ;; Load Low Byte
  LD A, 0xC0
  OUT (TIMER_C1_PORT), A

  ;; Load High Byte
  LD A, 0x12
  OUT (TIMER_C1_PORT), A



	; Enable interrrupt with Mode 1
	IM 1
	EI
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
	IN A, (VECTOR_PORT)	; Get inturrupt vector
	BIT TIMER_BIT, A
	CALL Z, TIMER_INT		; If it's true then call this "func"


END_INT:
	EX AF, AF'        ; Restore prev state
	EXX               ; Restore prev state
	EI                ; Enable interrupts
	RET


INIT_MMU:
	;; Set MODE 0;  A: OUTPUT; B: OUTPUT; C: OUTPUT
	LD A, 0x80
	OUT (0x23), A

	;; SET Port A (LCD) = 0
	LD A, 0x00
	OUT (0x20), A

	;; SET Port B (MMU) = 0
	LD A, 0x00
	OUT (0x21), A

	;; SET Port C (MMU) = 0x03
	; LD A, 0x03
	IN A, (SWITCH_PORT)
	OUT (0x22), A

	LD A, 0xFF
	OUT (VECTOR_PORT), A
	RET


	;; ########################################################
	;; ################## TIMER Interrupt #####################
	;; ########################################################
TIMER_INT:
	LD A, (TIMER_VAR)
	INC A
	LD (TIMER_VAR), A

	LD HL, MUSIC_PTR						;; Load Music Ptr
	LD E, (HL)									;; Get the LOW BYTE of the pointer
	INC HL											;; Move cursor
	LD D, (HL)									;; Get the HIGH BYTE of the pointer

	OR A
	JP NZ, END_TIMER_INT

	EX DE, HL										;; Switch addr value
	INC HL
	INC HL
	INC HL
	INC HL

	LD A, (HL)									;; Check the next Note, it can be a reset value
	XOR 0xFF
	JP NZ, END_RESTART_TIMER_INT

;; RESTART_TIMER_INT:
	LD DE, MUSIC
	LD HL, MUSIC_PTR						;; Load Music Ptr
	LD (HL), E
	INC HL
	LD (HL), D
	EX DE, HL
END_RESTART_TIMER_INT:


	LD B, (HL)									;; Get the LOW BYTE of the NOTE
	INC HL
	LD C, (HL)									;; Get the HIGH BYTE of the NOTE

	LD A, B
	OR C
	JP Z, DISABLE_TIMER

	; LD A, 0x30									;; Enable Note Timer

	;; SET Port C (MMU) = 0x03
	LD A, 0x03
	JP END_DISABLE_TIMER
DISABLE_TIMER:
	; LD A, 0x20									;; Disable Note Timer

	;; SET Port C (MMU) = 0x03
	LD A, 0x02
END_DISABLE_TIMER:
	; OUT (MMU_PORT), A
	OUT (0x22), A


	;; Update Timer 0 Value (Change NOTE)
	LD A, B
	OUT (TIMER_C0_PORT), A
	LD A, C
	OUT (TIMER_C0_PORT), A

	;; Skip Delay Word
	EX DE, HL
	DEC DE

	LD (HL), D									;; Load HIGH BYTE of the pointer
	DEC HL
	LD (HL), E									;; Load LOW BYTE of the pointer

END_TIMER_INT:
	;; Ignore Note WORD
	INC DE
	INC DE

	LD A, (DE)									;; Get the LOW BYTE of the DELAY
	OUT (TIMER_C1_PORT), A

	INC DE

	LD A, (DE)									;; Get the HIGH BYTE of the DELAY
	OUT (TIMER_C1_PORT), A
	RET



  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
	IN A, (SWITCH_PORT)
	OUT (LED_PORT), A

	CALL DISPLAY_BYTE
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
	OUT (HEX_PORT), A			;; Show LOW bites

	LD A, D						;; Restore save value

	;; Shift Reg by 4 bit
	RR A
	RR A
	RR A
	RR A

	CALL GET_NUM			;; Input/Output value is located in reg 'A'
	OR 0x80						;; Set to show in second display
	OUT (HEX_PORT), A			;; Show HIGHT bites

	LD A, D
	RET

; Variables name
STACK           	EQU 0x5FFF

TIMER_VAR        	EQU 0x4000
MUSIC_PTR        	EQU 0x4001


; ;; Ports													NEW PORT
; SWITCH_PORT     	EQU 0x00			;;	0x00
; LED_PORT        	EQU 0x10			;;	0x00
; HEX_PORT        	EQU 0x20			;;  0x10
; DISPLAY_PORT    	EQU 0x30			;;  0x20
; KEYBOARD_PORT   	EQU 0x40			;; 	0x40
; VECTOR_PORT     	EQU 0x50			;; 	0x50
; UART_PORT       	EQU 0x60			;; 	0x30
; MMU_PORT        	EQU 0x70			;; 	0x20
; TIMER_C0_PORT   	EQU 0x80			;;  0x00
; TIMER_C1_PORT   	EQU 0x81			;;  0x80
; TIMER_C2_PORT   	EQU 0x82
; TIMER_MODE_PORT 	EQU 0x83
; RTC_PORT					EQU 0xff			;; 	0x70
; 																;;  0x60  ???
;; Ports													NEW PORT
SWITCH_PORT     	EQU 0x00			;;	0x00
LED_PORT        	EQU 0x00			;;	0x00
HEX_PORT        	EQU 0x10			;;  0x10
DISPLAY_PORT    	EQU 0x20			;;  0x20
KEYBOARD_PORT   	EQU 0x40			;; 	0x40
VECTOR_PORT     	EQU 0x50			;; 	0x50
UART_PORT       	EQU 0x30			;; 	0x30
MMU_PORT        	EQU 0x20			;; 	0x20
TIMER_C0_PORT   	EQU 0x00			;;  0x00
TIMER_C1_PORT   	EQU 0x81			;;  0x80
TIMER_C2_PORT   	EQU 0x82
TIMER_MODE_PORT 	EQU 0x83
RTC_PORT					EQU 0x70			;; 	0x70
																;;  0x60  ???

;; Interrupt Vector Bits
KEY_BIT						EQU 7
UART_Rx_BIT 			EQU 6
UART_Tx_BIT 			EQU 5
RTC_BIT 					EQU 4
TIMER_BIT 				EQU 3


NUM:			; This array of values need for display HEX number
	db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E


;; This array is based on such style like:
;; Note div and Playing time of this note
;; For example (0x0AE9) - it's Note C4
;; 0xFFFF - its a restart value
;; and 0x12C0	- it will play a second
MUSIC:
	;; ***MARIO***
	dw NOTE_E7, 0x0190, NOTE_E7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_E7, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_C7, 0x0190, NOTE_E7, 0x0190, NOTE_DISABLE, 0x0190
	dw NOTE_G7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190
	dw NOTE_G6, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190

	dw NOTE_C7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190, NOTE_G6, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190, NOTE_E6, 0x0190, NOTE_DISABLE, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_A6, 0x0190, NOTE_DISABLE, 0x0190, NOTE_B6, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_AS6, 0x0190, NOTE_A6, 0x0190, NOTE_DISABLE, 0x0190

	dw NOTE_G6, 0x0215, NOTE_E7, 0x0215, NOTE_G7, 0x0215
	dw NOTE_A7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_F7, 0x0190, NOTE_G7, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_E7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_C7, 0x0190
	dw NOTE_D7, 0x0190, NOTE_B6, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190

	dw NOTE_C7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190, NOTE_G6, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190, NOTE_E6, 0x0190, NOTE_DISABLE, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_A6, 0x0190, NOTE_DISABLE, 0x0190, NOTE_B6, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_AS6, 0x0190, NOTE_A6, 0x0190, NOTE_DISABLE, 0x0190

	dw NOTE_G6, 0x0215, NOTE_E7, 0x0215, NOTE_G7, 0x0215
	dw NOTE_A7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_F7, 0x0190, NOTE_G7, 0x0190
	dw NOTE_DISABLE, 0x0190, NOTE_E7, 0x0190, NOTE_DISABLE, 0x0190, NOTE_C7, 0x0190
	dw NOTE_D7, 0x0190, NOTE_B6, 0x0190, NOTE_DISABLE, 0x0190, NOTE_DISABLE, 0x0190


	;; ***Star Wars***
	dw NOTE_A4, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_A4, 0x0960, NOTE_DISABLE, 0x00F0
	dw NOTE_A4, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_F4, 0x0690, NOTE_DISABLE, 0x00F0, NOTE_C5, 0x02D0, NOTE_DISABLE, 0x00F0
	dw NOTE_A4, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_F4, 0x0690, NOTE_DISABLE, 0x00F0, NOTE_C5, 0x02D0, NOTE_DISABLE, 0x00F0
	dw NOTE_A4, 0x0C30, NOTE_DISABLE, 0x00F0, NOTE_DISABLE, 0x0960

	dw NOTE_E5, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_E5, 0x0960, NOTE_DISABLE, 0x00F0
	dw NOTE_E5, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_F5, 0x0690, NOTE_DISABLE, 0x00F0, NOTE_C5, 0x02D0, NOTE_DISABLE, 0x00F0
	dw NOTE_GS4, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_F4, 0x0690, NOTE_DISABLE, 0x00F0, NOTE_C5, 0x02D0, NOTE_DISABLE, 0x00F0
	dw NOTE_A4, 0x0C30, NOTE_DISABLE, 0x00F0, NOTE_DISABLE, 0x0960

	dw NOTE_A5, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_A4, 0x05A0, NOTE_DISABLE, 0x00F0, NOTE_A4, 0x02D0, NOTE_DISABLE, 0x00F0
	dw NOTE_A5, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_GS5, 0x0618, NOTE_DISABLE, 0x00F0, NOTE_G5, 0x0348, NOTE_DISABLE, 0x00F0
	dw NOTE_FS5, 0x0258, NOTE_DISABLE, 0x00F0, NOTE_F5, 0x0258, NOTE_DISABLE, 0x00F0, NOTE_FS5, 0x04B0, NOTE_DISABLE, 0x00F0
	dw NOTE_DISABLE, 0x0618, NOTE_AS4, 0x04B0, NOTE_DISABLE, 0x00F0

	dw NOTE_DS5, 0x0960, NOTE_DISABLE, 0x00F0, NOTE_D5, 0x0618, NOTE_DISABLE, 0x00F0, NOTE_CS5, 0x0348, NOTE_DISABLE, 0x00F0
	dw NOTE_C5, 0x0258, NOTE_DISABLE, 0x00F0, NOTE_B4, 0x0258, NOTE_DISABLE, 0x00F0, NOTE_C5, 0x04B0, NOTE_DISABLE, 0x00F0
	dw NOTE_DISABLE, 0x0690




	dw 0xFFFF



;; Notes
NOTE_E0						EQU 0xE902
NOTE_F0						EQU 0xDBE1
NOTE_FS0					EQU 0xCF9C
NOTE_G0						EQU 0xC3EB
NOTE_GS0					EQU 0xB8E6
NOTE_A0						EQU 0xAE8B
NOTE_AS0					EQU 0xA4B8
NOTE_B0						EQU 0x9B7D
NOTE_C1						EQU 0x92C9
NOTE_CS1					EQU 0x8A87
NOTE_D1						EQU 0x82C1
NOTE_DS1					EQU 0x7B6C
NOTE_E1						EQU 0x7481
NOTE_F1  					EQU 0x6DF7
NOTE_FS1 					EQU 0x67C8
NOTE_G1  					EQU 0x61F5
NOTE_GS1 					EQU 0x5C77
NOTE_A1  					EQU 0x5745
NOTE_AS1 					EQU 0x5260
NOTE_B1  					EQU 0x4DBE
NOTE_C2  					EQU 0x4962
NOTE_CS2 					EQU 0x4543
NOTE_D2  					EQU 0x4160
NOTE_DS2 					EQU 0x3DB6
NOTE_E2  					EQU 0x3A3E
NOTE_F2  					EQU 0x36F9
NOTE_FS2 					EQU 0x33E4
NOTE_G2  					EQU 0x30FA
NOTE_GS2 					EQU 0x2E3A
NOTE_A2  					EQU 0x2BA2
NOTE_AS2 					EQU 0x2930
NOTE_B2  					EQU 0x26E0
NOTE_C3  					EQU 0x24B1
NOTE_CS3 					EQU 0x22A2
NOTE_D3  					EQU 0x20B0
NOTE_DS3 					EQU 0x1EDB
NOTE_E3  					EQU 0x1D1F
NOTE_F3  					EQU 0x1B7D
NOTE_FS3 					EQU 0x19F2
NOTE_G3  					EQU 0x187D
NOTE_GS3 					EQU 0x171D
NOTE_A3  					EQU 0x15D1
NOTE_AS3 					EQU 0x1498
NOTE_B3  					EQU 0x1370
NOTE_C4  					EQU 0x1258
NOTE_CS4 					EQU 0x1151
NOTE_D4  					EQU 0x1058
NOTE_DS4 					EQU 0x0F6D
NOTE_E4  					EQU 0x0E8F
NOTE_F4  					EQU 0x0DBE
NOTE_FS4 					EQU 0x0CF9
NOTE_G4  					EQU 0x0C3E
NOTE_GS4 					EQU 0x0B8E
NOTE_A4  					EQU 0x0AE8
NOTE_AS4 					EQU 0x0A4C
NOTE_B4  					EQU 0x09B8
NOTE_C5  					EQU 0x092C
NOTE_CS5 					EQU 0x08A8
NOTE_D5  					EQU 0x082C
NOTE_DS5 					EQU 0x07B6
NOTE_E5  					EQU 0x0747
NOTE_F5  					EQU 0x06DF
NOTE_FS5 					EQU 0x067C
NOTE_G5  					EQU 0x061F
NOTE_GS5 					EQU 0x05C7
NOTE_A5  					EQU 0x0574
NOTE_AS5 					EQU 0x0525
NOTE_B5  					EQU 0x04DC
NOTE_C6  					EQU 0x0496
NOTE_CS6 					EQU 0x0454
NOTE_D6  					EQU 0x0416
NOTE_DS6 					EQU 0x03DB
NOTE_E6  					EQU 0x03A3
NOTE_F6  					EQU 0x036F
NOTE_FS6 					EQU 0x033E
NOTE_G6  					EQU 0x030F
NOTE_GS6 					EQU 0x02E3
NOTE_A6  					EQU 0x02BA
NOTE_AS6 					EQU 0x0292
NOTE_B6  					EQU 0x026E
NOTE_C7  					EQU 0x024B
NOTE_CS7 					EQU 0x022A
NOTE_D7  					EQU 0x020B
NOTE_DS7 					EQU 0x01ED
NOTE_E7  					EQU 0x01D1
NOTE_F7  					EQU 0x01B7
NOTE_FS7 					EQU 0x019F
NOTE_G7  					EQU 0x0187
NOTE_GS7 					EQU 0x0171
NOTE_A7  					EQU 0x015D
NOTE_AS7 					EQU 0x0149
NOTE_B7  					EQU 0x0137
NOTE_C8  					EQU 0x0125
NOTE_CS8 					EQU 0x0115
NOTE_D8  					EQU 0x0105
NOTE_DS8 					EQU 0x00F6
NOTE_E8  					EQU 0x00E8
NOTE_F8  					EQU 0x00DB
NOTE_FS8 					EQU 0x00CF
NOTE_G8  					EQU 0x00C3
NOTE_GS8 					EQU 0x00B8
NOTE_A8  					EQU 0x00AE
NOTE_AS8 					EQU 0x00A4
NOTE_B8  					EQU 0x009B
NOTE_DISABLE			EQU 0x0000