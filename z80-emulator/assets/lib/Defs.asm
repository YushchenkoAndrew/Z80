; Variables name
STACK          EQU 0x5FFF
LED_PORT       EQU 0x00
SWITCH_PORT    EQU 0x00
HEX_PORT       EQU 0x10
PPI_PORT_A     EQU 0x20
PPI_PORT_B     EQU 0x21
PPI_PORT_C     EQU 0x22
PPI_PORT_CTRL  EQU 0x23
UART_PORT      EQU 0x30
KEYBOARD_PORT  EQU 0x40
INT_PORT       EQU 0x50
PIT_PORT_CT0   EQU 0x80
PIT_PORT_CT1   EQU 0x81
PIT_PORT_CT2   EQU 0x82
PIT_PORT_CTL   EQU 0x83

PPI_CS_CS0        EQU 0x01
PPI_CS_CS1        EQU 0x02
PPI_CS_CSA        EQU 0x04
PPI_CS_CSW        EQU 0x08
PPI_CS_CS3        EQU 0x10
PPI_CS_CS4        EQU 0x20
PPI_CS_CS5        EQU 0x40
PPI_CS_CSB        EQU 0x80

PPI_IO_LCD     EQU 0x10

; IO_LCD_EN      EQU 0x03
; IO_LCD_CMD     EQU 0x01

IM_KEYBOARD   EQU 0x80
IM_RxRDY      EQU 0x40
IM_TxRDY      EQU 0x20
IM_RLT        EQU 0x10
IM_CT1        EQU 0x08
IM_CT2        EQU 0x04
IM_NONE1      EQU 0x02
IM_NONE2      EQU 0x01


;; Static FUNC ptr
PTR_INTR           EQU 0x4000
PTR_LCD_CURSOR     EQU PTR_INTR + 1
PTR_PRINTF_RET     EQU PTR_INTR + 2
PTR_PREV_SCAN_CODE EQU PTR_INTR + 3
PTR_TEXT_BUFF_BGN  EQU PTR_INTR + 4
PTR_TEXT_BUFF_END  EQU PTR_INTR + 5
PTR_TEMP_WORD      EQU PTR_INTR + 6  ;; 2 bytes ;; TODO: Make this word separate for each file Command / FileSystem
PTR_INPUT_STATE    EQU PTR_INTR + 8  ;; char '>' | '|'
PTR_INPUT_FUNC     EQU PTR_INTR + 9  ;; 2 bytes
PTR_INPUT_FILE     EQU PTR_INTR + 11 ;; 2 bytes
; PTR_CT1_CONF       EQU PTR_INTR + 14 ;; 3 bytes
; PTR_CT2_CONF       EQU PTR_INTR + 17 ;; 3 bytes

; Dynamic args
; PTR_FUNC_ARGS      EQU PTR_ADDR + 8


;; Allocated memory
SCAN_KEY_BUF       EQU 0x40F0  ;; F0h - FFh Keybuffer
SCAN_KEY_MAP       EQU 0x4100  ;; 00h - FFh scan codes
TEXT_BUF_MAP       EQU 0x4200  ;; 00h - FFh text buffer
TASK_BUF_MAP       EQU 0x4300  ;; 00h - FFh task buffer
CT1_BUF_MAP        EQU 0x4400  ;; 00h - FFh CT1 task buffer
CT2_BUF_MAP        EQU 0x4500  ;; 00h - FFh CT2 task buffer
SUPER_BLOCK_MAP    EQU 0x4600  ;; 00h - 1Fh Super block map
INODE_MAP          EQU 0x4620  ;; 20h - FFh Inode map
DATA_ZONE_MAP      EQU 0x4700  ;; 00h - FFh Data zone map


#include "ASCII.asm"