; ###################################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Create small lib that will help to work on TinyBASIC
;	DATE: 05.02.21
;	AUTHOR: Andrew Yushchenko
; ###################################################

	ORG 0x0000
	JP SETUP

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
	LD SP, STACK      ; Set Memory Paging RAM
	IM 1              ; Use interrupt Mode 1
	EI                ; Enable interrupts

  ; Reset LCD
	LD A, LCD_RESET
	CALL SEND_LCD
	CALL CLEAR_LCD
	JP MAIN


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
	OUT (0x10), A
	; CP A, INT_KEY			; Check if Int
	BIT 7, A
	CALL Z, KEY_INT		; If it's true then call this "func"


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



	;; ########################################################
	;; ########## KEYBOARD Basic Functionality Keys ###########
	;; ########################################################
	LD A, D
	CP ENTER_KEY
	JP Z, ENTER_FUNC

	CP LEFT_ARROW_KEY
	JP Z, LEFT_ARROW_FUNC

	CP RIGHT_ARROW_KEY
	JP Z, RIGHT_ARROW_FUNC

	CP UP_ARROW_KEY
	JP Z, UP_ARROW_FUNC

	CP DOWN_ARROW_KEY
	JP Z, DOWN_ARROW_FUNC

	CP BACKSPACE_KEY
	JP Z, BACKSPACE_FUNC

	;; #########################################################


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
	LD A, D						; Copy reg 'D' into reg 'A'
	OUT (0x30), A     ; Send char to LCD

	;; Increment Cursor location on one
	LD HL, LCD_PTR		; Load address to LCD_PTR
	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	LD C, A						; Save prev value of reg 'A' in the reg 'C'
	INC A							; Shift Cursor to the right
	AND 0x1F					; Get only low bites and 2 lines
	LD (HL), A				; Save new value

	;; Save entered char in the LCD Stack
	LD HL, LCD_STACK 	; Load the start of LCD Stack
	LD L, C						; Move Cursor based on the PTR
	LD (HL), D				; Save entered char in the LCD Stack

	;; Testing....
	LD A, (HL)				;; Get written data
	CALL HEX_DISPLAY	; Show pointer at the HEX DISPLAY
	JP END_KEY_INT


	;; ########################################################
	;; ####### Create basic functionality for KEYBOARD ########
	;; ########################################################
ENTER_FUNC:
	LD A, LCD_RESET		; Reset LCD
	CALL SEND_LCD

	LD HL, LCD_PTR		; Load address to LCD_PTR
	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	LD (HL), 0x00			; Reset LCD_PTR
	LD L, A						; Move Cursor based on the PTR
	LD (HL), 0x10			; Put ASCII Code of 'Enter', that will indicate that string is ended

	; CALL CLEAR_LCD
	CALL EXEC

	LD L, 0x10				; Set addr to the position of a new line
	LD (HL), ">"			; Set the symbol of output result
	CALL REFRESH_LCD  ; Update LCD based on the saved data in LCD Stack

	JP END_KEY_INT

LEFT_ARROW_FUNC:
	LD A, LCD_LEFT		; Move Cursor Left
	CALL SEND_LCD

	LD HL, LCD_PTR		; Load address to LCD_PTR
	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	DEC A							; Shift Cursor to the Left
	AND 0x1F					; Get only low bites and 2 lines
	LD (HL), A				; Save new value
	JP END_KEY_INT


RIGHT_ARROW_FUNC:
	LD A, LCD_RIGHT		; Move Cursor Right
	CALL SEND_LCD

	LD HL, LCD_PTR		; Load address to LCD_PTR
	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	INC A							; Shift Cursor to the Left
	AND 0x1F					; Get only low bites and 2 lines
	LD (HL), A				; Save new value
	JP END_KEY_INT


UP_ARROW_FUNC:
	LD A, LCD_UP				; Move Cursor Up
	CALL SEND_LCD

	LD HL, LCD_PTR		; Load address to LCD_PTR
	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	SUB A, 	0x10			; Shift Cursor to the Prev Line
	AND 0x1F					; Get only low bites and 2 lines
	LD (HL), A				; Save new value
	JP END_KEY_INT


