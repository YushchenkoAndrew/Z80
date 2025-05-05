; #include "Utils.asm"

;; NOTE: Inspired by this article
;; http://ohm.hgesser.de/sp-ss2012/Intro-MinixFS.pdf


;;
;; Example:
;;  LD B, 5   ; String length
;;  LD DE, .STRING  ; Load ptr to the string in reg DE
;;  CALL #FIND_FILE; Will return reg IX which will point to the found INODE or 0
;; 
;; #TEMP:
;;  RET
;; 
;; proc FIND_FILE() -> reg IX;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;   reg IX -- as defined
;;
#FIND_FILE:
  PUSH AF     ; Save reg AF in stack
  PUSH HL     ; Save reg HL in stack

  PUSH DE     ; Temp save string pointer in reg HL
  LD HL, (PTR_DIR_INODE); Get current dirrectory inode ptr
  LD A, FS_SZ_INODE; Load inode size
  CALL #MUL16x8; Calc inode address offset
  PUSH HL     ; Temp save inode addr offset in stack
  POP IX      ; Get inode addr offset in reg IX
  LD DE, INODE_MAP; Load inode addr start
  ADD IX, DE  ; Find current directory inode addr

  LD A, FS_SZ_ZONE_BLK; Load zone size
  LD L, (IX+FS_INODE_SIZE); Set current directory inode, low byte
  LD H, (IX+FS_INODE_SIZE+1); Set current directory inode, high byte
  CALL #DIV16x8; Calc index offset to the last zone
  LD A, 2     ; Load size of the word, convert index to bytes offset
  CALL #MUL16x8; Calc offset to the last zone
  EX DE, HL   ; Load into reg DE offset

  LD A, FS_SZ_DIR_ENTITY; Load dir entity size
  LD L, (IX+FS_INODE_SIZE); Set current directory inode, low byte
  LD H, (IX+FS_INODE_SIZE+1); Set current directory inode, high byte
  CALL #DIV16x8; Calc amount of inodes in current directory

  ADD IX, DE  ; Make the offset to the last zone
  POP DE      ; Restore string pointer in reg DE
  PUSH HL     ; Save amount of inodes in curr dir in stack

#FIND_FILE_lp_zone:
  LD L, (IX+FS_INODE_ZONE0); Set directory data from the last zone, low byte
  LD H, (IX+FS_INODE_ZONE0+1); Set directory data from the last zone, high byte

#FIND_FILE_nxt:
  PUSH DE     ; Save ptr to the string in stack
  PUSH HL     ; Save ptr to directory inode in stack
  PUSH BC     ; Save reg BC in stack
  INC HL      ; Move ptr to the byte before of file name start
  INC HL      ; Move ptr to the start of the name

#FIND_FILE_lp:
  LD A, (DE)  ; Load filename char from text buf
  CP (HL)     ; Check if chars are equal to inode
  JR NZ, #FIND_FILE_lp_end-$
  INC HL      ; Move buf ptr by one
  INC DE      ; Move cmd ptr by one
  DJNZ #FIND_FILE_lp-$
  LD A, (HL)  ; Check if filename is ended
  OR A        ; Check if this char is 0, aka end of the word
  JR Z, #FIND_FILE_end-$; If buf counter is ended, aka buf word is less then cmd

#FIND_FILE_lp_end:
  POP BC      ; Restore string length in reg B
  POP HL      ; Restore ptr to the dir inode
  LD DE, FS_SZ_DIR_ENTITY; Load entity size 
  ADD HL, DE  ; Calc the next entity blk
  POP DE      ; Restore ptr to the string
  EX (SP), HL ; Load the amount of inodes in curr dir
  DEC HL      ; Calculate the remaining inodes
  LD A, FS_SZ_ZONE_BLK-1; Load allocated size for zone
  AND L       ; Check if zone is ended 
  EX (SP), HL ; Get ptr to the dir inode
  JR NZ, #FIND_FILE_nxt-$; If reminder is not empty, then get the another dir entity

  DEC IX      ; Move inode pointer to the prev zone
  DEC IX      ; Move inode pointer to the prev zone
  LD A, FS_SZ_ZONE_BLK; Load allocated size for zone
  POP HL      ; Get the amount of dir entity
  CALL #DIV16x8; Check if there are any zones remaning
  OR L        ; Check if the low byte of  is empty
  OR H        ; Check if the high byte is empty
  JR NZ, #FIND_FILE_lp_zone; If there are still additional zones, then check them

  LD IX, 0    ; Reset reg HL
  JR #FIND_FILE_esc-$

#FIND_FILE_end:
  POP BC      ; Restore string length in reg B
  POP HL      ; Get ptr to the inode
  LD E, (HL)  ; Load the low byte of the inode addr
  INC HL      ; Move queue ptr to the high addr of func
  LD D, (HL)  ; Load the high byte of the inode addr
  EX DE, HL   ; Move inode index to the reg HL
  LD A, FS_SZ_INODE; Load inode size
  CALL #MUL16x8; Calc inode address offset
  PUSH HL     ; Temp save inode addr offset in stack
  POP IX      ; Get inode addr offset in reg IX
  LD DE, INODE_MAP; Load inode addr start
  XOR A       ; Reset Acc
  OR L        ; Check if offset of inode is 0
  OR H        ; Check if offset of inode is 0
  JR Z, #FIND_FILE_archived-$; If it zero that means that it's deleted inode, so just skip it
  ADD IX, DE  ; Find current directory inode addr
#FIND_FILE_archived:
  POP DE      ; Restore ptr to the string
  POP HL      ; Restore amount of inodes in curr dir

#FIND_FILE_esc:
  POP HL      ; Restore reg HL
  POP AF      ; Restore reg AF
  RET

;;
;; Example:
;;  LD IX, 5
;;  CALL #HAS_INODE
;; 
;; proc HAS_INODE() -> reg F(Z);
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;   reg IX -- unaffected
;;
#HAS_INODE:
  PUSH HL     ; Save reg HL in stack
  PUSH IX     ; Temp save INODE ptr in stack
  POP HL      ; Load INODE ptr to the reg HL
  XOR A       ; Reset reg Acc
  OR H        ; Check if inode addr is empty
  OR L        ; Check if inode addr is empty
  POP HL      ; Restore reg HL
  RET

;;
;; Example:
;;  LD DE, 0x2000 ; Reg DE can be used inside #TEMP func
;;  LD HL, #TEMP
;;  CALL #ITER_INODE
;; 
;; #TEMP:
;;  RET
;; 
;; proc ITER_INODE() -> callback;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
#ITER_INODE:
  PUSH DE     ; Temp save reg DE in stack
  PUSH HL     ; Temp save function addr
  LD DE, INODE_MAP; Load addr to first inode map
  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes
  EX DE, HL   ; Load to reg DE inodes count, and in reg HL inode start addr
  PUSH HL     ; Temp save calc inode addr
  POP IX      ; Load calc inode addr in reg IX
  POP HL      ; Restore func addr

  INC DE      ; Inc DE by, need to check if reg is 0
#ITER_INODE_offset:
  DEC DE      ; Decrement counter
  XOR A       ; Reset Acc
  OR D        ; Check if counter DE is empty
  OR E        ; Check if counter DE is empty
  JR Z, #ITER_INODE_end-$

  EX DE, HL   ; Swap reg DE & HL, only need for `EX (SP), HL`
  EX (SP), HL ; Swap prev DE with curr aka inode counter
  EX DE, HL   ; Restore reg HL & DE = prev value

  PUSH HL     ;  Temp save function addr
  LD HL, #ITER_INODE_ret
  EX (SP), HL ; Save return addr & get addr for manual call
  JP (HL)     ; Manual call to the func

