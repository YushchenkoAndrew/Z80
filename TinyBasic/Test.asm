;*************************************************************
;	PROGRAM NAME: - Test
;	DISCRIPTION:  - Create small lib that will help to work on TinyBASIC
;	DATE: 27.03.21
;	AUTHOR: Andrew Yushchenko
;*************************************************************
;
;                 TINY BASIC FOR INTEL 8080
;                       VERSION 2.0
;                     BY LI-CHEN WANG
;                  MODIFIED AND TRANSLATED
;                    TO INTEL MNEMONICS
;                     BY ROGER RAUSKOLB
;                      10 OCTOBER,1976
;                        @COPYLEFT
;                   ALL WRONGS RESERVED
;
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
dwa MACRO #WHERE
	db (#WHERE >> 8) + 128
	db #WHERE AND 0xFF
	ENDM

	ORG 0x0000
SETUP:
	;; COLD START
	LD SP, STACK      ; Set Memory Paging RAM
	LD A, 0x00				; This command need just to fill the gape
	JP INIT

;; *** TSTC OR RST 08H ***
;; IGNORE BLANKS AND
;;	TEST CHARACTER
RST08:
	EX (SP), HL						; Get the ADDR for the char value near the CALL of this FUNC
  RST 0x28							; Skip all blanks
  CP (HL)								; Check the char
  JP TC1

CRLF:
  LD A, CR           ; Set into reg 'A' value to "Carriage return" (reset a device position to the beginning of a line)

;; *** OUTC or RST 10H ***
;; PRINT CHARACTER ONLY
;; IF OCSW SWITCH IS ON
;; REST OF THIS AT OUTC
RST10:
	PUSH AF           ; Save reg 'A' and Flag reg 'F' in the Stack
  LD A, (OCSW)      ; Load INIT Config
  OR A              ; Define/Update Acc Flags reg 'F'
  JP OUTC						; Call Output Function

	;;*** EXPR OR RST 18H ***
RST18:
	CALL EXPR2				; On the sign value
	PUSH HL
	JP EXPR1					; Check on Operation
	db 'w'

	;; Function to compeare a WORD
RST20:
	LD A, H
	CP D
	RET NZ
	LD A, L
	CP E
	RET
	db 'an'

RST28:
	LD A, (DE)				; Get the char from the Text Buffer
	CP SPACE					; Check if it equal to Space
	RET NZ						; If it not a space then return
	INC DE						; If it's a Space then take next char
	JP RST28					; Go one more time

RST30:
	POP AF						; Restore reg 'A'
	CALL FIN					; Check end of command
	JP QWHAT					; Print "What?" on Error
	db "G"

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

RST38:
	RST 0x28					; Skip all the Space
	SUB 0x40					; Check if the Char is a Symbol, not NUM
	RET C							; If Carry bit is set then, it's not a VAR
	JR NZ, TV1				; Check if char is NOT EQUAL to "@", NOT ARRAY
	INC DE						; If it's ARRAY
	;; TODO:
	RET

TV1:
	CP 0x1B						; Check on the Range of Upper Cased Char
	CCF								; Invert Carry Flag
	RET C							; Return if NOT(A < N) => if char is more then the range
	INC DE						; Get next char Value
	LD HL, VARBGN			; Get the VARBGN ADDR Sapce
	RLCA 							; Shift Left reg 'A' by 1
	ADD A, L					; Get the new Variable ADDR
	LD L, A						; Update ADDR
	LD A, 0x00				; Reset reg 'A'
	ADC A, H					; Sum regs with carry bit
	LD H, A						; Update ADDR
	RET								; RETURN to the FIRST CALLER


TC1:
	INC HL						; Move the ADDR to the Func ADDR, Flag reg is not affected
	JR Z, TC2					; Check (HL) char is equal to the reg 'A' (Curr Text char)
	PUSH BC						; Save reg 'C' in the Stack
	LD C, (HL)				; Load the low byte, a difference between JUMP ADDR
	LD B, 0x00				; Load the high byte
	ADD HL, BC				; Add difference to the SP ADDR
	POP BC						; Restore reg 'BC'
	DEC DE						; Move Text PTR Back, need for the next step

TC2:
	INC DE						; Move Text PTR UP
	INC HL						; Get the next JUMP ADDR
	EX (SP), HL				; Save Calculated ADDR at the Stack
	RET								; And return to the new location


TSTNUM:
	LD HL, 0x0000					; Reset reg 'HL'
	LD B, H								; Reset reg 'B'
	RST 0x28							;	Reach the next char which is NOT Space

TN1:
	CP "0"								; Check the range of the received char
	RET C									; If it less then "0" = 48 then return
	CP ":"								; After a "9" goest char ":"
	RET NC								; Check if more or qual then 58
	LD A, 0xF0
	AND H									; Get only high bits
	JR NZ, QHOW						; Check if there is no room for the next number
	INC B									; Count the digits
	PUSH BC								; Save curr count at the Stack

	;; Copy reg 'HL' -> 'BC'
	LD B, H
	LD C, L

	;; Calculation HL = HL * 10 + NEW_DIGIT (Transform DEC -> HEX)
	ADD HL, HL						; HL = HL + HL = 2 * HL
	ADD HL, HL						; HL = 2 * HL + 2 * HL = 4 * HL
	ADD HL, BC						; HL = 4 * HL + HL = 5 * HL
	ADD HL, HL						; HL = 5 * HL + 5 * HL = 10 * HL

	LD A, (DE)						; Get the char of NEW_DIGIT
	INC DE								; Move the PTR to the next char
	AND 0x0F							; Get the LOW Bits of the char ( 0x30 = "0", 0x31 = "1", "0x32" = "2" )
	ADD A, L							; Add prev value to the new value (based on hex)
	LD L, A								; Set new value to reg 'L'
	LD A, 0x00						; Reset reg 'A'
	ADC A, H							; Add the carry bit to high bits, set the flag 'P/V' (Overflow)
	LD H, A								; Set new value to reg 'H'
	POP BC								; Restore curr Count from Stack
	LD A, (DE)						; Get the next chat
	JP P, TN1							; Jump if no Overflow was detected in Operation HL + NEW_DIGIT

	; ***ERROR "HOW?"***
QHOW:
	PUSH DE								; Save curr char at the Stack
AHOW:
	LD DE, HOW						; Load addr of the messsage "HOW?"
	JP ERROR

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
	; Reset some values
	LD HL, 0x0000
	LD (LOPVAR), HL
	LD (STKGOS), HL

ST3:
	LD A, ">"									; Load '>' char
	CALL GETLN								; Read a line
	PUSH DE										; Save the end of the Text Buffer in the Stack
	LD DE, BUFFER							; Load the Start to the Text Buffer
	CALL TSTNUM								; Check if it a number
	RST 0x28									;	Reach the next char which is NOT Space

	;; Check if the NUM != 0x00, which mean that it didn't find any thing
	LD A, H
	OR L

	POP BC										; Get the end of the Text Buffer reg 'DE'
	JP Z, DIRECT							; If reg 'HL' = 0x00, then it's DIRECT COMMAND

	;; Save HL value at the Text Buffer
	DEC DE										; Move Text Buffer PTR to the SPACE
	LD A, H
	LD (DE), A
	DEC DE
	LD A, L
	LD (DE), A

	PUSH BC										; Save the end of the Text Buffer
	PUSH DE										; Save the start of the Text Buffer

	LD A, C											; Get the low bits of Text Buffer end
	SUB E												; Find difference (end - start)
	PUSH AF											; Save difference in the Stack

	CALL FNDLN
	PUSH DE											; Save the start of the Text Saved Area
	JR NZ, ST4									; If the value was not found
	PUSH DE											; If it was found then save DE one more time ???
	CALL FNDNXT									; Then Find the next Line = reg 'DE'

	POP BC											; Restore reg 'DE' saved value, Line that will be DELETED
	LD HL, (TXTUNF)							; Load PTR of the UNFILLED SAVE AREA
	CALL MVUP										; Overwrite the LINES

	;; Load the Text Saved Area PTR, var UNFILLED SAVE AREA
	LD H, B
	LD L, C
	LD (TXTUNF), HL							; Update PTR

ST4:
	POP BC											; Restore reg 'DE' saved value
	LD HL, (TXTUNF)							; Load PTR of the UNFILLED SAVE AREA
	POP AF											; Restore the saved difference between the end and start
	PUSH HL											; Save PTR of the UNFILLED SAVE AREA
	CP 0x03											; Check if the line only have NUM and CR
	JR Z, RSTART								; If so then Clear the Stack
	ADD A, L										; Add to the length start ptr of the Text Saved Area
	LD L, A											; Save this value
	LD A, 0x00									; Reset reg 'A'
	ADC A, H										; Add high byte with the carry bit
	LD H, A											; Update UNFILLED SAVE AREA to the new PTR
	LD DE, TXTEND								; Load the end of the Text Saved Area
	RST 0x20										; Compare Word
	JP NC, QSORRY								; If New calculated PTR is more or equal to the TXTEND, then ERROR
	LD (TXTUNF), HL							; Save new PTR of Text Saved Area
	POP DE											; Restore the prev value of the UNFILLED SAVE AREA
	CALL MVDOWN									; Create a empty Space for the new LINE
	POP DE											; Restore the start of the Text Buffer
	POP HL											; Restore the end of the Text Buffer
	CALL MVUP										; Copy the LINE Text Buffer -> Text Saved Area
	JR ST3

;*************************************************************
;
; *** UART_INIT *** UART_Rx_INT *** UART_Tx_INT ***
;
; THIS SECTION OF THE CODE HANDLE UART COMUNICATION AND
; INITIALIZATION
;
; UART_Rx_INT	WILL BE CALLED BY INTERRUP AND
; IT WILL RESET INTERRUPT FLAG AND SAVE THE VALUE
; AT THE VAR
;
; UART_Tx_INT	WILL BE CALLED BY INTERRUP AND
; IT WILL RESET INTERRUPT FLAG AND SEND THE VALUE
; TO THE UART REG HOLDER
;*************************************************************
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


;*************************************************************
;
; WHAT FOLLOWS IS THE CODE TO EXECUTE DIRECT AND STATEMENT
; COMMANDS.  CONTROL IS TRANSFERED TO THESE POINTS VIA THE
; COMMAND TABLE LOOKUP CODE OF 'DIRECT' AND 'EXEC' IN LAST
; SECTION.  AFTER THE COMMAND IS EXECUTED, CONTROL IS
; TRANSFERED TO OTHERS SECTIONS AS FOLLOWS:
;
; FOR 'LIST', 'NEW', AND 'STOP': GO BACK TO 'RSTART'
; FOR 'RUN': GO EXECUTE THE FIRST STORED LINE IF ANY, ELSE
; GO BACK TO 'RSTART'.
; FOR 'GOTO' AND 'GOSUB': GO EXECUTE THE TARGET LINE.
; FOR 'RETURN' AND 'NEXT': GO BACK TO SAVED RETURN LINE.
; FOR ALL OTHERS: IF 'CURRENT' -> 0, GO TO 'RSTART', ELSE
; GO EXECUTE NEXT COMMAND.  (THIS IS DONE IN 'FINISH'.)
;*************************************************************
;
; *** NEW *** STOP *** RUN (& FRIENDS) *** & GOTO ***
;
; 'NEW(CR)' SETS 'TXTUNF' TO POINT TO 'TXTBGN'
;
; 'STOP(CR)' GOES BACK TO 'RSTART'
;
; 'RUN(CR)' FINDS THE FIRST STORED LINE, STORE ITS ADDRESS (IN
; 'CURRENT'), AND START EXECUTE IT.  NOTE THAT ONLY THOSE
; COMMANDS IN TAB2 ARE LEGAL FOR STORED PROGRAM.
;
; THERE ARE 3 MORE ENTRIES IN 'RUN':
; 'RUNNXL' FINDS NEXT LINE, STORES ITS ADDR. AND EXECUTES IT.
; 'RUNTSL' STORES THE ADDRESS OF THIS LINE AND EXECUTES IT.
; 'RUNSML' CONTINUES THE EXECUTION ON SAME LINE.
;
; 'GOTO EXPR(CR)' EVALUATES THE EXPRESSION, FIND THE TARGET
; LINE, AND JUMP TO 'RUNTSL' TO DO IT.
;*************************************************************

NEW:
	CALL ENDCHK										; Check if the LINE reached the end
	LD HL, TXTBGN
	LD (TXTUNF), HL								; Reset PTR for Text Saved Area to the Start
																; Didn't use JP, I gues because he wanna to dicrease the size
STOP:
	CALL ENDCHK										; Check if the LINE reached the end
	JP RSTART											; Wait until the next command

RUN:
	CALL ENDCHK										; Check if the LINE reach the end
	LD DE, TXTBGN									; Load the PTR to Text Saved Area
RUNNXL:
	LD HL, 0x00										; Reset reg 'HL'
	CALL FNDLP										; Find the LINE
	JP C, RSTART									; If it reached the END then RESTART
RUNTSL:
	EX DE, HL											; Exchange reg 'DE' (PTR to Text Saved Area ) and 'HL' (??? LINE Counter)
	LD (CURRNT), HL								; Saved the CURR PTR to the TEXT Saved Area
	EX DE, HL											; Return values
	INC DE												; Skip Number
	INC DE												; Skip Number
RUNSML:
	CALL CHKIO										; Check on the Ctrl-C
	LD HL, TAB2 - 1								; Load the Statements TABLE
	JP EXEC												; Find the Command and execute it


GOTO:
	RST 0x18											; Get the result of the Expr
	PUSH DE												; Save PTR for Error Routine
	CALL ENDCHK										; Must find the END OF the LINE
	CALL FNDLN										; Find the TARGET LINE
	JP NZ, AHOW										; TARGET LINE not exist, then ERROR
	POP AF												; If TARGET LINE WAS FOUDND THEN JUMP INTO IT
	JR RUNTSL											; Run Operation


;*************************************************************
;
; *** LIST *** & PRINT ***
;
; LIST HAS TWO FORMS:
; 'LIST(CR)' LISTS ALL SAVED LINES
; 'LIST #(CR)' START LIST AT THIS LINE #
; YOU CAN STOP THE LISTING BY CONTROL C KEY
;
; PRINT COMMAND IS 'PRINT ....;' OR 'PRINT ....(CR)'
; WHERE '....' IS A LIST OF EXPRESIONS, FORMATS, BACK-
; ARROWS, AND STRINGS.  THESE ITEMS ARE SEPERATED BY COMMAS.
;
; A FORMAT IS A POUND SIGN FOLLOWED BY A NUMBER.  IT CONTROLS
; THE NUMBER OF SPACES THE VALUE OF A EXPRESION IS GOING TO
; BE PRINTED.  IT STAYS EFFECTIVE FOR THE REST OF THE PRINT
; COMMAND UNLESS CHANGED BY ANOTHER FORMAT.  IF NO FORMAT IS
; SPECIFIED, 6 POSITIONS WILL BE USED.
;
; A STRING IS QUOTED IN A PAIR OF SINGLE QUOTES OR A PAIR OF
; DOUBLE QUOTES.
;
; A BACK-ARROW MEANS GENERATE A (CR) WITHOUT (LF)
;
; A (CRLF) IS GENERATED AFTER THE ENTIRE LIST HAS BEEN
; PRINTED OR IF THE LIST IS A NULL LIST.  HOWEVER IF THE LIST
; ENDED WITH A COMMA, NO (CRLF) IS GENERATED.
;*************************************************************

LIST:
	CALL TSTNUM									; Get any number at the LINE
	CALL ENDCHK									; Check if the command reached the END of LINE
	CALL FNDLN									; Check if the PTR for Text Saved Area, DE = PTR to Text Saved Area

LS1:
	JP C, RSTART								; If it passed the size of RAM, then restart and what for the next
	CALL PRTLN									; Print the LINE
	CALL CHKIO									; Check if Ctrl-C was pressed
	CALL FNDLP									; Find next LINE
	JR LS1

PRINT:
	LD C, 6											; Load Num of Spaces, set offset
	RST 0x08										; Calculate next JUMP func based on next CHAR
	db SEMI											; Symbol ";"
	db PR2 - $ - 1							; Addr difference between PR2 and CURR pos
	CALL CRLF										; Go to the next Line
	JR RUNNXL										; Continue the rest of the Line

PR2:
	RST 0x08										; Calculate next JUMP func based on next CHAR
	db CR												; Symbol of Carry Line
	db PR0 - $ - 1							; Addr difference between PR2 and CURR pos
	CALL CRLF										; Go to the next Line
	JR RUNNXL										; Continue the rest of the Line

PR0:
	RST 0x08										; Calculate next JUMP func based on next CHAR
	db COMMENT									; Symbol "#"
	db PR1 - $ - 1							; Addr difference between PR1 and CURR pos
	RST 0x18										; TODO: ???
	LD C, L
	JR PR3

PR1:
	CALL QTSTR									; Check if it the string if no then it must be an exp
	JR PR8

PR3:
	RST 0x08										; Calculate next JUMP func based on next CHAR
	db COMMA										; Symbol ","
	db PR6 - $ - 1							; Addr difference between PR6 and CURR pos
	CALL FIN
	JR PR0											; Continue the list

PR6:
	CALL CRLF										; Go to the next Line
	RST 0x30

PR8:
	;; TODO:
	JR PR3

;*************************************************************
;
; *** HEXOUT *** & GETNUM ***
;
; HEXOUT WILL SHOW THE REG 'A' VALUE AT THE HEX DISPLAY
; THIS FUNCTION WILL CHANGE REG VALUE SUCH AS 'BC', 'HL', 'D'
;
; GETNUM IS A FUCNTION THAT WILL RETURN AT THE REG 'A'
;	A CORRECT VALUE THAT WILL BE DISPLAY ON THE SCREEN BASED ON
; THE LOW BIT
;*************************************************************
GETNUM:
	AND 0x0F
	LD HL, NUM						;; Get addr of NUM
	ADD A, L							;; Sum Low Bytes
	LD L, A								;; Update Value to the low byte reg 'L'
	LD A, 0x00						;; Reset reg 'A'
	ADC A, H							;; Add high bytes with a carry bit
	LD H, A								;; Update Value to the high byte reg 'H'
	LD A, (HL)						;; Get the value
	RET


HEXOUT:
	RST 0x18							; Get the result of the Expr
	CALL ENDCHK						; Must find the END OF the LINE

	LD A, L								;; Get the NUM value into reg 'A'
	PUSH AF
	CALL GETNUM						;; Input/Output value is located in reg 'A'
	OUT (HEX_PORT), A			;; Show LOW bites
	POP AF
	RRCA									; Shift reg 'A' Righ
	RRCA									; Shift reg 'A' Righ
	RRCA									; Shift reg 'A' Righ
	RRCA									; Shift reg 'A' Righ
	CALL GETNUM						;; Input/Output value is located in reg 'A'
	OR 0x80								;; Set to show in second display
	OUT (HEX_PORT), A			;; Show HIGHT bites

	INC DE								;; Skip CR char
	JP RUNNXL							; RUN NEXT LINE

;*************************************************************
;
; *** REM *** IF *** INPUT *** & LET (& DEFLT) ***
;
; 'REM' CAN BE FOLLOWED BY ANYTHING AND IS IGNORED BY TBI.
; TBI TREATS IT LIKE AN 'IF' WITH A FALSE CONDITION.
;
; 'IF' IS FOLLOWED BY AN EXPR. AS A CONDITION AND ONE OR MORE
; COMMANDS (INCLUDING OTHER 'IF'S) SEPERATED BY SEMI-COLONS.
; NOTE THAT THE WORD 'THEN' IS NOT USED.  TBI EVALUATES THE
; EXPR. IF IT IS NON-ZERO, EXECUTION CONTINUES.  IF THE
; EXPR. IS ZERO, THE COMMANDS THAT FOLLOWS ARE IGNORED AND
; EXECUTION CONTINUES AT THE NEXT LINE.
;
; 'INPUT' COMMAND IS LIKE THE 'PRINT' COMMAND, AND IS FOLLOWED
; BY A LIST OF ITEMS.  IF THE ITEM IS A STRING IN SINGLE OR
; DOUBLE QUOTES, OR IS A BACK-ARROW, IT HAS THE SAME EFFECT AS
; IN 'PRINT'.  IF AN ITEM IS A VARIABLE, THIS VARIABLE NAME IS
; PRINTED OUT FOLLOWED BY A COLON.  THEN TBI WAITS FOR AN
; EXPR. TO BE TYPED IN.  THE VARIABLE IS THEN SET TO THE
; VALUE OF THIS EXPR.  IF THE VARIABLE IS PROCEDED BY A STRING
; (AGAIN IN SINGLE OR DOUBLE QUOTES), THE STRING WILL BE
; PRINTED FOLLOWED BY A COLON.  TBI THEN WAITS FOR INPUT EXPR.
; AND SET THE VARIABLE TO THE VALUE OF THE EXPR.
;
; IF THE INPUT EXPR. IS INVALID, TBI WILL PRINT "WHAT?",
; "HOW?" OR "SORRY" AND REPRINT THE PROMPT AND REDO THE INPUT.
; THE EXECUTION WILL NOT TERMINATE UNLESS YOU TYPE CONTROL-C.
; THIS IS HANDLED IN 'INPERR'.
;
; 'LET' IS FOLLOWED BY A LIST OF ITEMS SEPERATED BY COMMAS.
; EACH ITEM CONSISTS OF A VARIABLE, AN EQUAL SIGN, AND AN EXPR.
; TBI EVALUATES THE EXPR. AND SET THE VARIABLE TO THAT VALUE.
; TBI WILL ALSO HANDLE 'LET' COMMAND WITHOUT THE WORD 'LET'.
; THIS IS DONE BY 'DEFLT'.
;*************************************************************

INPERR:
	;; TODO:
	JP RSTART

DEFLT:
	LD A, (DE)									; Get the Char on the Line
	CP CR												; If it's EMPTY LINE
	JR Z, LT1										; Else it's "LET"

LET:
	CALL SETVAL									; Save new variable
	RST 0x08										; Check if VAR is ARRAY
	db COMMA
	db LT1 - $ - 1
	JR LET											; Add ITEM BY ITEM
LT1:
	RST 0x30										; Check if it the END of the LINE

;*************************************************************
;
; *** EXPR ***
;
; 'EXPR' EVALUATES ARITHMETICAL OR LOGICAL EXPRESSIONS.
; <EXPR>::<EXPR2>
;         <EXPR2><REL.OP.><EXPR2>
; WHERE <REL.OP.> IS ONE OF THE OPERATORS IN TAB8 AND THE
; RESULT OF THESE OPERATIONS IS 1 IF TRUE AND 0 IF FALSE.
; <EXPR2>::=(+ OR -)<EXPR3>(+ OR -<EXPR3>)(....)
; WHERE () ARE OPTIONAL AND (....) ARE OPTIONAL REPEATS.
; <EXPR3>::=<EXPR4>(* OR /><EXPR4>)(....)
; <EXPR4>::=<VARIABLE>
;           <FUNCTION>
;           (<EXPR>)
; <EXPR> IS RECURSIVE SO THAT VARIABLE '@' CAN HAVE AN <EXPR>
; AS INDEX, FUNCTIONS CAN HAVE AN <EXPR> AS ARGUMENTS, AND
; <EXPR4> CAN BE AN <EXPR> IN PARANTHESE.
;*************************************************************
EXPR1:
	LD HL, TAB8 - 1							; Get the add of REL. OP
	JP EXEC											; Find Operation

XP11:													; REL. OP ">="
	CALL XP18										; Compare 2 WORDS Value
	RET C												; If NO, then return HL=0 (A < N)
	LD L, A											; If YES, then return HL=1
	RET

XP12:													; REL. OP "#", (OPERATION "NOT")
	CALL XP18										; Compare 2 WORDS Value
	RET Z												; False, Return HL=0
	LD L, A											; True, Return HL=1
	RET

XP13:													; REL. OP ">"
	CALL XP18										; Compare 2 WORDS Value
	RET Z												; False, if WORDS are EQUAL
	RET C												; False, if More then, HL=0
	LD L, A											; True, HL=1
	RET

XP14:													; REL. OP "<="
	CALL XP18										; Compare 2 WORDS Value
	LD L, A											; Set HL=1
	RET Z												; True
	RET C												; True
	LD L, H											; Reset HL, HL=0
	RET

XP15:													; REL. OP "="
	CALL XP18										; Compare 2 WORDS Value
	RET NZ											; False, Return HL=0
	LD L, A											; True, Return HL=1
	RET

XP16:													; REL. OP "<"
	CALL XP18										; Compare 2 WORDS Value
	RET NC											; False if A>=N, HL=0
	LD L, A											; True, HL=1
	RET

XP17:
	POP HL											; NOT REL. OP
	RET													; RETURN HL=<EXPR2>

XP18:
	LD A, C											; Save reg 'C' value
	POP HL											; Get 1 Value
	POP BC											; Get 2 Value
	PUSH HL											; Move Position values in the Stack
	PUSH BC											; Move Position values in the Stack
	LD C, A											; Restore reg 'C' value
	CALL EXPR2									; Run Expr2
	EX DE, HL										; Find 1 Value and save it in reg 'DE'
	EX (SP), HL									; Save 2 Value instead of 1 Value
	CALL CKHLDE									; Compare FIRST value with the SECOND one
	POP DE											; Restore Text PTR
	LD HL, 0x00									; Reset HL value, this operation doesn't change Flag reg
	LD A, 0x01									; Set reg 'A' 0x01, this operation dosn't change Flag reg
	RET

EXPR2:
	RST 0x08										; Check if the Num is Negative
	db "-"
	db XP21 - $ - 1
	LD HL, 0x00									; If yes, then imitate curr operation => 0 - NUM
	JR XP26
XP21:
	RST 0x08										; Check if the NUM is Positive
	db "+"
	db XP22 - $ - 1
XP22:
	CALL EXPR3									; Get the value
XP23:
	RST 0x08										; Check if is ADD operation
	db "+"
	db XP25 - $ - 1
	PUSH HL											; If Yes then Save NUM (reg 'HL')
	CALL EXPR3									; Get the Value
XP24:
	EX DE, HL										; Save reg 'HL' Value
	EX (SP), HL									; Get the 1 Exp Value, that was push at the Stack
	LD A, H											; Check Sign
	XOR D												; Get the Sign
	LD A, D											; Save reg 'D'
	ADD HL, DE									; Add two values, 16 bit Operation do not change Flag reg
	POP DE											; Restore Exp 1 from Stack
	JP M, XP23									; Check if Flag 'S' is set
	XOR H												; Check Sign one more time
	JP P, XP23									; Check if Flag 'S' is reset
	JP QHOW											; If else then we have OVERFLOW
XP25:
	RST 0x08										; Check if is SUB operation
	db "-"
	db XP42 - $ - 1
XP26:
	PUSH HL											; Save first Number
	CALL EXPR3									; Get Second Exp
	CALL CHGSGN									; Change NUM sign
	JR XP24											; And ADD them

EXPR3:
	CALL EXPR4									; Get the Value form Var, func
XP31:
	;; TODO:



EXPR4:
XP40:
XP42:

RND:
ABS:
SIZE:
	;; TODO:

;*************************************************************
;
; *** DIVIDE *** SUBDE *** CHKSGN *** CHGSGN *** & CKHLDE ***
;
; 'DIVIDE' DIVIDES HL BY DE, RESULT IN BC, REMAINDER IN HL
;
; 'SUBDE' SUBSTRACTS DE FROM HL
;
; 'CHKSGN' CHECKS SIGN OF HL.  IF +, NO CHANGE.  IF -, CHANGE
; SIGN AND FLIP SIGN OF B.
;
; 'CHGSGN' CHECKS SIGN N OF HL AND B UNCONDITIONALLY.
;
; 'CKHLDE' CHECKS SIGN OF HL AND DE.  IF DIFFERENT, HL AND DE
; ARE INTERCHANGED.  IF SAME SIGN, NOT INTERCHANGED.  EITHER
; CASE, HL DE ARE THEN COMPARED TO SET THE FLAGS.
;*************************************************************

DIVIDE:
	PUSH HL								; Save the HL Value
	LD L, H								; Load the high byte
	LD H, 0x00						; Reset high byte value
	CALL DV1
	LD B, C								; Save the result in the reg 'B'
	LD A, L								; Get the REAMAINDER of the DIVISION, HIGH BYTES
	POP HL								; Restore reg 'HL' value
	LD H, A								; Set the REMAINDER as the value
DV1:
	LD C, 0xFF						; Set all bits in the reg 'C', for reseting in the next step
DV2:
	INC C									; Increment the C value, show the result
	CALL SUBDE						; Devide by SUBTRACT
	JR NC, DV2						; Continue on sub util the Carry bit will set, that mean -> (prev HL) < DE
	ADD HL, DE						; Restore prev value of the HL
	RET

SUBDE:
	LD A, L								; Get the low byte value
	SUB E									; Sub the value from the byte
	LD L, A								; Save new value at the reg 'L'
	LD A, H								; Get the high byte value
	SBC A, D							; Sub with carry bit
	LD H, A								; Save new value at the reg 'H'
	RET


CHKSGN:
	LD A, H								; Load reg 'H'
	OR A									; Update Flag reg 'F'
	RET P									; Check if Flag 'S' is reset

CHGSGN:
	LD A, H								; Load reg 'H'
	PUSH AF								; Save the reg 'AF' at the Stack
	CPL										; Invert all bits of reg 'A'
	LD H, A								; Restore value reg 'H'
	LD A, L								; Load reg 'L'
	CPL										; Invert all bits of reg 'A'
	LD L, A								; Restore value reg 'L'
	INC HL								; Increment on one
	POP AF								; Restore prev reg 'A' value
	XOR H									; Check if all bytes will be set
	JP P, QHOW						; Check the Parity Flag bit, Parity is set if even num of bits set
	LD A, B								; Flip reg 'B'
	XOR 0x80							; Change sign value in the reg 'B'
	LD B, A								; Set new value at the reg 'B'
	RET

CKHLDE:
	LD A, H								;; Check the Sign first
	XOR D
	JP P, CK1							; If there is the same one then compare, Flag 'S' reset
	EX DE, HL							; If the value have diff Sign then swap values
CK1:
	RST 0x20							; Compare WORDS
	RET

;*************************************************************
;
; *** SETVAL *** FIN *** ENDCHK *** & ERROR (& FRIENDS) ***
;
; "SETVAL" EXPECTS A VARIABLE, FOLLOWED BY AN EQUAL SIGN AND
; THEN AN EXPR.  IT EVALUATES THE EXPR. AND SET THE VARIABLE
; TO THAT VALUE.
;
; "FIN" CHECKS THE END OF A COMMAND.  IF IT ENDED WITH ";",
; EXECUTION CONTINUES.  IF IT ENDED WITH A CR, IT FINDS THE
; NEXT LINE AND CONTINUE FROM THERE.
;
; "ENDCHK" CHECKS IF A COMMAND IS ENDED WITH CR.  THIS IS
; REQUIRED IN CERTAIN COMMANDS.  (GOTO, RETURN, AND STOP ETC.)
;
; "ERROR" PRINTS THE STRING POINTED BY DE (AND ENDS WITH CR).
; IT THEN PRINTS THE LINE POINTED BY 'CURRNT' WITH A "?"
; INSERTED AT WHERE THE OLD TEXT POINTER (SHOULD BE ON TOP
; OF THE STACK) POINTS TO.  EXECUTION OF TB IS STOPPED
; AND TBI IS RESTARTED.  HOWEVER, IF 'CURRNT' -> ZERO
; (INDICATING A DIRECT COMMAND), THE DIRECT COMMAND IS NOT
; PRINTED.  AND IF 'CURRNT' -> NEGATIVE # (INDICATING 'INPUT'
; COMMAND), THE INPUT LINE IS NOT PRINTED AND EXECUTION IS
; NOT TERMINATED BUT CONTINUED AT 'INPERR'.
;
; RELATED TO 'ERROR' ARE THE FOLLOWING:
; 'QWHAT' SAVES TEXT POINTER IN STACK AND GET MESSAGE "WHAT?"
; 'AWHAT' JUST GET MESSAGE "WHAT?" AND JUMP TO 'ERROR'.
; 'QSORRY' AND 'ASORRY' DO SAME KIND OF THING.
; 'AHOW' AND 'AHOW' IN THE ZERO PAGE SECTION ALSO DO THIS.
;*************************************************************

