;; NOTE: Inspired by this article
;; http://ohm.hgesser.de/sp-ss2012/Intro-MinixFS.pdf


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