#ITER_INODE_ret:
  EX DE, HL   ; Swap reg DE & HL, only need for `EX (SP), HL`
  EX (SP), HL ; Swap prev DE with curr aka inode counter
  EX DE, HL   ; Restore reg HL & DE = inode counter

  PUSH BC     ; Temp save reg BC in stack
  LD BC, FS_SZ_INODE; Load byte offset of inode
  ADD IX, BC  ; Find next inode addr of zone 0
  POP BC      ; Restore reg BC after calculation
  JR #ITER_INODE_offset-$

#ITER_INODE_end:
  POP DE      ; Restore reg DE
  RET


;;
;; Example:
;;  LD BC, FS_MODE_DIR | FS_MODE_USR_R | FS_MODE_USR_W; Load mount mode
;;  CALL #NEW_INODE
;; 
;; proc NEW_INODE() -> reg HL & IX;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
#NEW_INODE:
  PUSH DE     ; Save reg DE in stack

  LD HL, #NEW_INODE_iter; Load handler func for iter to call
  CALL #ITER_INODE; Get the next inode addr in IX

  LD (IX+FS_INODE_MODE),   C; Set inode mode low byte
  LD (IX+FS_INODE_MODE+1), B; Set inode mode high byte
  CALL #NEXT_DATA_BLK; Calc next data block

  PUSH IX     ; Temp save addr of curr inode
  LD IX, SUPER_BLOCK_MAP; Load ptr to start of super block
  LD C, (IX+FS_SP_BLK_INODES); Get low byte of inodes size
  LD B, (IX+FS_SP_BLK_INODES+1); Get high byte of inodes size
  INC BC      ; Increment amount of inode by one
  LD (IX+FS_SP_BLK_INODES),   C; Save new value in super block, low byte
  LD (IX+FS_SP_BLK_INODES+1), B; Save new value in super block, high byte
  POP IX      ; Restore ptr to curr inode

  LD (IX+FS_INODE_ZONE0),   L; Set directory data zone0, low byte
  LD (IX+FS_INODE_ZONE0+1), H; Set directory data zone0, high byte
  POP DE      ; Restore reg DE
  RET

#NEW_INODE_iter:
  RET


;;
;; Example:
;;  LD BC, 5
;;  LD DE, mount_name
;;  CALL #NEW_FILENAME_BLK
;; 
;; mount_name:
;;   db "mount"
;; 
;; proc NEXT_DATA_BLK() -> reg HL;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- unaffected
;;
#NEXT_DATA_BLK:
  PUSH IX     ; Save reg IX in reg
  PUSH DE     ; Save reg DE in reg
  PUSH BC     ; Save reg BC in reg

  LD IX, SUPER_BLOCK_MAP; Load ptr to start of super block
  LD L, (IX+FS_SP_BLK_DATA_ST); Get low byte of data block
  LD H, (IX+FS_SP_BLK_DATA_ST+1); Get high byte of data block
  LD C, (IX+FS_SP_BLK_INODES); Get low byte of inodes size
  LD B, (IX+FS_SP_BLK_INODES+1); Get high byte of inodes size

  LD IX, INODE_MAP; Load addr to first inode map
  INC BC      ; Add small adjustment to inlcude first inode

#NEXT_DATA_BLK_lp:
  DEC BC      ; Decrement conter
  XOR A       ; Reset Acc
  OR B        ; Check if reg BC is empty
  OR C        ; Check if reg BC is empty
  JR Z, #NEXT_DATA_BLK_end-$
  LD E, (IX+FS_INODE_ALLOCATED); Get low byte of inodes size
  LD D, (IX+FS_INODE_ALLOCATED+1); Get high byte of inodes size
  ADD HL, DE  ; Calc next data block addr
  LD DE, FS_SZ_INODE; Load byte offset of inode
  ADD IX, DE  ; Calc next inode addr
  JR #NEXT_DATA_BLK_lp-$

#NEXT_DATA_BLK_end:
  POP BC      ; Restore reg BC
  POP DE      ; Restore reg DE
  POP IX      ; Restore reg IX
  RET


;;
;; Example:
;;  LD BC, 5
;;  LD DE, mount_name
;;  CALL #NEW_FILENAME_BLK
;; 
;; mount_name:
;;   db "mount"
;; 
;; proc NEW_FILENAME_BLK() -> reg IX;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
#NEW_FILENAME_BLK:
  PUSH IX     ; Save inode addr in stack
  PUSH HL     ; Save addr to data blk in stack
  PUSH BC     ; Save string counter in stack
  LD L, (IX+FS_INODE_SIZE); Get curr inode size, low byte
  LD H, (IX+FS_INODE_SIZE+1); Get curr inode size, high byte
  LD BC, FS_SZ_FILENAME_BLK; Load filename blk size
  ADD HL, BC  ; Calc new inode size

  LD (IX+FS_INODE_SIZE),   L; Save calculated size in inode, low byte
  LD (IX+FS_INODE_SIZE+1), H; Save calculated size in inode, high byte
  LD (IX+FS_INODE_ALLOCATED),   L; Save calculated size in inode, low byte
  LD (IX+FS_INODE_ALLOCATED+1), H; Save calculated size in inode, high byte
  POP BC      ; Restore string counter
  POP IX      ; Restore data blk addr in reg IX

  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes
  DEC HL      ; Get prev countr, aka curr current
  LD (IX+FS_DIR_INODE),   L; Set directory inode, low byte
  LD (IX+FS_DIR_INODE+1), H; Set directory inode, high byte
  PUSH IX     ; Temp save data blk addr 
  POP HL      ; Load this addr in reg HL
  INC HL      ; Move data blk ptr to the start of file name
  INC HL      ; Move data blk ptr to the start of file name
  EX DE, HL   ; Change string src & dst
  LDIR        ; Copy file name from reg DE to HL
  EX DE, HL   ; Restore src & dst as it was prev
  POP IX      ; Restore inode addr
  RET


;;
;; Example:
;;  LD HL, #TEMP
;;  CALL #ITER_INODE
;; 
;; #TEMP:
;;  RET
;; 
;; proc ITER_FILE_BLK() -> callback;
;;   reg A  -- as defined
;;   reg BC -- uneffected
;;   reg DE -- uneffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
#ITER_FILE_BLK:
  LD A, (IX+FS_INODE_MODE+1); Load file mask into to Acc
  AND FS_MODE_MASK; Get only file types
  CP FS_MODE_FILE_MASK; Check if node is a dir
  RET NZ      ; If file is not the file then just end exec 

  PUSH DE     ; Save reg DE in stack
  PUSH BC     ; Save reg BC in stack
  PUSH HL     ; Temp save function addr
  LD BC, FS_SZ_FILENAME_BLK; Load filename blk size
  LD L, (IX+FS_INODE_SIZE)  ; Get curr inode size, low byte
  LD H, (IX+FS_INODE_SIZE+1); Get curr inode size, high byte

  OR A        ; Reset flags
  SBC HL, BC  ; Calculate file size without zone 0 (aka filename)
  PUSH HL     ; Temp save file size in Stack
  POP BC      ; Load file size into reg BC
  POP HL      ; Restore function addr

  ;; TODO: Add ability to load another zone aka (INC IX)
  LD E, (IX+FS_INODE_ZONE1)  ; Get data zone 1 addr, low byte
  LD D, (IX+FS_INODE_ZONE1+1); Get data zone 1 addr, high byte

  DEC DE      ; Dec DE by, need just for adjustmens
  INC BC      ; Inc BC by, need to check if reg is 0
#ITER_FILE_BLK_lp:
  INC DE      ; Move data block ptr by one
  DEC BC      ; Decrement counter
  XOR A       ; Reset Acc
  OR C        ; Check if counter DE is empty
  OR B        ; Check if counter DE is empty
  JR Z, #ITER_FILE_BLK_end-$
  PUSH HL     ; Temp save func addr in stack
  LD HL, #ITER_FILE_BLK_lp; Load return addr
  EX (SP), HL ; Save return addr in stack & get in HL func addr
  JP (HL)     ; Manual call

