; ###################################################
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Create small lib that will help to work on TinyBASIC
;	DATE: 05.02.21
;	AUTHOR: Andrew Yushchenko
; ###################################################


;*************************************************************
; *** ZERO PAGE SUBROUTINES ***
;
; THE Z80 INSTRUCTION SET ALLOWS FOR 8 ROUTINES IN LOW MEMORY
; THAT MAY BE CALLED BY RST 00H, 08H, 10H, 18H, 20H, 28H, 30H,
; AND 38H.  THIS IS A ONE BYTE INSTRUCTION, AND IS FUNCTIONALLY
; SIMILAR TO THE THREE BYTE INSTRUCTION 'CALL XXXX'. TINY BASIC
; WILL USE THE RST INSTRUCTION FOR THE 7 MOST FREQUENTLY USED
; SUBROUTINES. TWO OTHER SUBROUTINES (CRLF & TSTNUM) ARE ALSO
; IN THIS SECTION. THEY CAN BE REACHED WITH 'CALL'.
;*************************************************************

	ORG 0x0000
SETUP:
	;; COLD START
	LD SP, STACK      ; Set Memory Paging RAM
	LD A, 0x00				; This command need just to fill the gape
	JP INIT

;; *** TSTC OR RST 08H ***
;; IGNORE BLANKS AND
;;	TEST CHARACTER
; TODO:
RST08:
	EX (SP), HL
  RST 0x28
  CP (HL)
  JP TC1

CRLF:
  LD A, CR           ; Set into reg 'A' value to "Carriage return" (reset a device position to the beginning of a line)

;; *** OUTC or RST 10H ***
;; PRINT CHARACTER ONLY
;; IF OCSW SWITCH IS ON
;; REST OF THIS AT OUTC
RST10:
	PUSH AF           ; Save reg 'A' and Flag reg 'F' in the Stack
  LD A, (OCSW)      ; FIXME: ????
  OR A              ; Define/Update Acc Flags reg 'F'
  JP OUTC						; Call Output Function


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
	IN A, (VECTOR_PORT)			; Get inturrupt vector
	BIT KEY_BIT, A
	CALL Z, KEY_INT		; If it's true then call this "func"

	IN A, (VECTOR_PORT)			; Get inturrupt vector
	BIT UART_Rx_BIT, A
	CALL Z, UART_Rx_INT		; If it's true then call this "func"

	IN A, (VECTOR_PORT)			; Get inturrupt vector
	BIT UART_Tx_BIT, A
	CALL Z, UART_Tx_INT		; If it's true then call this "func"


END_INT:
	EX AF, AF'        ; Restore prev state
	EXX               ; Restore prev state
	EI                ; Enable interrupts
	RET

TC1:
	RET


	;; ########################################################
	;; ################### KEY Interrupt ######################
	;; ########################################################
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
	; LD A, D
	; CP ENTER_KEY
	; JP Z, ENTER_FUNC

	; CP LEFT_ARROW_KEY
	; JP Z, LEFT_ARROW_FUNC

	; CP RIGHT_ARROW_KEY
	; JP Z, RIGHT_ARROW_FUNC

	; CP UP_ARROW_KEY
	; JP Z, UP_ARROW_FUNC

	; CP DOWN_ARROW_KEY
	; JP Z, DOWN_ARROW_FUNC

	; CP BACKSPACE_KEY
	; JP Z, BACKSPACE_FUNC

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
	; OUT (0x30), A     ; Send char to LCD

	; ;; Increment Cursor location on one
	; LD HL, LCD_PTR		; Load address to LCD_PTR
	; LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
	; LD C, A						; Save prev value of reg 'A' in the reg 'C'
	; INC A							; Shift Cursor to the right
	; AND 0x1F					; Get only low bites and 2 lines
	; LD (HL), A				; Save new value

	; ;; Save entered char in the LCD Stack
	; LD HL, LCD_STACK 	; Load the start of LCD Stack
	; LD L, C						; Move Cursor based on the PTR
	; LD (HL), D				; Save entered char in the LCD Stack

	; ;; Testing....
	; LD A, (HL)				;; Get written data
	CALL HEX_DISPLAY	; Show pointer at the HEX DISPLAY
	JP END_KEY_INT


	;; ########################################################
	;; ####### Create basic functionality for KEYBOARD ########
	;; ########################################################
; ENTER_FUNC:
; 	LD A, LCD_RESET		; Reset LCD
; 	CALL SEND_LCD

; 	LD HL, LCD_PTR		; Load address to LCD_PTR
; 	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
; 	LD (HL), 0x00			; Reset LCD_PTR
; 	LD L, A						; Move Cursor based on the PTR
; 	LD (HL), 0x10			; Put ASCII Code of 'Enter', that will indicate that string is ended

; 	; CALL CLEAR_LCD
; 	CALL EXEC

; 	LD L, 0x10				; Set addr to the position of a new line
; 	LD (HL), ">"			; Set the symbol of output result
; 	CALL REFRESH_LCD  ; Update LCD based on the saved data in LCD Stack

; 	JP END_KEY_INT

; LEFT_ARROW_FUNC:
; 	LD A, LCD_LEFT		; Move Cursor Left
; 	CALL SEND_LCD

; 	LD HL, LCD_PTR		; Load address to LCD_PTR
; 	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
; 	DEC A							; Shift Cursor to the Left
; 	AND 0x1F					; Get only low bites and 2 lines
; 	LD (HL), A				; Save new value
; 	JP END_KEY_INT


; RIGHT_ARROW_FUNC:
; 	LD A, LCD_RIGHT		; Move Cursor Right
; 	CALL SEND_LCD

; 	LD HL, LCD_PTR		; Load address to LCD_PTR
; 	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
; 	INC A							; Shift Cursor to the Left
; 	AND 0x1F					; Get only low bites and 2 lines
; 	LD (HL), A				; Save new value
; 	JP END_KEY_INT


; UP_ARROW_FUNC:
; 	LD A, LCD_UP				; Move Cursor Up
; 	CALL SEND_LCD

; 	LD HL, LCD_PTR		; Load address to LCD_PTR
; 	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
; 	SUB A, 	0x10			; Shift Cursor to the Prev Line
; 	AND 0x1F					; Get only low bites and 2 lines
; 	LD (HL), A				; Save new value
; 	JP END_KEY_INT


; DOWN_ARROW_FUNC:
; 	LD A, LCD_DOWN		; Move Cursor Donw
; 	CALL SEND_LCD

; 	LD HL, LCD_PTR		; Load address to LCD_PTR
; 	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
; 	ADD A, 	0x10			; Shift Cursor to the Next Line
; 	AND 0x1F					; Get only low bites and 2 lines
; 	LD (HL), A				; Save new value
; 	JP END_KEY_INT


; BACKSPACE_FUNC:
; 	LD A, LCD_RESET		; Reset LCD
; 	CALL SEND_LCD

; 	LD HL, LCD_PTR		; Load address to LCD_PTR, in this case the step value
; 	LD A, (HL)				; Get LCD_PTR, it's saved only LOW BYTE
; 	DEC A							; Shift Cursor to the Left
; 	AND 0x1F					; Get only low bites and 2 lines
; 	LD (HL), A				; Save new value

; 	;; Delete prev char (change it to the SPACE)
; 	LD HL, LCD_STACK 	; Load the start of LCD Stack
; 	LD L, A						; Move Cursor based on the PTR
; 	LD (HL), " "			; Delete char in the LCD Stack

; 	CALL REFRESH_LCD  ; Update LCD based on the saved data in LCD Stack
; 	JP END_KEY_INT

RES_KEY:
  LD A, 0x00        ; Reset Key Code
  LD (HL), A        ; Reset Key Code in Stack

END_KEY_INT:
	RET

;; Result of operation
HOW:   db "HOW?",	 CR
OK:	   db "OK", 	 CR
WHAT:  db "WHAT?", CR
SORRY: db "SORRY", CR

;*************************************************************
;
; *** MAIN ***
;
; THIS IS THE MAIN LOOP THAT COLLECTS THE TINY BASIC PROGRAM
; AND STORES IT IN THE MEMORY.
;
; AT START, IT PRINTS OUT "(CR)OK(CR)", AND INITIALIZES THE
; STACK AND SOME OTHER INTERNAL VARIABLES.  THEN IT PROMPTS
; ">" AND READS A LINE.  IF THE LINE STARTS WITH A NON-ZERO
; NUMBER, THIS NUMBER IS THE LINE NUMBER.  THE LINE NUMBER
; (IN 16 BIT BINARY) AND THE REST OF THE LINE (INCLUDING CR)
; IS STORED IN THE MEMORY.  IF A LINE WITH THE SAME LINE
; NUMBER IS ALREADY THERE, IT IS REPLACED BY THE NEW ONE.  IF
; THE REST OF THE LINE CONSISTS OF A CR ONLY, IT IS NOT STORED
; AND ANY EXISTING LINE WITH THE SAME LINE NUMBER IS DELETED.
;
; AFTER A LINE IS INSERTED, REPLACED, OR DELETED, THE PROGRAM
; LOOPS BACK AND ASKS FOR ANOTHER LINE.  THIS LOOP WILL BE
; TERMINATED WHEN IT READS A LINE WITH ZERO OR NO LINE
; NUMBER; AND CONTROL IS TRANSFERED TO "DIRECT".
;
; TINY BASIC PROGRAM SAVE AREA STARTS AT THE MEMORY LOCATION
; LABELED "TXTBGN" AND ENDS AT "TXTEND".  WE ALWAYS FILL THIS
; AREA STARTING AT "TXTBGN", THE UNFILLED PORTION IS POINTED
; BY THE CONTENT OF A MEMORY LOCATION LABELED "TXTUNF".
;
; THE MEMORY LOCATION "CURRNT" POINTS TO THE LINE NUMBER
; THAT IS CURRENTLY BEING INTERPRETED.  WHILE WE ARE IN
; THIS LOOP OR WHILE WE ARE INTERPRETING A DIRECT COMMAND
; (SEE NEXT SECTION). "CURRNT" SHOULD POINT TO A 0.
;*************************************************************
RSTART:
	LD SP, STACK							; Set stack position

ST1:
	CALL CRLF									; Go to a new line
	LD DE, OK									; Load the addr of OK String
	XOR A											; Reset reg A VALUE
	CALL PRTSTR								; Print String
	LD HL, 0x0000							; Reset Reg HL
	LD (CURRNT), HL						; Set CURRNT PTR to 0x00

ST2:
	LD A, ">"									; Load '>' char
	CALL GETLN								; Read a line

	IN A, (SWITCH_PORT)
	OUT (LED_PORT), A
	JR ST2


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

