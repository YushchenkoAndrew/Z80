#include "Defs.asm"

;;
;; Example:
;;  CALL #CMD_EXEC_BUF_CALC
;;
;; proc CMD_EXEC_BUF_CALC() -> reg B & reb HL;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- as defined
;;
#CMD_EXEC_BUF_CALC:
  LD HL, PTR_TEXT_BUFF_BGN; Get the start of the buf offset
  LD A, (PTR_TEXT_BUFF_END); Get the end of the buf offset
  DEC A       ; FIXME: Bug that first char is ignored
  SUB (HL)    ; Get line size, end offset - start offset
  LD B, A     ; Set reg B as a counter
  LD A, (HL)  ; Get the value of start buf offset
  LD HL, TEXT_BUF_MAP; Load ptr to the start of the buf
  LD L, A     ; Offset text buf map by start offset
  RET

;;
;; Example:
;;  CALL _CMD_EXEC
;;
;; func _CMD_EXEC() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
;;   reg BC'-- as defined
;;   reg DE'-- as defined
;;   reg HL'-- as defined
_CMD_EXEC:
  EXX         ; Save reg in alt regs
  EX AF, AF'  ; Save reg A & flags in alt regs
  LD DE, .COMMANDS_VEC_ST; Load ptr to cmd vector
  LD B, COMMANDS_SIZE; Load amount of avaliable commands

_CMD_EXEC_bgn:
  PUSH BC     ; Save commands counter in Stack
  CALL #CMD_EXEC_BUF_CALC; Sets value in reg B - amount of chars & reg HL buf start pos

_CMD_EXEC_lp:
  LD A, (DE)  ; Load char from command
  OR A        ; Check if this char is 0, aka end of the word
  JR Z, _CMD_EXEC_cmp-$
  CP (HL)     ; Check if chars are equal
  JR NZ, _CMD_EXEC_nxt-$
  INC HL      ; Move buf ptr by one
  INC DE      ; Move cmd ptr by one
  DJNZ _CMD_EXEC_lp-$
  LD A, (DE)  ; Load next char from command, (which was skiped)
  OR A        ; Check if this char is 0, aka end of the word
  JR NZ, _CMD_EXEC_nxt-$; If buf counter is ended, aka buf word is less then cmd
  INC B       ; If cmd was found, then set reg B to 1, it will be reset after

_CMD_EXEC_cmp:
  LD A, (HL)  ; Get buf char
  CP LINE_FEED; Check if buf char is "\n", aka str compair is valid
  JR Z, _CMD_EXEC_cmp_bgn-$
  CP " "      ; Check if cmd has multiple args
  JR NZ, _CMD_EXEC_nxt-$

_CMD_EXEC_cmp_bgn:
  INC HL      ; Skip the next char, usefull when char is " "
  DEC B       ; Update counter of remaining chars
  EX DE, HL   ; Load cmd string ptr to reg HL
  INC HL      ; Move ptr to cmd high byte addr
  RST 0x20    ;  Complete in reg HL cmd addr hanlder
  LD A, B     ; Save remained buf char counter
  POP BC      ; Restore commands coutner, now is usless
  LD BC, _CMD_EXEC_esc; Load custom return addr
  PUSH BC     ; Manually create CALL, aka save return addr
  JP (HL)     ; Jump to the cmd execution
  
_CMD_EXEC_nxt:
  LD A, (DE)  ; Get current cmd char
  OR A        ; Check if the word is ended
  INC DE      ; Move cmd ptr to the next char
  JR NZ, _CMD_EXEC_nxt-$

  INC DE      ; Skip cmd high byte addr
  INC DE      ; Skip cmd low byte addr
  POP BC      ; Restore commands coutner
  DJNZ _CMD_EXEC_bgn-$

  CALL #CMD_EXEC_BUF_CALC; Sets value in reg B - amount of chars & reg HL buf start pos
  LD A, B     ; Load counter to Acc
  OR A        ; Check of counter is empty
  JR Z, _CMD_EXEC_esc-$; If counter is empty just end execution

  PUSH HL     ; Temporary save reg HL, ptr to the buf
  LD HL, MSG_COMMAND_NOT_FOUND
  RST 0x18    ; Print string
  POP HL      ; Restore reg HL
  EX DE, HL   ; Load to reg DE start buf pos
  CALL #CMD_ECHO_lp; Display command
  LD A, "'"   ; Print the close single quoute
  RST 0x10    ; Print string