#ITER_FILE_BLK_end:
  POP BC      ; Restore reg BC
  POP DE      ; Restore reg DE
  RET

;;
;; Example:
;;  LD BC, 5
;;  LD DE, mount_name
;;  CALL #PRINT_INODE
;; 
;; PRINT_INODE:
;;   db "mount"
;; 
;; proc PRINT_INODE() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
_FS_EXEC:
  ; LD HL, PTR_SYNC_BYTE; Load ptr to the sync byte
  ; LD A, ~SYNC_ACK; Set sync ack mask byte to Acc
  ; AND (HL)   ; Reset SYNC ACK bit
  ; LD (HL), A ; Save new SYNC byte in memory

  ; TODO: 
  ; LD B, 8
  ; LD DE, .FILE_TEST
  ; CALL #FIND_FILE

  LD HL, PTR_STDIN_BUF_BGN; Load ptr to the start offset of STDIN
  LD A, (PTR_STDIN_BUF_END); Load the position of the last byte in stdin
  LD DE, STDIN_BUF; Load ptr to the start of stdin buffer
  LD E, (HL) ; Load current index of STDIN & make offset for reg DE

  SUB E      ; Calc the diff of position end - index
  PUSH DE    ; Save in stack start of string
  LD B, A    ; Load diff between end & index to the reg C
  LD C, 0    ; Reset reg C, use it as a string length

_FS_EXEC_lp:
  LD A, (DE) ; Load current ASCII char
  INC E      ; Increment ptr to the next ASCII char
  LD (HL), E ; Update the start of STDIN ptr
  CP SPACE   ; Check if char is ' '
  JR Z, _FS_EXEC_lp_esc-$
  CP LINE_FEED ; Check if char is '\n'
  JR Z, _FS_EXEC_lp_esc-$
  INC C      ; Increment filename string length
  DJNZ _FS_EXEC_lp-$

_FS_EXEC_lp_esc:
  LD B, C    ; Load file name string size
  POP DE     ; Restore the start of the string

  CALL #FIND_FILE; Will return reg IX which will point to the found INODE or 0
  ; CALL #HAS_INODE; Check if the file was founded

  ; TODO: if not found show error ????????
  ; RET Z

  LD A, FS_SZ_ZONE_BLK; Load zone size
  LD L, (IX+FS_INODE_SIZE); Set current directory inode, low byte
  LD H, (IX+FS_INODE_SIZE+1); Set current directory inode, high byte
  CALL #DIV16x8; Calc index offset to the last zone
  LD B, L     ; Load the last zone index
  INC B       ; Add 1 to the index zone, for adjusting with DJNZ

  LD DE, RAM_ADDR
  PUSH DE

_FS_EXEC_load:
  LD L, (IX+FS_INODE_ZONE0); Set directory data from the last zone, low byte
  LD H, (IX+FS_INODE_ZONE0+1); Set directory data from the last zone, high byte

  PUSH BC
  LD BC, 5


  LDIR        ; Copy program to the MEMORY
  INC IX      ; Move inode pointer to the next zone
  INC IX      ; Move inode pointer to the next zone

  POP BC
  DJNZ _FS_EXEC_load-$

  POP HL
  JP (HL)
  
  ; LD HL, .MSG_FILE_NOT_FOUND; Load ptr to the string
  ; JP Z, #STR_PRINT; If file is not found then display an error

  ; LD HL, .MSG_OK; Load ptr to the string
  ; JP #STR_PRINT; If file is not found then display an error



;   CALL #STR_STRIP; Strip the STDIN string
;   XOR A      ; Reset Acc
;   OR B       ; Check if reg B reached the end
  
;   ; TODO: Think how to do this !!!
;   ; JP Z, #MSG_FILE_NOT_FOUND; If for some reason it reached the end, display an error
;   PUSH DE    ; Save ptr to the STDIN in stack

; ;;  LD B, 5   ; String length
; ;;  LD DE, .STRING  ; Load ptr to the string in reg DE
; ;;  CALL #FIND_FILE; Will return reg IX which will point to the found INODE or 0