SETVAL:
	CALL RST38								; Save the Variable
	JP C, QWHAT								; Error Checking
	PUSH HL										; Save the variable ADDR
	RST 0x08									; Check if the next operation is ASSIGN
	db "="
	db SV1 - $ - 1
	RST 0x18									; Run Expression Hanlder
	LD B, H										; Get the Result into reg 'BC'
	LD C, L										; Get the Result into reg 'BC'
	POP HL										; Restore Variable ADDR
	LD (HL), C								; Save low byte of the result
	INC HL										; Move VAR PTR
	LD (HL), B								; Save high byte of the result
	RET

SV1:
	JP QWHAT									; Error Handler

FIN:
	RST 0x08									; Calculate next JUMP func based on next CHAR
	db SEMI										; Symbol ";"
	db FI1 - $ - 1						; Addr difference between FI1 and CURR pos
	POP AF										; TODO: ???
	JP RUNSML									; Continue the same LINE
FI1:
	RST 0x08									; Calculate next JUMP func based on next CHAR
	db CR											; Symbol of Carry Line
	db FI2 - $ - 1						; Addr difference between FI2 and CURR pos
	POP AF										; TODO: ???
	JP RUNNXL									; Run the NEXT LINE
FI2:
	RET

ENDCHK:
	RST 0x28									; Skip the space
	CP CR											; Check if the next char is NEW LINE
	RET Z