DOWN_ARROW_FUNC:
	LD A, LCD_DOWN		; Move Cursor Donw
	CALL SEND_LCD

	LD HL, LCD_PTR		; Load address to LCD_PTR
	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	ADD A, 	0x10			; Shift Cursor to the Next Line
	AND 0x1F					; Get only low bites and 2 lines
	LD (HL), A				; Save new value
	JP END_KEY_INT


BACKSPACE_FUNC:
	LD A, LCD_RESET		; Reset LCD
	CALL SEND_LCD

	LD HL, LCD_PTR		; Load address to LCD_PTR, in this case the step value
	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	DEC A							; Shift Cursor to the Left
	AND 0x1F					; Get only low bites and 2 lines
	LD (HL), A				; Save new value

	;; Delete prev char (change it to the SPACE)
	LD HL, LCD_STACK 	; Load the start of LCD Stack
	LD L, A						; Move Cursor based on the PTR
	LD (HL), " "			; Delete char in the LCD Stack

	CALL REFRESH_LCD  ; Update LCD based on the saved data in LCD Stack
	JP END_KEY_INT

RES_KEY:
  LD A, 0x00        ; Reset Key Code
  LD (HL), A        ; Reset Key Code in Stack

END_KEY_INT:
	RET


  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
	IN A, (0x00)
	; OUT (0x10), A
	; CALL HEX_DISPLAY	; Show pointer at the HEX DISPLAY
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

SEND_LCD:
	OUT (0x30), A			; Send to the LCD value from reg 'A'
	LD D, LCD_DELAY		; Create a small delay
	CALL DELAY
	RET

REFRESH_LCD:
	LD HL, LCD_PTR		; Load address to LCD_PTR, in this case the step value
	LD B, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	LD HL, LCD_STACK 	; Load the start of LCD Stack

LOOP_REFRESH:
	LD A, (HL)				; Get char from the stack
	CALL SEND_LCD

	LD A, 0x20				; Load the stack end
	INC L							; Increment Address
	CP L							; Check if the Address reached the end of the LCD Stack
	JP NZ, LOOP_REFRESH

	;; Return cursor to the prev position
	LD A, LCD_GO_RIGHT ; Reset LCD
	CALL SEND_LCD

	LD A, B						; Get LCD_PTR, it's saved only LOW BYTE
	CALL SEND_LCD
	RET

CLEAR_LCD:
	LD HL, LCD_STACK 	; Load the start of LCD Stack
	LD A, 0x20			 	; Reset reg 'A' value

LOOP_CLEAR:
	LD (HL), " "  		; Save Simple counter in the LCD Stack
	INC HL
	CP L
	JP NZ, LOOP_CLEAR
	RET

DELAY:
	NOP
	NOP
	NOP
	NOP
	DEC D
	JP NZ, DELAY
	RET



  ; ###################################################
  ; ##############  BASIC INTERPRETER  ################
  ; ###################################################

EXEC:
	LD HL, KEYWDS		 	; Load the start of LCD Stack
	LD DE, LCD_STACK 	; Load the start of LCD Stack

LOOP_EXEC:
	LD A, (DE)				; Get the char which is saved at the LCD Stack
	CP " "						; Compare if string reached the seperator
	JP Z, LOOP_EXEC_END	; Then end the loop

	CP (HL)						; Compare with KEYWDS
	JP NZ, EXEC_NOT_FOUND ; Jump if string is not equal

	INC DE						; Increment LCD ptr
	INC HL						; Increment KEYWDS ptr
	JP LOOP_EXEC

LOOP_EXEC_END:
	;; Find the address to the FUNC
	LD E, (HL)				; Load LOW BYTES
	INC HL
	LD D, (HL)				; Load HIGH BYTES
	EX DE, HL
	JP HL							; If KEYWDS was founded then jump into func
	; CALL PRINT