; _FS_EXEC_lp:
;   LD A, (DE) ; Load current ASCII char
;   INC E      ; Increment ptr to the next ASCII char
;   CP LINE_FEED; Check if char is '\n'
;   JR Z, _FS_EXEC_lp_esc-$; Exec cmd if STDIN reached LINE_FEED
;   CP SPACE    ; Check if char is ' '
;   JR Z, _FS_EXEC_lp_esc-$; Exec cmd if STDIN reached ' ' and treat everything after as args

;   DJNZ _FS_EXEC_lp-$
;   RET         ; End filesystem exec proc if '/n' is not found in STDIN


; _FS_EXEC_lp_esc:
;   LD A, E    ; Load to Acc the end of the STDIN cmd
;   LD (HL), E ; Update the start offset to reach cmd end

;   POP DE     ; Restore ptr to the STDIN start
;   SUB E      ; Calc string length
;   LD B, A    ; Move string length into reg B
;   DEC B      ; Remove from string size '\n'
;   RET Z      ; If string length is 0, then skip file searching

;   CALL #FIND_FILE; Will return reg IX which will point to the found INODE or 0
;   CALL #HAS_INODE; Check if the file was founded
  
;   LD HL, .MSG_FILE_NOT_FOUND; Load ptr to the string
;   JP Z, #STR_PRINT; If file is not found then display an error

;;  LD B, 5   ; String length
;;  LD DE, .STRING  ; Load ptr to the string in reg DE
;;  CALL #FIND_FILE; Will return reg IX which will point to the found INODE or 0


#TTTTTTTTTTTTTTT:
  LD HL, .MSG_FILE_NOT_FOUND
  RST 0x18    ; Print string
  RET

;;
;; Example:
;;  LD BC, 5
;;  LD DE, mount_name
;;  CALL #PRINT_INODE
;; 
;; PRINT_INODE:
;;   db "mount"
;; 
;; proc PRINT_INODE() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
#PRINT_INODE:
  LD A, (IX+FS_INODE_MODE+1); Load file mask into to Acc
  AND FS_MODE_MASK; Get only file type
  CP FS_MODE_SOKET_MASK; Check if node is a socket
  LD C, "s"   ; Load "s" char into reg C
  JR Z, #PRINT_INODE_size-$
  CP FS_MODE_SLINK_MASK; Check if node is a dir
  LD C, "l"   ; Load "l" char into reg C
  JR Z, #PRINT_INODE_size-$
  CP FS_MODE_BLOCK_MASK; Check if node is a block
  LD C, "b"   ; Load "b" char into reg C
  JR Z, #PRINT_INODE_size-$
  CP FS_MODE_DIR_MASK; Check if node is a dir
  LD C, "d"   ; Load "d" char into reg C
  JR Z, #PRINT_INODE_size-$
  LD C, "f"   ; Load "f" char into reg C

#PRINT_INODE_size:
  LD A, C     ; Load char into Acc
  RST 0x10    ; Print the char
  LD A, " "   ; Make a break between prev value
  RST 0x10

  LD BC, 0x0200; Display only first 2 bytes & reset reg C
  LD HL, 0      ; Load ptr to the temp val
  PUSH IX     ; Temp save ptr to the curr inode

#PRINT_INODE_size_lp:
  LD A, (IX+FS_INODE_SIZE+1); Load file mask into to Acc
  OR A        ; Check if Acc is empty
  JR NZ, #PRINT_INODE_size_lp_print-$
  OR C        ; Check we reach to non zero number and we cant ignore it now
  JR Z, #PRINT_INODE_size_lp_end-$
#PRINT_INODE_size_lp_print:
  LD C, B     ; Save random value to reg C, shoud be not null
  LD (HL), A  ; Copy size byte into temp ptr
  PUSH BC     ; Save curr loop counter
  CALL #HEX_ASCII; Display first byte
  POP BC      ; Restore curr loop counter