QWHAT:
	PUSH DE										; Save reg 'DE' value
AWHAT:
	LD DE, WHAT								; Load the "What" String
ERROR:
	XOR A											; Reset reg 'A'
	CALL PRTSTR								; Print the error message "WHAT?" || "HOW?" || "SORRY"
	POP DE										; Restore PTR to the last char
	LD A, (DE)								; Get that last char
	PUSH AF										; Save the last char, where old reg 'DE' was
	XOR A											; Reset reg 'A'
	LD (DE), A								; Put 0x00 where the error was happen
	LD HL, (CURRNT)						; Get the curr line
	PUSH HL										; Save curr line at the Stack
	LD A, (HL)								; Get the NUM of the LINE
	INC HL										; Move PTR UP
	OR (HL)										; Check if the NUM is Equal to 0x00
	POP DE										; Restore reg 'HL' into reg 'DE'
	JP Z, RSTART							; Restart if the NUM value is 0x00
	LD A, (HL)								; Check if NUM is Negative
	OR A											; Update the Flags
	JP M, INPERR							; If the Sign Flag is Set the Redo the Input
	CALL PRTLN								; Print the Current LINE
	DEC DE										; Return PTR to END CHAR at the CURR LINE
	POP AF										; Restore Last Char
	LD (DE), A								; Set the last Char at the Text Buffer (Area)
	LD A, QMARK								; Load the char "?"
	RST 0x10									; Print the "?" char
	XOR A											; Reset reg 'A'
	CALL PRTSTR								; Print the rest of the LINE
	JP RSTART