EXEC_NOT_FOUND:
	RET


  ; ###################################################
  ; ##############  DISPLAY COMMANDS   ################
  ; ###################################################

	ORG 0x0200
PRINT:
	LD BC, 0x06
	LD HL, LCD_STACK 	; Load the start of LCD Stack
	LD DE, LCD_STACK 	; Load the start of LCD Stack

	ADD HL, BC				; Move ptr to the start of the text
	EX DE, HL					; Exchange ptr with reg 'DE' this need because Acc only d opertaion with reg 'HL'
	LD C, 0x12				; Move ptr to the output mode
	ADD HL, BC				; Move ptr to the output position
	LD B, 0x0F				; Set the end value for the loop

LOOP_PRINT:
	LD A, (DE)				; Get the char which is saved at the LCD Stack
	LD (HL), A				; Copy the text as an output result

	INC DE						; Increment LCD ptr
	INC HL						; Increment OUTPUT ptr

	LD A, B 					; Restore the end value
	CP E							; Check if reg 'E' reached the end of the string
	JP NZ, LOOP_PRINT	; If no the do the loop again

	RET

; Variables name
STACK 						EQU 0x1FFF
INPUT 						EQU 0x1020

NUM_DELAY 				EQU 0x05
LCD_DELAY 				EQU 0xFF
INT_DELAY 				EQU 0x1F

;; Interrupt Vectors values
INT_KEY 					EQU 0x7F


LCD_PTR 					EQU 0x1022
LCD_STACK 				EQU 0x1000

;; LCD COMMANDS
LCD_RESET 				EQU 0x00
LCD_LEFT 					EQU 0x01
LCD_RIGHT			 		EQU 0x02
LCD_UP 						EQU 0x03
LCD_DOWN 					EQU 0x04
LCD_HOME 					EQU 0x05
LCD_BLINK 				EQU 0x06
LCD_BLINK 				EQU 0x06
LCD_GO_LEFT				EQU 0x07
LCD_GO_RIGHT			EQU 0x08

KEYBOARD_INPUT	 	EQU 0x1021
KEYBOARD_INT	 		EQU 0x03
UPSTROKE	 				EQU 0xF0
SPECIAL_COMMAND 	EQU 0xE0
NEXT_LINE 				EQU 0x0010

;; SP/2 Key code
ENTER_KEY 				EQU 0x5A
LEFT_ARROW_KEY	 	EQU 0x6B
RIGHT_ARROW_KEY 	EQU 0x74
UP_ARROW_KEY	 		EQU 0x75
DOWN_ARROW_KEY	 	EQU 0x72
BACKSPACE_KEY  		EQU 0x66



NUM:			; This array of values need for display HEX number
	db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E

KEYBOARD: ;; Key sequence:																			 1 	 2		3	 4	 5 6 7	8		  9		  A	  B	C D E
  db 0x00, "??????????????", 			0x00					     ;; 0x0X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
  db 0x00, "????Q1???ZSAW2", 			0x00					     ;; 0x1X -> ALT SHIFT ? CTRL Q 1 ?  ?     ?     Z   S A W 2
  db 0x00, "CXDE43?? VFTR5", 			0x00					     ;; 0x2X ->  C   X    D  E   4 3 ?  ?           V   F T R 5
  db 0x00, "NBHGY6???MJU78", 			0x00					     ;; 0x3X ->  N   B    H  G   Y 6 ?  ?     ?     M   J U 7 8
  db 0x00, ",KIO09??./L;P-", 			0x00					     ;; 0x4X ->  ,   K    I  O   0 9 ?  ?     .     /   L ; P -
  db 0x00, "?'?[=????",10,"]?\?", 0x00				       ;; 0x5X ->  ?   '    ?  [   = ? ? CAPS SHIFT ENTER ] ? \ ?
  db 0x00, "??????????????", 			0x00					     ;; 0x6X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
  db 0x00, "??????????????", 			0x00					     ;; 0x7X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?

KEYWDS:
	db "PRINT"
	dw PRINT