#PRINT_INODE_size_lp_end:
  DEC IX      ; Move inode ptr back to one
  DJNZ #PRINT_INODE_size_lp-$
  POP IX      ; Restore ptr to the start of curr inode

  LD A, "h"   ; Show that size displaied in hex
  RST 0x10

  LD A, " "   ; Make a break between prev value
  RST 0x10

  LD L, (IX+FS_INODE_ZONE0); Set directory data zone0, low byte
  LD H, (IX+FS_INODE_ZONE0+1); Set directory data zone0, high byte
  INC HL      ; Move ptr to the byte before of file name start
  INC HL      ; Move ptr to the start of the name
  RST 0x18    ; Print filaname
  RET


;; File System Inode offsets
FS_INODE_MODE          EQU 0x00
FS_INODE_UID           EQU 0x02
FS_INODE_SIZE          EQU 0x04
FS_INODE_ALLOCATED     EQU 0x06
FS_INODE_CREATED       EQU 0x08
FS_INODE_UPDATED       EQU 0x08
FS_INODE_GID           EQU 0x0C
FS_INODE_LINKS         EQU 0x0D
FS_INODE_ZONE0         EQU 0x0E
FS_INODE_ZONE1         EQU 0x10
FS_INODE_ZONE2         EQU 0x12
FS_INODE_ZONE3         EQU 0x14
FS_INODE_ZONE4         EQU 0x16
FS_INODE_ZONE5         EQU 0x18
FS_INODE_ZONE6         EQU 0x1A
FS_INODE_ZONE7         EQU 0x1C
FS_INODE_ZONE8         EQU 0x1E

;; File System Inode offsets
FS_SP_BLK_INODES        EQU 0x00
FS_SP_BLK_ZONES         EQU 0x02
FS_SP_BLK_BLK_MAP       EQU 0x04
FS_SP_BLK_ZMAP_BLK      EQU 0x06
FS_SP_BLK_DATA_ST       EQU 0x08
FS_SP_BLK_ZONE_SIZE     EQU 0x0A
FS_SP_BLK_FILE_SIZE     EQU 0x0C
FS_SP_BLK_RAND          EQU 0x10
FS_SP_BLK_STATE         EQU 0x12


;; File System Dir offsets
FS_DIR_INODE           EQU 0x00
FS_DIR_FILENAME        EQU 0x02

;; Mode masks
FS_MODE_MASK          EQU 0xF0
FS_MODE_SOKET_MASK    EQU 0xC0
FS_MODE_SLINK_MASK    EQU 0xA0
FS_MODE_FILE_MASK     EQU 0x80
FS_MODE_BLOCK_MASK    EQU 0x60
FS_MODE_DIR_MASK      EQU 0x40

;; File System Inode mode
FS_MODE_SOKET          EQU 0xC000
FS_MODE_SLINK          EQU 0xA000
FS_MODE_FILE           EQU 0x8000
FS_MODE_BLOCK          EQU 0x6000
FS_MODE_DIR            EQU 0x4000
FS_MODE_CHR            EQU 0x2000
FS_MODE_FIFO           EQU 0x1000
FS_MODE_UID            EQU 0x0800
FS_MODE_GID            EQU 0x0400
FS_MODE_VTX            EQU 0x0200

FS_MODE_USR_R          EQU 0x0100
FS_MODE_USR_W          EQU 0x0080
FS_MODE_USR_X          EQU 0x0040

FS_MODE_GRP_R          EQU 0x0020
FS_MODE_GRP_W          EQU 0x0010
FS_MODE_GRP_X          EQU 0x0008

FS_MODE_OTH_R          EQU 0x04
FS_MODE_OTH_W          EQU 0x02
FS_MODE_OTH_X          EQU 0x01


FS_UID_ROOT            EQU 0x00
FS_GID_ROOT            EQU 0x00


;; Mount state
FS_MOUNT_DI       EQU 0x00
FS_MOUNT_EN       EQU 0x01

