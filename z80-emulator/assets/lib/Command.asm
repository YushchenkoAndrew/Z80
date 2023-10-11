#include "Utils.asm"

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
  LD A, (HL)  ; Get cmd high byte addr
  INC HL      ; Move ptr to cmd low byte addr
  LD L, (HL)  ; Get cmd low byte addr
  LD H, A     ; Complete in reg HL cmd addr hanlder
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
  LD A, (PTR_TEXT_BUFF_END); Get the end of the buf offset
  INC A       ; Move start ptr to the next char, aka skip "\n"
  LD (PTR_TEXT_BUFF_BGN), A; Save the end buffer offset as the next start
  EXX         ; Restore reg from alt regs
  RET


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
;;  CALL #CMD_CLEAR
;; 
;; number:
;;  db "mem 10..12"
;;
;; proc CMD_CLEAR() -> void;
;;   reg A  -- as defined
;;   reg B  -- unaeffected
;;   reg DE -- as defined
;;   reg HL -- unaffected
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
  LD A, (DE)  ; Load the next char from buf
  CP LINE_FEED; Check if only requisted mem block
  JR NZ, #CMD_MEM_range-$
  LD HL, (PTR_TEMP_WORD); Load parsed number as a ptr
  CALL #HEX_ASCII; Display char

  LD A, LINE_FEED; Go to then next line
  RST 0x10    ; Print the char
  JP #MSG_OK

#CMD_MEM_range:
  CP "."      ; Check if requisted mem range
  JR NZ, #CMD_MEM_input-$
  INC DE      ; Move buf ptr by one
  LD A, (DE)  ; Load the next char from buf
  CP "."      ; Check if next char is also "."
  JP NZ, #MSG_ARG_ERR; If not show the err

  INC DE      ; Move buf ptr by one
  LD A, (DE)  ; Load the next char from buf
  CALL #IS_HEX; Check if the value is hex
  JP Z, #MSG_NUM_ERR; If char wasn't change that meen that its not correct
  LD HL, (PTR_TEMP_WORD); Load parsed number as start addr
  PUSH HL     ; Save ptr start in stack
  LD HL, PTR_TEMP_WORD; Load ptr where result be saved
  CALL #STR_HEX
  LD HL, (PTR_TEMP_WORD); Load parsed number as a end addr
  EX DE, HL   ; Load in reg DE end of ptr addr
  EX (SP), HL ; Save in stack buf ptr and load in reg HL ptr start

#CMD_MEM_range_lp:
  CALL #HEX_ASCII; Display char
  PUSH HL     ; Temp save reg HL
  SBC HL, DE  ; Check if start addr reach the end addr
  POP HL      ; Restore reg HL, aka ignore subtraction
  JR Z, #CMD_MEM_range_end-$
  INC HL      ; Increment start addr by one
  LD A, " "   ; Add separation by space between values
  RST 0x10    ; Print the char
  JR #CMD_MEM_range_lp-$

#CMD_MEM_range_end:
  POP DE      ; Restore buf ptr from stack
  LD A, LINE_FEED; Go to then next line
  RST 0x10    ; Print the char
  JP #MSG_OK



#CMD_MEM_input:
  ; TODO: ... add ability to write to memory by one & display range
  JP #MSG_OK

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
  CALL #NEW_DIR_BLK

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
  LD C, A     ; Load counter to reg C
  LD B, 0     ; Reset reg B

  PUSH BC     ; Save buf char counter in stack, reg B
  LD BC, FS_MODE_DIR | FS_MODE_USR_R | FS_MODE_USR_W; Load mount mode
  CALL #NEW_INODE
  
  POP BC      ; Restore buf char counter
  CALL #NEW_DIR_BLK
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
#CMD_LS:
  LD C, A     ; Load counter to reg C
  LD B, 0     ; Reset reg B

  LD BC, FS_SZ_INODE; Load byte offset of inode
  LD DE, INODE_MAP; Load addr to first inode map
  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes
  EX DE, HL   ; Load to reg DE inodes count, and in reg HL inode start addr
  PUSH HL

  ; LD IX, SUPER_BLOCK_MAP; Load ptr to start of super block
  POP IX

  INC DE      ; Inc DE by, need to check if reg is 0
#CMD_LS_offset:
  XOR A       ; Reset Acc
  DEC DE      ; Decrement counter
  CP D        ; Check if reg D is empty
  JR NZ, #CMD_LS_offset_nxt-$
  CP E        ; Check if reg E is empty
  JR Z, #CMD_LS_offset_esc-$
#CMD_LS_offset_nxt:
  ; LD A, 0x40
  ; AND (IX+FS_INODE_MODE+1); Check if node is a dir
  ; JR NZ, #NEW_INODE_offset_end-$

  ; EXX
  ; LD C, (IX+FS_INODE_SIZE); Get low byte of inodes size
  ; LD B, (IX+FS_INODE_SIZE+1); Get high byte of inodes size

  ; TODO: Check IX MODE on file type and etc ...

  LD L, (IX+FS_INODE_ZONE0); Set directory data zone0, low byte
  LD H, (IX+FS_INODE_ZONE0+1); Set directory data zone0, high byte
  ; PUSH HL

  INC HL
#CMD_LS_lp:
  INC HL
  LD A, (HL)
  OR A
  JR Z, #CMD_LS_lp_esc-$
  RST 0x10
  JR #CMD_LS_lp-$

#CMD_LS_lp_esc:
  LD A, LINE_FEED
  RST 0x10

  ADD IX, BC  ; Find next inode addr of zone 0
  JR #CMD_LS_offset-$

#CMD_LS_offset_esc:
  JP #MSG_OK


.COMMANDS_VEC_ST:
  db "echo", 0, #CMD_ECHO
  db "clear", 0, #CMD_CLEAR
  db "mount", 0, #CMD_MOUNT
  db "umount", 0, #CMD_UMOUNT
  db "mkdir", 0, #CMD_MKDIR
  db "ls", 0, #CMD_LS
  db "mem", 0, #CMD_MEM
.COMMANDS_VEC_ED:

.MOUNT_NAME_ST:
  db "mnt"
.MOUNT_NAME_ED:

#include "Message.asm"

;; Variables
COMMANDS_SIZE        EQU  7

#include "../lib/FileSystem.asm"