QSORRY:
	PUSH DE
ASORRY:
	LD DE, SORRY							; Load the "Sorry" Message
	JR ERROR									; Handle the Error

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

	OR A										; Check on char 'NULL'
	JR Z, GL1								; If so then ignore 'NULL' char
	CP DELLN								; Delete whole Line ?
	JR Z, GL4								; If yes then jump into GL4

	LD (DE), A							; Save Input char in Text Buffer
	INC DE									; Increement PTR
	CP CR										; Check if it the end of the string
	RET Z										; If Yes then end of line
	LD A, E									; If no then check if we have some spare space
	CP BUFEND AND 0xFF			; Check if we have char to delete ?
	JR NZ, GL1

GL3:
	LD A, E									; If no then check if we have some spare space
	CP BUFEND AND 0xFF			; Check if we have char to delete ?
	JR Z, GL4								; If no the redo whole line
	DEC DE									; If yes, then decrement PTR
	LD A, DEL								; Load DEL Code into reg 'A' for output
	RST 0x10								; Output Backslash symbol
	JR GL1

GL4:
	CALL CRLF								; Redo entire line
	LD A, UPARR							; Load symbol of UP-Arrow
	JR GETLN								; Get new Line


FNDLN:
	LD A, H									; Check a sign of reg 'HL'
	OR A										; It can't be '-' (Flag 'S' is set)
	JP M, QHOW							; If Flag 'S' is set then call the Error
	LD DE, TXTBGN						; Load the address of the Text Save Area