; Sizes
FS_SZ_INODE            EQU 0x20
FS_SZ_DIR_ENTITY       EQU 0x20
FS_SZ_ZONE_BLK         EQU 0x80

; FIXME: Delete this
FS_SZ_FILENAME_BLK     EQU 0x20

;; Inodes
; FILENAME_SCAN_KEY_BUF     EQU DATA_ZONE_MAP ;; Map SCAN_KEY_BUF
; FILENAME_TEXT_BUF_MAP     EQU DATA_ZONE_MAP +  FS_SZ_FILENAME_BLK;; Map TEXT_BUF_MAP

;; FS MODE
FS_FILE_TYPE      EQU FS_MODE_FILE | FS_MODE_USR_R | FS_MODE_USR_W
FS_DIR_TYPE       EQU FS_MODE_DIR  | FS_MODE_USR_R | FS_MODE_USR_W

.SUPER_BLOCK:
  ;;  INDOES  ZONES   IMAP BLOCKS   ZMAP BLOCKS   DATA ZONE START   ZONE SIZE   MAX FILE SIZE
  dw   0x02,   0x00,    0x00,         0x00,        DATA_ZONE_MAP,     0xFF,       0x00
  
  ;; RAND VAL  MOUNT STATE
  dw  0x00,     FS_MOUNT_EN
.SUPER_BLOCK_ED:

.INODE_BLOCK:
  ;; ====================================================================
  ;;   MODE           UID         SIZE                 ALLOCATED       CREATED   UPDATED  GID|LINKS     ZONE0
  dw  FS_DIR_TYPE,    0x00,  FS_SZ_DIR_ENTITY + FS_SZ_DIR_ENTITY + FS_SZ_DIR_ENTITY,  FS_SZ_ZONE_BLK,     0x00,    0x00,     0x00,    DATA_ZONE_MAP
  
  ;;   ZONE1  ZONE2  ZONE3  ZONE4  ZONE5  ZONE6  ZONE7  ZONE8
  dw   0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00

  ;; ====================================================================
  ;;   MODE           UID     SIZE         ALLOCATED   CREATED   UPDATED  GID|LINKS     ZONE0
  dw  FS_FILE_TYPE,   0x00,  0x002F,    FS_SZ_ZONE_BLK,      0x00,    0x00,     0x00,    SCAN_KEY_BUF
  
  ;;  ZONE1  ZONE2  ZONE3  ZONE4  ZONE5  ZONE6  ZONE7  ZONE8
  dw  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00

  ;; ====================================================================
  ;;   MODE           UID     SIZE       ALLOCATED   CREATED  UPDATED   GID|LINKS     ZONE0
  dw  FS_FILE_TYPE,   0x00,  0x011F,   FS_SZ_ZONE_BLK,     0x00,     0x00,     0x00,    TEXT_BUF_MAP
  
  ;;  ZONE1  ZONE2  ZONE3  ZONE4  ZONE5  ZONE6  ZONE7  ZONE8
  dw  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00
  ;; ====================================================================
  ;;   MODE           UID     SIZE       ALLOCATED   CREATED  UPDATED   GID|LINKS     ZONE0
  dw  FS_FILE_TYPE,   0x00,  0x0005,   FS_SZ_ZONE_BLK,     0x00,     0x00,     0x00,    #TTTTTTTTTTTTTTT
  
  ;;  ZONE1  ZONE2  ZONE3  ZONE4  ZONE5  ZONE6  ZONE7  ZONE8
  dw  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00,  0x00
.INODE_BLOCK_ED:

.FILENAME_BLOCK:
  ;; ====================================================================
  dw  1;; FILENAME_SCAN_KEY_BUF
  db "scan-key",  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0xFF
  ;; ====================================================================
  dw  2;; FILENAME_TEXT_BUF_MAP
  db "text-buf",  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0xFF
  ;; ====================================================================
  dw  3;; FILENAME_TEXT_BUF_MAP
  db "test",  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 0xFF
  ;; ====================================================================
.FILENAME_BLOCK_ED: