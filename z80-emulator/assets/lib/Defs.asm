; Variables name
STACK          EQU 0x5FFF
LED_PORT       EQU 0x00
HEX_PORT       EQU 0x10
PPI_PORT_A     EQU 0x20
PPI_PORT_B     EQU 0x21
PPI_PORT_C     EQU 0x22

LCD_IO_EN      EQU 0x03
LCD_IO_CMD     EQU 0x01



LCD_CLEAR      EQU 0x01
LCD_RETURN     EQU 0x02
LCD_MODE       EQU 0x04
LCD_DISPLAY    EQU 0x08
LCD_CURSOR     EQU 0x10
LCD_FUNCTION   EQU 0x20
LCD_CGRAM      EQU 0x40
LCD_DDRAM      EQU 0x80

LCD_CURSOR_LEFT  EQU 0x10
LCD_CURSOR_RIGHT EQU 0x14

; Static FUNC ptr
PTR_ADDR           EQU 0x4000
PTR_LCD_MODE       EQU PTR_ADDR + 1
PTR_LCD_DISPLAY    EQU PTR_ADDR + 2
PTR_LCD_CURSOR     EQU PTR_ADDR + 3
PTR_PRINTF_RET     EQU PTR_ADDR + 4

; Dynamic args
PTR_FUNC_ARGS      EQU PTR_ADDR + 5