_CMD_EXEC_esc:
  LD A, LINE_FEED; Go to the next line when cmd is finished
  RST 0x10    ; Output the char
  LD A, (PTR_INPUT_STATE); Print ">" | "|" which means that curr state is cmd mode or input redirected
  RST 0x10    ; Output the char
  LD A, (PTR_TEXT_BUFF_END); Get the end of the buf offset
  INC A       ; Move start ptr to the next char, aka skip "\n"
  LD (PTR_TEXT_BUFF_BGN), A; Save the end buffer offset as the next start
  EXX         ; Restore reg from alt regs
  EX AF, AF'  ; Restore reg A & flags
  RET


;;
;; Example:
;;  CALL _CMD_EXEC_ESC
;;
;; func _CMD_EXEC_ESC() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
;;   reg BC'-- as defined
;;   reg DE'-- as defined
;;   reg HL'-- as defined
_CMD_EXEC_ESC:
  EXX         ; Save reg in alt regs
  EX AF, AF'  ; Restore reg A & flags
  JR _CMD_EXEC_esc-$


;;
;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_ECHO
;; 
;; number:
;;  db "echo test"
;;
;; proc CMD_ECHO() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_ECHO:
  OR A        ; Check if remaining chars exist
  RET Z       ; If not then just return
  LD B, A     ; Load counter to reg B
#CMD_ECHO_lp:
  LD A, (DE)  ; Get buf char
  RST 0x10    ; Output the char
  INC DE      ; Move buf ptr to the next char
  DJNZ #CMD_ECHO_lp-$
  RET         ; Return to the addr of _CMD_EXEC_esc 

;;
;; Example:
;;  LD DE, number+5
;;  CALL #CMD_HISTORY
;; 
;; number:
;;  db "history"
;;
;; proc CMD_HISTORY() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- as defined
;;
#CMD_HISTORY:
  PUSH DE     ; Buf ptr in stack
  LD A, (PTR_TEXT_BUFF_END); Get the end of the buf offset
  LD DE, TEXT_BUF_MAP;Load addr of text buf start
  CALL #CMD_ECHO; Print the history
  POP DE      ; Restore buf ptr
  LD A, LINE_FEED; Go to then next line
  RST 0x10    ; Print the char
  JP #MSG_OK

;;
;; Example:
;;  LD DE, number+5
;;  CALL #CMD_CLEAR
;; 
;; number:
;;  db "clear"
;;
;; proc CMD_CLEAR() -> void;
;;   reg A  -- as defined
;;   reg B  -- unaeffected
;;   reg DE -- unaeffected
;;   reg HL -- unaffected
;;
#CMD_CLEAR:
  LD A, FORM_FEED; Load to Acc char that correspondes to Clear display
  RST 0x10    ; Output the char
  RET         ; Return to the addr of _CMD_EXEC_esc 

;;
;; Example:
;;  LD DE, number+5
;;  CALL #CMD_MEM
;; 
;; number:
;;  db "mem 10.12"
;;
;; proc CMD_MEM() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- as defined
;;
#CMD_MEM:
  OR A        ; Check if remaining chars exist
  JP Z, #MSG_ARG_ERR
  LD B, A     ; Load counter to reg B
  LD A, (DE)  ; Get buf char
  CALL #IS_HEX; Check if the value is hex
  JP Z, #MSG_NUM_ERR; If char wasn't change that meen that its not correct

  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  CALL #STR_HEX
  LD HL, (PTR_TEMP_WORD); Load parsed number as start addr
  LD A, (DE)  ; Load the next char from buf

  CP LINE_FEED; Check if only requisted mem block
  JR NZ, #CMD_MEM_range-$
  CALL #HEX_ASCII; Display char
  JR #CMD_MEM_end-$

#CMD_MEM_range:
  CP "."      ; Check if requisted mem range
  JR NZ, #CMD_MEM_input-$

  INC DE      ; Move buf ptr by one
  LD A, (DE)  ; Load the next char from buf
  CALL #IS_HEX; Check if the value is hex
  JP Z, #MSG_NUM_ERR; If char wasn't change that meen that its not correct
  PUSH HL     ; Save ptr start in stack
  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  CALL #STR_HEX; Convert buf string to number
  LD HL, (PTR_TEMP_WORD); Load parsed number as a end addr
  EX DE, HL   ; Load in reg DE end of ptr addr
  EX (SP), HL ; Save in stack buf ptr and load in reg HL ptr start

#CMD_MEM_range_lp:
  CALL #HEX_ASCII; Display char
  PUSH HL     ; Temp save reg HL
  XOR A       ; Reset flag C
  SBC HL, DE  ; Check if start addr reach the end addr
  POP HL      ; Restore reg HL, aka ignore subtraction
  JR Z, #CMD_MEM_range_end-$
  INC HL      ; Increment start addr by one
  LD A, " "   ; Add separation by space between values
  RST 0x10    ; Print the char
  JR #CMD_MEM_range_lp-$

#CMD_MEM_range_end:
  POP DE      ; Restore buf ptr from stack
  JR #CMD_MEM_end-$

#CMD_MEM_input:
  CP " "      ; Check if requisted mem is input
  JP NZ, #MSG_ARG_ERR

#CMD_MEM_input_lp:
  PUSH HL     ; Save ptr start in stack
  LD A, (DE)  ; Load the next char from buf
  CP " "      ; Check if another number exist
  JR NZ, #CMD_MEM_input_end-$
  INC DE      ; Move buf ptr by one
  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  CALL #STR_HEX; Convert buf string to number
  POP HL      ; Restore reg HL, aka start ptr
  CALL #HEX_ASCII; Display prev value
  LD A, ARROW_RIGHT; Load char to show next state
  RST 0x10    ; Display char
  LD A, (PTR_TEMP_WORD); Get low byte of the converted number
  LD (HL), A  ; Rewrite memory value with requested one
  CALL #HEX_ASCII; Display curr value
  LD A, " "   ; Load char to show next state
  RST 0x10    ; Display char
  INC HL      ; Move start ptr by one
  JR #CMD_MEM_input_lp-$

#CMD_MEM_input_end:
  POP HL      ; Restore start ptr, now is usless

#CMD_MEM_end:
  LD A, LINE_FEED; Go to then next line
  RST 0x10    ; Print the char
  JP #MSG_OK

;;
;; Example:
;;  LD DE, number+5
;;  CALL #CMD_DEV
;; 
;; number:
;;  db "dev 0 FF"
;;
;; proc CMD_DEV() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- as defined
;;
#CMD_DEV:
  OR A        ; Check if remaining chars exist
  JP Z, #MSG_ARG_ERR
  LD B, A     ; Load counter to reg B
  LD A, (DE)  ; Get buf char
  CALL #IS_HEX; Check if the value is hex
  JP Z, #MSG_NUM_ERR; If char wasn't change that meen that its not correct

  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  CALL #STR_HEX
  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  LD A, (DE)  ; Load the next char from buf

  CP LINE_FEED; Check if only requisted mem block
  JR NZ, #CMD_DEV_input-$
  LD C, (HL)  ; Load low byte from the parsed word
  INC HL      ; Move ptr to the high byte
  LD B, (HL)  ; Load high byte from the parsed word
  IN A, (C)   ; Load value from the device
  LD (HL), A  ; Overwrite high byte with input val
  CALL #HEX_ASCII; Display char
  JR #CMD_DEV_end-$

#CMD_DEV_input:
  CP " "      ; Check if requisted mem is input
  JP NZ, #MSG_ARG_ERR
  LD HL, (PTR_TEMP_WORD); Get device addr