FNDLP:
FL1:
	PUSH HL									; Save entered num at the Stack
	LD HL, (TXTUNF)					; Get the PTR for the Text Saved Area ???
	DEC HL
	RST 0x20								; Check if the PTR located at the start
	POP HL									; Restore HL value
	RET C										; Return if PTR passed reg 'DE' value
	LD A, (DE)							; Get the low byte of the Num value
	SUB L										; This operation should give as a 0x00 result in reg 'A'
	LD B, A									; Put 0x00 into the reg 'B'
	INC DE									; Move Text ptr to the next value
	LD A, (DE)							; Get the high byte of the Num value
	SBC A, H								; Sub reg 'A' and 'H' with the carry bit
	JR C, FL2								; Check if the carry bit was set, that mean that loaded value is less then 'HL'
	DEC DE									; Restore Text ptr to the prev position
	OR B										; Set flag: NC, Z: Found; NC, NZ: Not Found
	RET

FNDNXT:
	INC DE
FL2:
	INC DE									; Move the Text ptr up
FNDSKP:
	LD A, (DE)							; Load the char from Text Buffer
	CP CR										; Find the end of the line
	JR NZ, FL2							; If not found yet then do it agian
	INC DE									; Move into the next Line
	JR FL1									; Find the Num one more time

;*************************************************************
;
; *** PRTSTR *** QTSTR *** PRTNUM *** & PRTLN ***
;
; 'PRTSTR' PRINTS A STRING POINTED BY DE.  IT STOPS PRINTING
; AND RETURNS TO CALLER WHEN EITHER A CR IS PRINTED OR WHEN
; THE NEXT BYTE IS THE SAME AS WHAT WAS IN A (GIVEN BY THE
; CALLER).  OLD A IS STORED IN B, OLD B IS LOST.
;
; 'QTSTR' LOOKS FOR A BACK-ARROW, SINGLE QUOTE, OR DOUBLE
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

