#include "Defs.asm"

;; NOTE: Inspired by this article
;; http://ohm.hgesser.de/sp-ss2012/Intro-MinixFS.pdf


_NEW_INODE:
  PUSH DE     ; Save reg DE in stack
  LD DE, INODE_MAP; Load addr to first inode map
  LD HL, (SUPER_BLOCK_MAP+FS_SP_BLK_INODES); Get amount of inodes
  ADD HL, DE  ; Find next inode addr of zone 0
  PUSH HL     ; Temp save calculated addr 
  POP IX      ; Load this addr in reg IX
  POP DE      ; Restore reg DE
  RET


_NEW_DATA_BLOCK:
  LD IX, SUPER_BLOCK_MAP; Load ptr to start of super block
  LD L, (IX+FS_SP_BLK_DATA_ST); Get low byte of data block
  LD H, (IX+FS_SP_BLK_DATA_ST+1); Get high byte of data block
  LD E, (IX+FS_SP_BLK_ZONES); Get low byte of zone size
  LD D, (IX+FS_SP_BLK_ZONES+1); Get high byte of zone size

  INC (IX+FS_SP_BLK_INODES); Increase emount of inode by one
  INC (IX+FS_SP_BLK_ZONES); Increase emount of zones by one
  ADD HL, DE  ; Calc next data block addr
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
FS_MODE_SOKET          EQU 0xC0
FS_MODE_SLINK          EQU 0xA0
FS_MODE_FILE           EQU 0x80
FS_MODE_BLOCK          EQU 0x60
FS_MODE_DIR            EQU 0x40
FS_MODE_CHR            EQU 0x20
FS_MODE_FIFO           EQU 0x10
FS_MODE_UID            EQU 0x08
FS_MODE_GID            EQU 0x04
FS_MODE_VTX            EQU 0x02
FS_MODE_USR_R          EQU 0x01

FS_MODE_USR_W          EQU 0x80
FS_MODE_USR_X          EQU 0x40

FS_MODE_GRP_R          EQU 0x20
FS_MODE_GRP_W          EQU 0x10
FS_MODE_GRP_X          EQU 0x08

FS_MODE_OTH_R          EQU 0x04
FS_MODE_OTH_W          EQU 0x02
FS_MODE_OTH_X          EQU 0x01


FS_UID_ROOT            EQU 0x00
FS_GID_ROOT            EQU 0x00

.SUPER_BLOCK:
  ;;  INDOES  ZONES   IMAP BLOCKS   ZMAP BLOCKS   DATA ZONE START   ZONE SIZE   MAX FILE SIZE
  dw  0x00,    0x00,    0x00,         0x00,        DATA_ZONE_MAP,     0xFF,       0x00
  
  ;; RAND VAL  MOUNT STATE
  dw  0x00,     MOUNT_EN
.SUPER_BLOCK_ED: