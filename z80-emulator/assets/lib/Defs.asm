; Variables name
STACK          EQU 0x5FFF
LED_PORT       EQU 0x00
HEX_PORT       EQU 0x10
PPI_PORT_A     EQU 0x20
PPI_PORT_B     EQU 0x21
PPI_PORT_C     EQU 0x22
PPI_PORT_CTRL  EQU 0x23

PPI_CSA        EQU 0x04
PPI_CSR        EQU 0x08
PPI_CSB        EQU 0x80

PPI_IO_LCD     EQU 0x10

; IO_LCD_EN      EQU 0x03
; IO_LCD_CMD     EQU 0x01



;; Static FUNC ptr
PTR_ADDR           EQU 0x4000
PTR_LCD_CURSOR     EQU PTR_ADDR + 1
PTR_PRINTF_RET     EQU PTR_ADDR + 2
PTR_PREV_SCAN_CODE EQU PTR_ADDR + 3
PTR_TEXT_BUFF_BGN  EQU PTR_ADDR + 4
PTR_TEXT_BUFF_END  EQU PTR_ADDR + 5
PTR_TEMP_WORD      EQU PTR_ADDR + 6 ;; 2 bytes
PTR_INPUT_STATE    EQU PTR_ADDR + 8 ;; char '>' | '|'
PTR_INPUT_FUNC     EQU PTR_ADDR + 9 ;; 2 bytes
PTR_INPUT_FILE     EQU PTR_ADDR + 11;; 2 bytes

; Dynamic args
PTR_FUNC_ARGS      EQU PTR_ADDR + 8


;; Allocated memory
SCAN_KEY_BUF       EQU 0x40F0  ;; F0h - FFh Keybuffer
SCAN_KEY_MAP       EQU 0x4100  ;; 00h - FFh scan codes
TEXT_BUF_MAP       EQU 0x4200  ;; 00h - FFh text buffer
SUPER_BLOCK_MAP    EQU 0x4300  ;; 00h - 1Fh Super block map
INODE_MAP          EQU 0x4320  ;; 20h - FFh Inode map
DATA_ZONE_MAP      EQU 0x4400  ;; 00h - FFh Data zone map


#include "ASCII.asm"