#CMD_DEV_input_lp:
  PUSH HL     ; Save device addr in stack
  LD A, (DE)  ; Load the next char from buf
  CP " "      ; Check if another number exist
  JR NZ, #CMD_DEV_input_end-$
  INC DE      ; Move buf ptr by one
  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  CALL #STR_HEX; Convert buf string to number
  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  CALL #HEX_ASCII; Display low byte value
  LD A, " "   ; Load char to show next state
  RST 0x10    ; Display char
  POP HL      ; Restore reg HL, aka device addr
  LD C, L     ; Load low byte from the parsed word
  LD B, H     ; Load high byte from the parsed word
  LD A, (PTR_TEMP_WORD); Get low byte of the converted number
  OUT (C), A  ; Output value to the device
  JR #CMD_DEV_input_lp-$

#CMD_DEV_input_end:
  POP HL      ; Restore device addr, now is usless

#CMD_DEV_end:
  LD A, LINE_FEED; Go to then next line
  RST 0x10    ; Print the char
  JP #MSG_OK



;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_HELP
;; 
;; number:
;;  db "help"
;;
;; proc CMD_HELP() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_HELP:
  LD HL, .COMMANDS_VEC_ST; Load ptr to the start of command
  LD B, COMMANDS_PRNT_SIZE; Load command size

#CMD_HELP_lp:
  LD A, (HL)  ; Get curr cmd char
  INC HL      ; Move char ptr by one
  OR A        ; Check if char is \0
  JR Z, #CMD_HELP_lp_end-$
  RST 0x10    ; Print the char
  JR #CMD_HELP_lp-$

#CMD_HELP_lp_end:
  LD A, " "   ; Load to Acc ' '
  RST 0x10    ; Print the char
  INC HL      ; Skip cmd high byte addr
  INC HL      ; Skip cmd low byte addr
  DJNZ #CMD_HELP_lp-$
  RET


;;
;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_MOUNT
;; 
;; number:
;;  db "mount 55h"
;;
;; proc CMD_MOUNT() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_MOUNT:
  OR A        ; Check if remaining chars exist
  JP Z, #MSG_ARG_ERR
  LD B, A     ; Load counter to reg B
  PUSH DE     ; Save ptr to buf in stack
  ;; TODO: Change this to, because this logic is incorrect
  ;; TODO: Need to save requested addr in data block
  LD DE, INODE_MAP+FS_INODE_ZONE0; Load addr to first inode map zone0
  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes
  ADD HL, DE  ; Find next inode addr of zone 0
  PUSH HL     ; Temp save calculated addr 
  POP IX      ; Load this addr in reg IX
  POP DE      ; Restore ptr to buf
  CALL #STR_HEX
  JP Z, #MSG_NUM_ERR; If flag Z is set, show error

  LD BC, FS_MODE_DIR | FS_MODE_USR_R | FS_MODE_USR_W; Load mount mode
  CALL #NEW_INODE
  
  LD BC, .MOUNT_NAME_ED-.MOUNT_NAME_ST; Load 
  LD DE, .MOUNT_NAME_ST
  CALL #NEW_FILENAME_BLK

  JP #MSG_OK


;;
;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_UMOUNT
;; 
;; number:
;;  db "umount"
;;
;; proc CMD_UMOUNT() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_UMOUNT:
  ; TODO:
  JP #MSG_OK

;;
;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_MKDIR
;; 
;; number:
;;  db "mkdir test"
;;
;; proc CMD_MKDIR() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_MKDIR:
  OR A        ; Check if buf conter is not empty
  JP Z, #MSG_ARG_ERR
  LD B, A     ; Load counter to reg B
  CALL #IS_FILENAME_EXIST; Check if such filename already exist
  PUSH HL     ; Temp save inode addr in Stack
  POP IX      ; Load inode addr into reg IX
  JP NZ, #MSG_OK  ; If so then just end the handler

  LD C, B     ; Load counter to reg C
  LD B, 0     ; Reset reg B
  PUSH BC     ; Save buf char counter in stack, reg B

  LD BC, FS_MODE_DIR | FS_MODE_USR_R | FS_MODE_USR_W; Load mount mode
  CALL #NEW_INODE
  
  POP BC      ; Restore buf char counter
  CALL #NEW_FILENAME_BLK
  JP #MSG_OK

;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_TOUCH
;; 
;; number:
;;  db "touch test"
;;
;; proc CMD_TOUCH() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_TOUCH:
  OR A        ; Check if buf conter is not empty
  JP Z, #MSG_ARG_ERR
  LD B, A     ; Load counter to reg B
  CALL #IS_FILENAME_EXIST; Check if such filename already exist
  PUSH HL     ; Temp save inode addr in Stack
  POP IX      ; Load inode addr into reg IX
  JP NZ, #MSG_OK  ; If so then just end the handler

  LD C, B     ; Move counter to the low byte of reg BC
  LD B, 0     ; Reset reg B
  PUSH BC     ; Save buf char counter in stack, reg B

  LD BC, FS_MODE_FILE | FS_MODE_USR_R | FS_MODE_USR_W; Load mount mode
  CALL #NEW_INODE
  
  POP BC      ; Restore buf char counter
  CALL #NEW_FILENAME_BLK
  JP #MSG_OK

;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_TOUCH
;; 
;; number:
;;  db "cat test"
;;
;; proc CMD_CAT() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_CAT:
  OR A        ; Check if buf conter is not empty
  JP Z, #MSG_ARG_ERR
  LD B, A     ; Load counter to reg B
  CALL #IS_FILENAME_EXIST; Check if such filename already exist
  PUSH HL     ; Temp save inode addr in Stack
  POP IX      ; Load inode addr into reg IX
  JP Z, #MSG_ARG_ERR; If file not exist dipslay the error

  LD HL, #CMD_CAT_iter; Save callback func
  CALL #ITER_FILE_BLK; Iterate data block
  JP NZ, #MSG_FILE_TYPE_ERR; If type is not file show an error

  LD A, LINE_FEED; Go to the next line
  RST 0x10    ; Display the char
  JP #MSG_OK

#CMD_CAT_iter:
  LD A, (DE)  ; Load in Acc char from data block
  RST 0x10    ; Display char
  RET


;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_TOUCH
;; 
;; number:
;;  db "touch test"
;;
;; proc CMD_TOUCH() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_WR:
  CALL #CMD_TOUCH; Create file
  JR NZ, #CMD_WR_bg-$; If file already exist then skip initialization
  CALL #NEXT_DATA_BLK; Calculate next data block addr
  LD (IX+FS_INODE_ZONE1),   L; Set data zone 1 addr, low byte
  LD (IX+FS_INODE_ZONE1+1), H; Set data zone 1 addr, high byte

  LD HL, FS_SZ_DATA_BLK; Preallocate default file size
  LD (IX+FS_INODE_ALLOCATED),   L; Set data zone 1 addr, low byte
  LD (IX+FS_INODE_ALLOCATED+1), H; Set data zone 1 addr, high byte

#CMD_WR_bg:
  LD (PTR_INPUT_FILE), IX; Save inode of file
  LD HL, #CMD_WR_input; Load input call function
  LD (PTR_INPUT_FUNC), HL; Save handler func in memory
  LD A, "|"   ; Set new state
  LD (PTR_INPUT_STATE), A; Save new buf command state
  RET

#CMD_WR_input:
  PUSH HL     ; Save reg HL in stack
  LD HL, TEXT_BUF_MAP; Get ptr to allocated text arae
  LD A, (PTR_TEXT_BUFF_END); Get buf-ptr offset
  LD L, A     ; Move text ptr to curr char
  LD A, (HL)  ; Load curr char

  CP END_OF_TXT; Check if buf redirect is ended
  JR Z, #CMD_WR_input_esc-$

  ; TODO: Add ability to allocate other zones
  PUSH HL     ; Save text ptr in stack
  PUSH DE     ; Temp save reg DE in stack
  LD IX, (PTR_INPUT_FILE); Load file inode into reg IX
  LD E, (IX+FS_INODE_SIZE)  ; Get curr inode size, low byte
  LD D, (IX+FS_INODE_SIZE+1); Get curr inode size, high byte
  LD L, (IX+FS_INODE_ZONE1)  ; Get data zone 1 addr, low byte
  LD H, (IX+FS_INODE_ZONE1+1); Get data zone 1 addr, high byte
  ADD HL, DE  ; Calculate next char position, with filename blk
  INC DE      ; Increase file size by one
  LD (IX+FS_INODE_SIZE),   E; Save calculated size in inode, low byte
  LD (IX+FS_INODE_SIZE+1), D; Save calculated size in inode, high byte
  LD DE, FS_SZ_FILENAME_BLK; Get filename block size
  SBC HL, DE  ; Calcuate next char position without filename block
  LD (HL), A  ; Save curr char in to file buffer
  POP DE      ; Restore reg DE from stack
  POP HL      ; Restore text ptr from stack

  CP LINE_FEED; Check if curr char is a new line aka '\n'
  JR NZ, #CMD_WR_input_end-$
  LD A, (PTR_INPUT_STATE); Print ">" | "|" which means that curr state is cmd mode or input redirected
  RST 0x10    ; Output the char
#CMD_WR_input_end:
  POP HL      ; Restore reg HL
  RET

#CMD_WR_input_esc:
  LD HL, 0    ; Reset reg HL
  LD (PTR_INPUT_FUNC), HL; Reset input handler
  LD A, ">"   ; Set new state, cmd
  LD (PTR_INPUT_STATE), A; Save new buf command state

  POP HL      ; Restore reg HL
  EX AF, AF'  ; Temp swap reg A & flags, need for _CMD_EXEC_esc
  EXX         ; Temp swap reg with alt
  JP _CMD_EXEC_esc; Mark the end of buf redirection

;;
;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_MKDIR
;; 
;; number:
;;  db "mkdir test"
;;
;; proc CMD_MKDIR() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_LS:
  LD HL, #CMD_LS_iter; Load handler func for iter to call
  CALL #ITER_INODE; Iterate throgh inodes
  LD A, LINE_FEED; Go to the next line
  RST 0x10    ; Display the char
  JP #MSG_OK

#CMD_LS_iter:
  PUSH HL     ; Save reg HL in stack
  LD L, (IX+FS_INODE_ZONE0); Set directory data zone0, low byte
  LD H, (IX+FS_INODE_ZONE0+1); Set directory data zone0, high byte
  INC HL      ; Move ptr to the byte before of file name start
  INC HL      ; Move ptr to the start of the name
  RST 0x18    ; Print filaname
  LD A, " "   ; Add space separator between files
  RST 0x10    ; Display the char
  POP HL      ; Restore reg HL
  RET


;;
;; Example:
;;  LD B, 0
;;  LD DE, number+5
;;  CALL #CMD_MKDIR
;; 
;; number:
;;  db "mkdir test"
;;
;; proc CMD_MKDIR() -> void;
;;   reg A  -- as defined
;;   reg B  -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#CMD_LL:
  LD HL, #CMD_LL_iter; Load handler func for iter to call
  CALL #ITER_INODE; Iterate throgh inodes
  JP #MSG_OK

#CMD_LL_iter:
  PUSH HL     ; Save reg HL in stack
  CALL #PRINT_INODE; Print info about curr inode
  LD A, LINE_FEED; Go to the next line
  RST 0x10    ; Display the char
  POP HL      ; Restore reg HL
  RET


; TODO: Treat each command as different task
.COMMANDS_VEC_ST:
  db "echo", 0, #CMD_ECHO
  db "clear", 0, #CMD_CLEAR
  db "mount", 0, #CMD_MOUNT
  db "umount", 0, #CMD_UMOUNT
  db "mkdir", 0, #CMD_MKDIR
  db "touch", 0, #CMD_TOUCH
  db "cat", 0, #CMD_CAT
  db "wr", 0, #CMD_WR
  db "ll", 0, #CMD_LL
  db "ls", 0, #CMD_LS
  db "mem", 0, #CMD_MEM
  db "dev", 0, #CMD_DEV
  db "help", 0, #CMD_HELP
  db "history", 0, #CMD_HISTORY

  db FORM_FEED, 0, #CMD_CLEAR
.COMMANDS_VEC_ED:

.MOUNT_NAME_ST:
  db "mnt"
.MOUNT_NAME_ED:

#include "Message.asm"

;; Variables
COMMANDS_SIZE        EQU  15
COMMANDS_PRNT_SIZE   EQU  COMMANDS_SIZE - 1

#include "../lib/FileSystem.asm"