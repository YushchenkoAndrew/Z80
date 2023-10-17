#include "Utils.asm"

;; NOTE: Inspired by this article
;; http://ohm.hgesser.de/sp-ss2012/Intro-MinixFS.pdf

;;
;; Example:
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
;;  LD HL, #TEMP
;;  CALL #ITER_INODE
;; 
;; #TEMP:
;;  RET
;; 
;; proc ITER_INODE() -> reg IX;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;   reg IX -- as defined
;;
#FILENAME_FIND_ITER:
  PUSH HL     ; Save reg HL in stack
  PUSH DE     ; Save reg DE in stack
  PUSH BC     ; Save reg BC in stack

  LD L, (IX+FS_INODE_ZONE0); Set directory data zone0, low byte
  LD H, (IX+FS_INODE_ZONE0+1); Set directory data zone0, high byte
  INC HL      ; Move ptr to the byte before of file name start
  INC HL      ; Move ptr to the start of the name

#FILENAME_FIND_ITER_lp:
  LD A, (DE)  ; Load filename char from text buf
  CP (HL)     ; Check if chars are equal to inode
  JR NZ, #FILENAME_FIND_ITER_end-$
  INC HL      ; Move buf ptr by one
  INC DE      ; Move cmd ptr by one
  DJNZ #FILENAME_FIND_ITER_lp-$
  LD A, (HL)  ; Check if filename is ended
  OR A        ; Check if this char is 0, aka end of the word
  JR NZ, #FILENAME_FIND_ITER_end-$; If buf counter is ended, aka buf word is less then cmd

  PUSH IX     ; Temp save reg IX in stack
  POP HL      ; Load reg IX into reg HL
  LD A, L     ; Load into reg A low byte of the inode
  LD (PTR_TEMP_WORD),   A; Reset low byte of the word
  LD A, H     ; Load into reg A high byte of the inode
  LD (PTR_TEMP_WORD+1), A; Reset high byte of the word

#FILENAME_FIND_ITER_end:
  POP BC      ; Restore reg BC
  POP DE      ; Restore reg DE
  POP HL      ; Restore reg HL
  RET

;;
;; Example:
;;  LD B, 5
;;  CALL #IS_FILENAME_EXIST
;; 
;; proc IS_FILENAME_EXIST() -> reg HL & reg F(Z);
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;   reg IX -- as defined
;;
#IS_FILENAME_EXIST:
  XOR A       ; Reset reg Acc
  LD (PTR_TEMP_WORD),   A; Reset low byte of the word
  LD (PTR_TEMP_WORD+1), A; Reset high byte of the word

  LD HL, #FILENAME_FIND_ITER; Load handler func for iter to call
  CALL #ITER_INODE; Iterate throgh inodes

  LD HL, (PTR_TEMP_WORD); Load inode addr into reg HL
  XOR A       ; Reset reg Acc
  OR H        ; Check if inode addr is empty
  OR L        ; Check if inode addr is empty
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
  PUSH IX     ; Temp save addr of curr inode

  LD IX, SUPER_BLOCK_MAP; Load ptr to start of super block
  LD L, (IX+FS_SP_BLK_DATA_ST); Get low byte of data block
  LD H, (IX+FS_SP_BLK_DATA_ST+1); Get high byte of data block
  LD C, (IX+FS_SP_BLK_INODES); Get low byte of inodes size
  LD B, (IX+FS_SP_BLK_INODES+1); Get high byte of inodes size

  INC BC      ; Increment amount of inode by one
  LD (IX+FS_SP_BLK_INODES),   C; Save new value in super block, low byte
  LD (IX+FS_SP_BLK_INODES+1), B; Save new value in super block, high byte
  LD IX, INODE_MAP; Load addr to first inode map

#NEW_INODE_blk_offset:
  DEC BC      ; Decrement conter
  XOR A       ; Reset Acc
  OR B        ; Check if reg BC is empty
  OR C        ; Check if reg BC is empty
  JR Z, #NEW_INODE_blk_esc-$
  LD E, (IX+FS_INODE_SIZE); Get low byte of inodes size
  LD D, (IX+FS_INODE_SIZE+1); Get high byte of inodes size
  ADD HL, DE  ; Calc next data block addr
  LD DE, FS_SZ_INODE; Load byte offset of inode
  ADD IX, DE  ; Calc next inode addr
  JR #NEW_INODE_blk_offset-$

#NEW_INODE_blk_esc:
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
;; proc NEW_FILENAME_BLK() -> reg IX;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
#NEW_FILENAME_BLK:
  PUSH HL     ; Save addr to data blk in stack
  PUSH BC     ; Save string counter in stack
  LD L, (IX+FS_INODE_SIZE); Get curr inode size, low byte
  LD H, (IX+FS_INODE_SIZE+1); Get curr inode size, high byte
  LD BC, FS_SZ_FILENAME_BLK; Load filename blk size
  ADD HL, BC  ; Calc new inode size

  LD (IX+FS_INODE_SIZE),   L; Save calculated size in inode, low byte
  LD (IX+FS_INODE_SIZE+1), H; Save calculated size in inode, high byte
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

  LD BC, 0x0400; Display only first 4 bytes & reset reg C
  LD HL, PTR_TEMP_WORD      ; Load ptr to the temp val
  PUSH IX     ; Temp save ptr to the curr inode

#PRINT_INODE_size_lp:
  LD A, (IX+FS_INODE_SIZE+3); Load file mask into to Acc
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
FS_INODE_TIME          EQU 0x08
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
FS_SZ_FILENAME_BLK     EQU 0x20

.SUPER_BLOCK:
  ;;  INDOES  ZONES   IMAP BLOCKS   ZMAP BLOCKS   DATA ZONE START   ZONE SIZE   MAX FILE SIZE
  dw  0x00,    0x00,    0x00,         0x00,        DATA_ZONE_MAP,     0xFF,       0x00
  
  ;; RAND VAL  MOUNT STATE
  dw  0x00,     FS_MOUNT_EN
.SUPER_BLOCK_ED:

; TODO: Add DEFAULT INODE BLOCK & DATA BLOCK for each map & buffer