;*************************************************************
;
; *** GETLN *** FNDLN (& FRIENDS) ***
;
; 'GETLN' READS A INPUT LINE INTO 'BUFFER'.  IT FIRST PROMPT
; THE CHARACTER IN A (GIVEN BY THE CALLER), THEN IT FILLS
; THE BUFFER AND ECHOS.  IT IGNORES LF'S AND NULLS, BUT STILL
; ECHOS THEM BACK.  RUB-OUT IS USED TO CAUSE IT TO DELETE
; THE LAST CHARACTER (IF THERE IS ONE), AND ALT-MOD IS USED TO
; CAUSE IT TO DELETE THE WHOLE LINE AND START IT ALL OVER.
; CR SIGNALS THE END OF A LINE, AND CAUSE 'GETLN' TO RETURN.
;
; 'FNDLN' FINDS A LINE WITH A GIVEN LINE # (IN HL) IN THE
; TEXT SAVE AREA.  DE IS USED AS THE TEXT POINTER.  IF THE
; LINE IS FOUND, DE WILL POINT TO THE BEGINNING OF THAT LINE
; (I.E., THE LOW BYTE OF THE LINE #), AND FLAGS ARE NC & Z.
; IF THAT LINE IS NOT THERE AND A LINE WITH A HIGHER LINE #
; IS FOUND, DE POINTS TO THERE AND FLAGS ARE NC & NZ.  IF
; WE REACHED THE END OF TEXT SAVE AREA AND CANNOT FIND THE
; LINE, FLAGS ARE C & NZ.
; 'FNDLN' WILL INITIALIZE DE TO THE BEGINNING OF THE TEXT SAVE
; AREA TO START THE SEARCH.  SOME OTHER ENTRIES OF THIS
; ROUTINE WILL NOT INITIALIZE DE AND DO THE SEARCH.
; 'FNDLNP' WILL START WITH DE AND SEARCH FOR THE LINE #.
; 'FNDNXT' WILL BUMP DE BY 2, FIND A CR AND THEN START SEARCH.
; 'FNDSKP' USE DE TO FIND A CR, AND THEN START SEARCH.
;*************************************************************
GETLN:
	RST 0x10								; Print the char wich in reg 'A'
	LD DE, BUFFER						; Load Buffer Addr

GL1:
	CALL CHKIO							; Ckeck on Inputs
	JR Z, GL1								; If value is 0x00 then check one more time
	CP DEL									; Delete last Char?
	JR Z, GL3								; If so then jump to GL3
	RST 0x10								; Echo back the input char
	CP LF										; Check if char is equal to LF (char '\n')
	JR Z, GL1								; If so then wait for the next char
	JR GL1
; 	OR A										; Check on char 'NULL'
; 	JR Z, GL1								; If so then ignore 'NULL' char
; 	CP DELLN								; Delete whole Line ?
; 	JR Z, GL4								; If yes then jump into GL4

; 	LD (DE), A							; Save Input char in Text Buffer
; 	INC DE									; Increement PTR
; 	CP CR										; Check if it the end of the string
; 	RET Z										; If Yes then end of line
; 	LD A, E									; If no then check if we have some spare space
; 	CP BUFEND AND 0xFF			; Check if we have char to delete ?
; 	JR NZ, GL1

GL3:
	LD A, E									; If no then check if we have some spare space
	CP BUFEND AND 0xFF			; Check if we have char to delete ?
	JR Z, GL4								; If no the redo whole line
	DEC DE									; If yes, then decrement PTR
	LD A, BKSL							; Load Basckslash Code into reg 'A' for output
	RST 0x10								; Output Backslash symbol
	JR GL1

; GL4:
; 	CALL CRLF								; Redo entire line
; 	LD A, UPARR							; Load symbol of UP-Arrow
; 	JR GETLN								; Get new Line

;*************************************************************
;
; *** PRTSTG *** QTSTG *** PRTNUM *** & PRTLN ***
;
; 'PRTSTG' PRINTS A STRING POINTED BY DE.  IT STOPS PRINTING
; AND RETURNS TO CALLER WHEN EITHER A CR IS PRINTED OR WHEN
; THE NEXT BYTE IS THE SAME AS WHAT WAS IN A (GIVEN BY THE
; CALLER).  OLD A IS STORED IN B, OLD B IS LOST.
;
; 'QTSTG' LOOKS FOR A BACK-ARROW, SINGLE QUOTE, OR DOUBLE
; QUOTE.  IF NONE OF THESE, RETURN TO CALLER.  IF BACK-ARROW,
; OUTPUT A CR WITHOUT A LF.  IF SINGLE OR DOUBLE QUOTE, PRINT
; THE STRING IN THE QUOTE AND DEMANDS A MATCHING UNQUOTE.
; AFTER THE PRINTING THE NEXT 3 BYTES OF THE CALLER IS SKIPPED
; OVER (USUALLY A JUMP INSTRUCTION.
;
; 'PRTNUM' PRINTS THE NUMBER IN HL.  LEADING BLANKS ARE ADDED
; IF NEEDED TO PAD THE NUMBER OF SPACES TO THE NUMBER IN C.
; HOWEVER, IF THE NUMBER OF DIGITS IS LARGER THAN THE # IN
; C, ALL DIGITS ARE PRINTED ANYWAY.  NEGATIVE SIGN IS ALSO
; PRINTED AND COUNTED IN, POSITIVE SIGN IS NOT.
;
; 'PRTLN' PRINTS A SAVED TEXT LINE WITH LINE # AND ALL.
;*************************************************************
PRTSTR:
	LD B, A										; Save reg 'A' value in reg 'B', for the comparing
PS1:
	LD A, (DE)								; Get the char from the pointer
	INC DE										; Bump pointer
	CP B											; Check if the saved value is the same with the read char
	RET Z											; If it is then return
	RST 0x10									; Else then Send this char by UART
	CP CR											; Check if it the end of the string
	JR NZ, PS1								; If not then do the loop one more time
	RET


;*************************************************************
;
; *** OUTC *** & CHKIO ***
;
; THESE ARE THE ONLY I/O ROUTINES IN TBI.
; 'OUTC' IS CONTROLLED BY A SOFTWARE SWITCH 'OCSW'.  IF OCSW=0
; 'OUTC' WILL JUST RETURN TO THE CALLER.  IF OCSW IS NOT 0,
; IT WILL OUTPUT THE BYTE IN A.  IF THAT IS A CR, A LF IS ALSO
; SEND OUT.  ONLY THE FLAGS MAY BE CHANGED AT RETURN. ALL REG.
; ARE RESTORED.
;
; 'CHKIO' CHECKS THE INPUT.  IF NO INPUT, IT WILL RETURN TO
; THE CALLER WITH THE Z FLAG SET.  IF THERE IS INPUT, Z FLAG
; IS CLEARED AND THE INPUT BYTE IS IN A.  HOWEVER, IF THE
; INPUT IS A CONTROL-O, THE 'OCSW' SWITCH IS COMPLIMENTED, AND
; Z FLAG IS RETURNED.  IF A CONTROL-C IS READ, 'CHKIO' WILL
; RESTART TBI AND DO NOT RETURN TO THE CALLER.
;
; Do not modify these routines.  Routines requiring
; modification are : UART_INIT, RX_RDY, and TX_RDY.
;*************************************************************
INIT:
	IM 1              		; Use interrupt Mode 1
	EI                		; Enable interrupts

	;; Reset MMU VALUE
	LD (MMU), A
	OUT (MMU_PORT), A
	LD (UART_INPUT), A

	;; FIXME: Temporary Solution, that will save DATA from INPUT SWITCH in the Stack
	IN A, (SWITCH_PORT)
	OUT (LED_PORT), A
	LD (OCSW), A

	CALL UART_INIT
	LD D, 0x19
PARLOP:									; This loop just clear the screen
	; CALL CRLF							; Clear the screen
	; DEC D
	; JR NZ, PARLOP
	XOR A									; Reset reg A VALUE
	LD DE, MSG1						; Print the boot message
	CALL PRTSTR						; Call function Print String
	; XOR A									; Reset reg A VALUE
	; LD DE, MSG2						; Print the boot message
	; CALL PRTSTR						; Call function Print String
	LD HL, SETUP
	LD (RANPNT), HL				; Set the Random value
	LD HL, TXTBGN					; Load the value of the start of TXT
	LD (TXTUNF), HL
	JP RSTART


OUTC:
	JR NZ, OUTC2					; Check if reg 'A' doesn't containe a zero, then output it
	POP AF								; Restore saved reg 'A' and Flags reg 'F'
	RET

OUTC2:
	POP AF								; Restore reg 'A'
	LD (UART_OUTPUT), A 	; Send char
	PUSH AF								; Save reg 'A'

	; LD A, UART_CR			; Set Mask of Tx Enable (Set Tx to Enable)
	; OR UART_Tx_EN			; Set Mask of Reqeust Send
	LD A, UART_Tx_EN			; Set Mask of Reqeust Send
	OR UART_REQ_SEND			; Set Mask of Reqeust Send
	OUT (UART_CR_PORT), A	; Write Command Reg

	;; Wait until char will be sended
OUTC3:
	LD A, (UART_OUTPUT)
	OR A
	JR NZ, OUTC3

	POP AF								; Restore reg 'A' char value
	CP CR									; Compare with "Carriage return" (reset a device position to the beginning of a line)
	RET NZ								; If its not equal then return
	LD A, LF							; Else then load char '\n' to reg 'A'
	RST 0x10							; Go directly at the addr = 0x10, Make an invisible loop
	LD A, CR							; And after just restore reg 'A' value
	RET

CHKIO:
	LD A, (UART_INPUT)		; Get UART Input var from UART_REG
	OR A									; Get the flag based on reg 'A'
	RET Z									; Return if Value is empty


	LD B, A								; Save char value in reg 'B'
	XOR LF								; Check if char is equal to LF (char '\n')
	JR Z, CHKIO2					; If so then reset this value in func CHKIO2

	XOR A									; Reset reg A value
	LD (UART_INPUT), A		; Reset UART Input var
	LD A, B								; Restore reg 'A' value

	CP CTRLO							; Check on CTRL-O
	JR NZ, CI1						; If not equal then end Input

	;; FIXME:	I have a physical Switches so ....
	; LD A, (OCSW)					; Get the SWITCH value
	; CPL										; Invert all bits of reg 'A'
	; LD (OCSW), A					; Save new value
	JR CHKIO


CHKIO2:
	LD (UART_INPUT), A		; Reset UART Input var
	RET										; Flag reg 'F' have 'Z' SET

CI1:
	CP CTRLC							; Check on Control-C
	RET NZ								; If not equal then return
	JP RSTART							; If its CTRL-C then Abord the line



MSG1:
  			; db ESC, "[2J", ESC, "[H"					;SCREEN CLEAR
        db "TINY BASIC FOR Z80", CR				;BOOT MESSAGE
MSG2:		db "    VERSION 1.0   ", CR

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
	IN A, (UART_SR_PORT)			; Get Status Reg
  LD D, A           ; Save reg 'A' value in reg 'D'

	IN A, (UART_PORT)			; Get Interrupt data from UART
	LD A, D						; Restore Status Reg in reg 'A'

	BIT UART_Rx_RDY, A	; Check If Uart is holding data
	RET Z ; If not then end interrupt


	AND UART_ERR			; Check recieved data on any errors
	JP NZ, UART_Rx_ERR_HANDLER		; If Some of the error flags is setted then call error handler func

	IN A, (UART_PORT)			; Get Interrupt data from UART
	LD (UART_INPUT), A				; Save Input value

	CALL HEX_DISPLAY	; Show saved value at the HEX DISPLAY
	RET

UART_Rx_ERR_HANDLER:
  LD A, UART_CR			; Load UART Command Reg value
	OR UART_ERR_RES		; Set Mask to Reset Error Flags
	OUT (UART_CR_PORT), A			; Write Command Reg
	RET


UART_Tx_INT:
	LD A, (UART_OUTPUT)				; Get the Char which need to send
	OUT (UART_PORT), A							; Write char into UART Holding reg

	OR A											; Check if the char is NULL
	JR Z, DISABLE_UART_Tx			; If so then disable UART_Tx

	OUT (UART_PORT), A							; Write char into UART Holding reg
	XOR A											; Reset Reg A
	LD (UART_OUTPUT), A				; And save it in Stack
	RET

DISABLE_UART_Tx:
	LD A, UART_CR							; Load basic configuration for UART
	OUT (UART_CR_PORT), A							; Write Command Reg
	RET




; Variables name
STACK 						EQU 0x1FFF
MMU_PORT					EQU 0x70

;; Stack Variables
INPUT 						EQU 0x1000
MMU								EQU 0x1001
KEYBOARD_INPUT	 	EQU 0x1002
UART_INPUT:				EQU 0x1003
UART_OUTPUT:			EQU 0x1004

OCSW            	EQU 0x1010          ;; SWITCH FOR OUTPUT
CURRNT          	EQU OCSW + 1        ;; POINTS FOR OUTPUT
STKGOS          	EQU OCSW + 3        ;; SAVES SP IN 'GOSUB'
VARNXT          	EQU OCSW + 5        ;; TEMP STORAGE
STKINP          	EQU OCSW + 7        ;; SAVES SP IN 'INPUT'
LOPVAR          	EQU OCSW + 9        ;; 'FOR' LOOP SAVE AREA
LOPINC          	EQU OCSW + 11       ;; INCREMENT
LOPLMT          	EQU OCSW + 13       ;; LIMIT
LOPLN           	EQU OCSW + 15       ;; LINE NUMBER
LOPPT           	EQU OCSW + 17       ;; TEXT POINTER
RANPNT          	EQU OCSW + 19       ;; RANDOM NUMBER POINTER
TXTUNF          	EQU OCSW + 21       ;; ->UNFILLED TEXT AREA

TXTBGN          	EQU 0x2000	        ;; TEXT SAVE AREA BEGINS
TXTEND          	EQU 0x2FFF          ;; TEXT SAVE AREA ENDS

NUM_DELAY 				EQU 0x05
LCD_DELAY 				EQU 0xFF
INT_DELAY 				EQU 0x1F

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


; LCD_PTR 					EQU 0x1022
; LCD_STACK 				EQU 0x1000

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

;; UART
UART_ERR:					EQU 0b00111000
UART_MR1:					EQU 0b01111110			;; 01 - one stop bit | 1 - Even | 1 - Parity EN | 11 - 8 bit | 10 - async 16X
UART_MR2:					EQU 0b01111110			;; 111 = I I 16X 16X | 1110 - Baud Rate 9600
; UART_CR:					EQU 0b00100110			;; 00 - Normal Mode | 1 - RTS Output Low | 0 - Normal on Error | 0 - Normal | 1 - RxEN Enable | 1 - DTR Data Terminal Ready | 0 - TxEN Disable
UART_CR:					EQU 0b00000110			;; 00 - Normal Mode | 1 - RTS Output Low | 0 - Normal on Error | 0 - Normal | 1 - RxEN Enable | 1 - DTR Data Terminal Ready | 0 - TxEN Disable

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

KEYBOARD: ;; Key sequence:																			 1 	 2		3	 4	 5 6 7	8		  9		  A	  B	C D E
  db 0x00, "??????????????", 			0x00					     ;; 0x0X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
  db 0x00, "????Q1???ZSAW2", 			0x00					     ;; 0x1X -> ALT SHIFT ? CTRL Q 1 ?  ?     ?     Z   S A W 2
  db 0x00, "CXDE43?? VFTR5", 			0x00					     ;; 0x2X ->  C   X    D  E   4 3 ?  ?           V   F T R 5
  db 0x00, "NBHGY6???MJU78", 			0x00					     ;; 0x3X ->  N   B    H  G   Y 6 ?  ?     ?     M   J U 7 8
  db 0x00, ",KIO09??./L;P-", 			0x00					     ;; 0x4X ->  ,   K    I  O   0 9 ?  ?     .     /   L ; P -
  db 0x00, "?'?[=????",10,"]?\?", 0x00				       ;; 0x5X ->  ?   '    ?  [   = ? ? CAPS SHIFT ENTER ] ? \ ?
  db 0x00, "??????????????", 			0x00					     ;; 0x6X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
  db 0x00, "??????????????", 			0x00					     ;; 0x7X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?

; KEYWDS:
	; db "PRINT"
	; dw PRINT
		ORG 0x1000
VARBGN: DS 55											;; Variables
BUFFER: DS 64											;; Input Buffer
BUFEND: DS 1											;; Buffer End
	END