QTSTR:
	RST 0x08									; Calculate next JUMP func based on next CHAR
	db QUOT										; Symbol '"'
	db QT3 - $ - 1							; Addr difference between QT3 and CURR pos
	LD A, QUOT								; Load '"' into reg 'A'
QT1:
	CALL PRTSTR								; Print string util it reach char '"' or "'"
	CP CR											; Check if the last char is CR
	POP HL										; Return the prev ADDR, which is recieved from the CALL, see "PR1" func
	JP Z, RUNNXL							; If it is true then run NEXT LINE
QT2:
	INC HL										;	Skip CALL initialization
	INC HL										;	Skip CALL initialization
	INC HL										;	Skip CALL initialization
	JP (HL)										; Jump into the jump operation
QT3:
	RST 0x08									; Calculate next JUMP func based on next CHAR
	db APOST									; Symbol "'"
	db QT4 - $ - 1						; Addr difference between QT4 and CURR pos
	LD A, APOST								; If yes then do the same as with '"'
	JR QT1										; Print string util it reach char "'"

QT4:
	;; FIXME: Don't know about "BACK-ARROW"
QT5:
	RET

PRTNUM:
	LD B, 0x00								; Reset reg 'B' without change Flag reg 'F'
	CALL CHKSGN								; Check the sign of the Num
	JP P, PN1									; Check if there NO SIGN (FLAG 'S' is reset)
	LD B, "-"									; Load the Minus Symbols
	DEC C											; Take space for the "-" symbol
PN1:
	PUSH DE										; Save the PTR to the Text Saved Area
	LD DE, 0x0000A						; Load the 10 into reg 'DE', which is show NUM base
	PUSH DE										; Save the NUM BASE value
	DEC C											; Reg 'C' = Spaces
	PUSH BC										; Save the Sign value and the Spaces value
PN2:
	CALL DIVIDE								; Divide HL by 10
	LD A, B										; Check if the value is 0
	OR C
	JR Z, PN3									; If yes then we got all, operation -> 0 / 10 = 0
	EX (SP), HL								; Exchange value with the saved reg 'BC', save the last value of DIV at the STACK
	DEC	L											; Decrement reg 'C' value, (Spaces num)
	PUSH HL										; Save the BC value again, shft BC value in the Stack
	LD H, B										; Move Result into reg 'HL'
	LD L, C										; Move Result into reg 'HL'
	JR PN2
PN3:
	POP BC										; Get reg 'BC'
PN4:
	DEC	C											; Decrement reg 'C' value, (Spaces num)
	LD A, C										; Check the Space Counter
	OR A											; Update Flags
	JP M, PN5									; Check if the Sign Flag is Set, wich mean that we reached the end of the blank Space
	LD A, SPACE								; Load the Space Symbol
	RST 0x10									; Print the Space Symbol
	JR PN4										; Check if there left some more
PN5:
	LD A, B										; Print Sign
	OR A											; Check if reg 'B' contian "-" Symbol
	CALL NZ, RST10						; If so then print the "-" Symbol
	LD E, L										; Load the last REMAINDER in reg 'E'
PN6:
	LD A, E										; Check digit in reg 'E'
	CP 0x0A										; Check the value is no more 10
	POP DE										; Get the Results
	RET Z
	ADD A, 0x30								; Else convert to ASCII
	RST 0x10									; Print Symbol
	JR PN6										; Do it one more time


PRTLN:
	LD A, (DE)								; Get the num low byte from the Text Saved Area
	LD L, A										; Save the value in the low byte reg 'L'
	INC DE										; Move PTR Text Saved Area UP
	LD A, (DE)								; Get the num high byte from the Text Saved Area
	LD H, A										; Save the value in the high byte reg 'H'
	INC DE										; Move PTR Text Saved Area UP
	LD C, 4										; Print 4 DIGIT in the LINE
	CALL PRTNUM
	LD A, SPACE								; Load the Space Symbol
	RST 0x10									; Print the Space char
	XOR A											; Reset reg 'A'
	CALL PRTSTR								; Print the rest of the string
	RET

;*************************************************************
;
; *** MVUP *** MVDOWN *** POPA *** & PUSHA ***
;
; 'MVUP' MOVES A BLOCK UP FROM WHERE DE-> TO WHERE BC-> UNTIL
; DE = HL
;
; 'MVDOWN' MOVES A BLOCK DOWN FROM WHERE DE-> TO WHERE HL->
; UNTIL DE = BC
;
; 'POPA' RESTORES THE 'FOR' LOOP VARIABLE SAVE AREA FROM THE
; STACK
;
; 'PUSHA' STACKS THE 'FOR' LOOP VARIABLE SAVE AREA INTO THE
; STACK
;*************************************************************
MVUP:
	RST 0x20								; Check if we reach the start of the Text Saved Area, (DE == HL)
	RET Z										; If so then Return
	LD A, (DE)							; Get char from the Line
	LD (BC), A							; Move it into next one
	INC DE									; Move PTR
	INC BC									; Move PTR
	JR MVUP									; Do it one more time

MVDOWN:
	;; Check if DE == BC
	LD A, B
	SUB D
	JR NZ, MD1						; If no then move data
	LD A, C
	SUB E
	RET Z									; If reg 'DE' equal to 'BC' then return

	;; Copy Data
MD1:
	DEC DE
	DEC HL
	LD A, (DE)
	LD (HL), A
	JR MVDOWN


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

	;; FIXME: For no it's fine
	LD (VECTOR_VAR), A
	OUT (VECTOR_PORT), A

	CALL UART_INIT
	LD D, 0x19
PARLOP:									; This loop just clear the screen
	CALL CRLF							; Clear the screen
	DEC D
	JR NZ, PARLOP
	XOR A									; Reset reg A VALUE
	LD DE, MSG1						; Print the boot message
	CALL PRTSTR						; Call function Print String
	XOR A									; Reset reg A VALUE
	LD DE, MSG2						; Print the boot message
	CALL PRTSTR						; Call function Print String
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

	LD A, UART_CR_OUT_MODE ; Set Mask of Tx Enable (Set Tx to Enable)
	OUT (UART_CR_PORT), A	; Write Command Reg

	;; Wait until char will be sended
	PUSH BC								; Save reg 'BC' in the Stack
	LD B, 0xFF
OUTC3:
	DEC B
	JR NZ, OUTC3
	POP BC								; Restore reg 'BC' value

	;; Close UART Tx Mode
	LD A, UART_CR							; Load basic configuration for UART
	OUT (UART_CR_PORT), A			; Write Command Reg

	;; Restore Interrupt Vector value
	LD A, (VECTOR_VAR)
	OUT (VECTOR_PORT), A

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

	;; Wait until char UART will change Rx_RDY flag
CI0:
	IN A, (UART_SR_PORT)			; Get Status Reg
	BIT UART_Rx_RDY, A				; Check If Uart is holding data
	JR NZ, CI0

	;; Restore Interrupt Vector value
	LD A, (VECTOR_VAR)
	OUT (VECTOR_PORT), A

	LD A, (UART_INPUT)		; Get UART Input var from UART_REG
	LD B, A								; Copy the char value into reg 'B'
	XOR LF								; Check if char is equal to LF (char '\n')
	JR Z, CI1							; If so then reset this value in func CI1

	XOR A									; Reset reg A value
	LD (UART_INPUT), A		; Reset UART Input var
	LD A, B								; Restore reg 'A' value


	CP CTRLO							; Check on CTRL-O
	JR NZ, CI2						; If not equal then end Input

	;; FIXME:	I have a physical Switches so ....
	; LD A, (OCSW)					; Get the SWITCH value
	; CPL										; Invert all bits of reg 'A'
	; LD (OCSW), A					; Save new value
	JR CHKIO


CI1:
	LD (UART_INPUT), A		; Reset UART Input var
	RET										; Flag reg 'F' have 'Z' SET

CI2:
	CP CTRLC							; Check on Control-C
	RET NZ								; If not equal then return
	CP CTRLD							; Check on Control-D
	RET NZ								; If not equal then return
	JP RSTART							; If its CTRL-C then Abord the line



