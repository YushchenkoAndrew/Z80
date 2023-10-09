#include "Defs.asm"

;; NOTE: Inspired by this article
;; http://ohm.hgesser.de/sp-ss2012/Intro-MinixFS.pdf


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
  PUSH BC     ; Save reg BC in stack
  LD BC, FS_SZ_INODE; Load byte offset of inode
  LD DE, INODE_MAP; Load addr to first inode map
  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes
  EX DE, HL   ; Load to reg DE inodes count, and in reg HL inode start addr
  INC DE      ; Inc DE by, need to check if reg is 0
  XOR A       ; Reset Acc
#NEW_INODE_offset:
  DEC DE      ; Decrement counter
  CP D        ; Check if reg D is empty
  JR NZ, #NEW_INODE_offset_nxt-$
  CP E        ; Check if reg E is empty
  JR Z, #NEW_INODE_offset_esc-$
#NEW_INODE_offset_nxt:
  ADD HL, BC  ; Find next inode addr of zone 0
  JR #NEW_INODE_offset-$

#NEW_INODE_offset_esc:
  PUSH HL     ; Temp save calculated addr 
  POP IX      ; Load this addr in reg IX
  POP BC      ; Restore reg BC
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
  CP B        ; Check if reg B is empty
  JR NZ, #NEW_INODE_blk_offset_nxt-$
  CP C        ; Check if reg C is empty
  JR Z, #NEW_INODE_blk_esc-$
#NEW_INODE_blk_offset_nxt:
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


;;
;; Example:
;;  LD BC, 5
;;  LD DE, mount_name
;;  CALL #NEW_DIR_BLK
;; 
;; mount_name:
;;   db "mount"
;; 
;; proc NEW_DIR_BLK() -> reg IX;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
;;   reg IX -- as defined
;;
#NEW_DIR_BLK:
  PUSH HL     ; Save addr to data blk in stack
  PUSH BC     ; Save string counter in stack
  LD L, (IX+FS_INODE_SIZE); Get curr inode size, low byte
  LD H, (IX+FS_INODE_SIZE+1); Get curr inode size, high byte
  LD BC, FS_SZ_DIR_BLK; Load directory blk size
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


; Sizes
FS_SZ_INODE            EQU 0x20
FS_SZ_DIR_BLK          EQU 0x20

.SUPER_BLOCK:
  ;;  INDOES  ZONES   IMAP BLOCKS   ZMAP BLOCKS   DATA ZONE START   ZONE SIZE   MAX FILE SIZE
  dw  0x00,    0x00,    0x00,         0x00,        DATA_ZONE_MAP,     0xFF,       0x00
  
  ;; RAND VAL  MOUNT STATE
  dw  0x00,     MOUNT_EN
.SUPER_BLOCK_ED: