#include "../lib/Utils.asm"

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

  LD HL, MSG_COMMAND_NOT_FOUND
  RST 0x18    ; Print string
  CALL #CMD_EXEC_BUF_CALC; Sets value in reg B - amount of chars & reg HL buf start pos
  EX DE, HL   ; Load to reg DE start buf pos
  CALL #CMD_ECHO_lp; Display command
  LD A, "'"    ; Load Signle quote to reg A to display
  RST 0x10     ; Dispay single quote

_CMD_EXEC_esc:
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
  XOR A       ; Reset Acc
  PUSH DE     ; Save ptr to buf in stack
  LD DE, INODE_MAP+FS_INODE_ZONE0; Load addr to first inode map zone0
  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes
  ADD HL, DE  ; Find next inode addr of zone 0
  PUSH HL     ; Temp save calculated addr 
  POP IX      ; Load this addr in reg IX
  POP DE      ; Restore ptr to buf
  LD (HL), A  ; Reset low byte of the addr

#CMD_MOUNT_bgn:
  LD A, (HL)  ; Get low byte of the addr
  LD (IX+1), A; Shift low byte -> high byte
  XOR A       ; Reset inner loop counter, aka bit offset counter

#CMD_MOUNT_lp:
  PUSH AF     ; Save bit offset cnt
  LD A, (DE)  ; Get buf char
  LD C, A     ; Save curr char in reg C
  CALL #ASCII_HEX; Convert Acc to hex if char is valid
  CP C        ; Check if char has changed
  JP Z, #MSG_NUM_ERR
  RLD         ; Save result memory
  INC DE      ; Move buf ptr to the next char
  POP AF      ; Get bit offset count
  OR A        ; Check if loop CMD_MOUNT_lp happend once
  JR NZ, #CMD_MOUNT_esc-$ ; If so then reset 
  INC A       ; Increment counter
  DJNZ #CMD_MOUNT_lp-$
  INC HL      ; Get ptr to high byte addr
  XOR A       ; Reset Acc
  RRD         ; Shift value pointed by reg HL by 4, uses when there are 3 digit
  JR #CMD_MOUNT_init-$

#CMD_MOUNT_esc:
  DJNZ  #CMD_MOUNT_bgn-$

#CMD_MOUNT_init:
  CALL _NEW_INODE; Get curr inode addr in reg HL & IX

  LD (IX+FS_INODE_MODE),  FS_MODE_USR_W; Set inode mode low byte
  LD (IX+FS_INODE_MODE+1), FS_MODE_DIR | FS_MODE_USR_R; Set inode mode high byte

  PUSH IX     ; Temp save addr of curr inode
  CALL _NEW_DATA_BLOCK
  POP IX      ; Restore ptr to inode

  LD (IX+FS_INODE_ZONE0),   L; Set directory data zone0, low byte
  LD (IX+FS_INODE_ZONE0+1), H; Set directory data zone0, high byte
  PUSH HL     ; Temp save calculated addr 
  POP IX      ; Load this addr in reg IX

  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes

  LD (IX+FS_DIR_INODE),   L; Set directory inode, low byte
  LD (IX+FS_DIR_INODE+1), H; Set directory inode, high byte

  ; TODO: Change this to filename
  LD (IX+FS_DIR_FILENAME),   "F"; Set directory inode, low byte

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
  LD B, A     ; Load counter to reg B
  LD A, (SUPER_BLOCK_MAP+FS_SP_BLK_STATE); Get the mount state
  JP Z, #MSG_ARG_ERR
  XOR A       ; Reset Acc

  CALL _NEW_INODE; Get curr inode addr in reg HL & IX
  LD (IX+FS_INODE_MODE),  FS_MODE_USR_W; Set inode mode low byte
  LD (IX+FS_INODE_MODE+1), FS_MODE_DIR | FS_MODE_USR_R; Set inode mode high byte

  PUSH IX     ; Temp save addr of curr inode
  CALL _NEW_DATA_BLOCK
  POP IX      ; Restore ptr to inode

  LD (IX+FS_INODE_ZONE0),   L; Set directory data zone0, low byte
  LD (IX+FS_INODE_ZONE0+1), H; Set directory data zone0, high byte
  PUSH HL     ; Temp save calculated addr 
  POP IX      ; Load this addr in reg IX
  PUSH HL     ; Temp save calculated addr 

  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes

  LD (IX+FS_DIR_INODE),   L; Set directory inode, low byte
  LD (IX+FS_DIR_INODE+1), H; Set directory inode, high byte

  ; TODO:
  LD (IX+FS_DIR_FILENAME),   "F"; Set directory inode, low byte
  ; LD C, B
  ; LD B, 0
  ; LD BC, 0x05
  ; POP HL
  ; INC HL
  ; INC HL

  ; LDIR        ; Copy file name

  ; TODO:
  JP #MSG_OK


.COMMANDS_VEC_ST:
  db "echo", 0, #CMD_ECHO
  db "mount", 0, #CMD_MOUNT
  db "umount", 0, #CMD_UMOUNT
  db "mkdir", 0, #CMD_MKDIR
.COMMANDS_VEC_ED:

#include "Message.asm"

;; Variables
COMMANDS_SIZE        EQU  4

#include "../lib/FileSystem.asm"