MSG1:		db ESC, "[2J", ESC, "[H"					;SCREEN CLEAR
        db "TINY BASIC FOR Z80", CR				;BOOT MESSAGE
MSG2:		db "    VERSION 1.0   ", CR

;*************************************************************
;
; *** TABLES *** DIRECT *** & EXEC ***
;
; THIS SECTION OF THE CODE TESTS A STRING AGAINST A TABLE.
; WHEN A MATCH IS FOUND, CONTROL IS TRANSFERED TO THE SECTION
; OF CODE ACCORDING TO THE TABLE.
;
; AT 'EXEC', DE SHOULD POINT TO THE STRING AND HL SHOULD POINT
; TO THE TABLE-1.  AT 'DIRECT', DE SHOULD POINT TO THE STRING.
; HL WILL BE SET UP TO POINT TO TAB1-1, WHICH IS THE TABLE OF
; ALL DIRECT AND STATEMENT COMMANDS.
;
; A '.' IN THE STRING WILL TERMINATE THE TEST AND THE PARTIAL
; MATCH WILL BE CONSIDERED AS A MATCH.  E.G., 'P.', 'PR.',
; 'PRI.', 'PRIN.', OR 'PRINT' WILL ALL MATCH 'PRINT'.
;
; THE TABLE CONSISTS OF ANY NUMBER OF ITEMS.  EACH ITEM
; IS A STRING OF CHARACTERS WITH BIT 7 SET TO 0 AND
; A JUMP ADDRESS STORED HI-LOW WITH BIT 7 OF THE HIGH
; BYTE SET TO 1.
;
; END OF TABLE IS AN ITEM WITH A JUMP ADDRESS ONLY.  IF THE
; STRING DOES NOT MATCH ANY OF THE OTHER ITEMS, IT WILL
; MATCH THIS NULL ITEM AS DEFAULT.
;*************************************************************

TAB1:                                ; DIRECT COMMANDS
  db "LIST"
	dwa LIST
  db "RUN"
  dwa RUN
  db "NEW"
  dwa NEW

TAB2:																; STATEMENT COMMANDS
	db "LET"
	dwa LET
	db "GOTO"
	dwa GOTO
	db "HEXOUT"
	dwa HEXOUT
	db "PRINT"
	dwa PRINT
	dwa DEFLT

TAB4:																; FUNCTIONS
	db "RND"
	dwa RND
	db "ABS"
	dwa ABS
	db "SIZE"
	dwa SIZE
	dwa XP40

TAB8:																; RELATION OPERATORS
	db ">="
	dwa XP11
	db "#"
	dwa XP12
	db ">"
	dwa XP13
	db "<="
	dwa XP14
	db "="
	dwa XP15
	db "<"
	dwa XP16
	dwa XP17

DIRECT:
	LD HL, TAB1 - 1										; ADDR of the commands
EXEC:
	RST 0x28													; Ignore Space symbols
	PUSH DE														; Save Text Buffer PTR

EX1:
	LD A, (DE)												; GET char from Text Buffer
	INC DE
	INC HL														; Shift PTR of the Table
	CP (HL)														; Check with the first char
	JR Z, EX1													; If the chars are the same then get next
	LD A, 0x7F												; If not equal FIXME: ???
	DEC DE														; Return PTR to the prev state
	CP (HL)														; Check if bit 7 of table is set, which is jump ADDR
	JR C, EX5													; Flag 'C' - Yes -> (HL) > 0x7F

EX2:
	INC HL														; GET next value
	CP (HL)
	JR NC, EX2												; If 0x7F more or equal to the new CHAR
	INC HL														; Get to the next TABLE ITEM
	POP DE														; Restore Text Buffer PTR
	JR EXEC														; Check one more time

EX3:
	LD A, 0x7F												; Load value for find addr
EX4:
	INC HL														; Move PTR of the TABLE
	CP (HL)														; Check if we find ADDR
	JR NC, EX4												; If no then do it one more time
EX5:
	LD A, (HL)												; At the end load the high byte of the ADDR
	INC HL														; Move the TABLE PTR
	LD L, (HL)												; Get the low byte of the ADDR
	AND 0x7F													; Clear the flag, which indecate the start of the ADDR
	LD H, A														; Load high byte into reg 'H'
	POP AF														; Clean up the GARBAGE
	JP (HL)														; Run the command



; Variables name
STACK 						EQU 0x1FFF

;; Stack Variables
MMU								EQU 0x1000
VECTOR_VAR				EQU 0x1001
UART_INPUT:				EQU 0x1002
UART_OUTPUT:			EQU 0x1003

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
QUOT	            EQU 0x22             ; Symbol '"'
APOST	            EQU 0x27             ; Symbol "'"
COMMENT          	EQU 0x23             ; Symbol "#"
COMMA	          	EQU 0x2C             ; Symbol ","
SEMI            	EQU 0x3B             ; Symbol ";"
QMARK	            EQU 0x3F             ; Symbol "?"
TAB             	EQU 0x09             ; HORIZONTAL TAB
CTRLC           	EQU 0x03             ; Control "C"
CTRLD           	EQU 0x04             ; Control "C"
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
UPARR	          	EQU 0x5E             ; UP-Arrow



NUM:			; This array of values need for display HEX number
	db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E

; KEYBOARD: ;; Key sequence:																			 1 	 2		3	 4	 5 6 7	8		  9		  A	  B	C D E
;   db 0x00, "??????????????", 			0x00					     ;; 0x0X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
;   db 0x00, "????Q1???ZSAW2", 			0x00					     ;; 0x1X -> ALT SHIFT ? CTRL Q 1 ?  ?     ?     Z   S A W 2
;   db 0x00, "CXDE43?? VFTR5", 			0x00					     ;; 0x2X ->  C   X    D  E   4 3 ?  ?           V   F T R 5
;   db 0x00, "NBHGY6???MJU78", 			0x00					     ;; 0x3X ->  N   B    H  G   Y 6 ?  ?     ?     M   J U 7 8
;   db 0x00, ",KIO09??./L;P-", 			0x00					     ;; 0x4X ->  ,   K    I  O   0 9 ?  ?     .     /   L ; P -
;   db 0x00, "?'?[=????",10,"]?\?", 0x00				       ;; 0x5X ->  ?   '    ?  [   = ? ? CAPS SHIFT ENTER ] ? \ ?
;   db 0x00, "??????????????", 			0x00					     ;; 0x6X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?
;   db 0x00, "??????????????", 			0x00					     ;; 0x7X ->  ?   ?    ?  ?   ? ? ?  ?     ?     ?   ? ? ? ?

	ORG 0x1000
VARBGN: DS 55											;; Variables
BUFFER: DS 64											;; Input Buffer
BUFEND: DS 1											;; Buffer End
	END