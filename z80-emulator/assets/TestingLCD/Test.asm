;#################################
;  PROGRAM NAME: - TestLCD
;  DISCRIPTION:  - Display some simple words in 7-display
;  DATE: 27.02.21
;  AUTOR: Andrew Yushchenko
;#################################


  org 0x0000
  JP setup

number:
  db "Number: '%d'", 0

number2:
  db "123", 0

lcd_settings:
  db 0b00000111 ;; PTR_LCD_MODE,    
  db 0b00001011 ;; PTR_LCD_DISPLAY

setup:
  ; Set Memory Paging RAM
  LD SP, 0x5FFF

  ; INIT PPI
  ; LD A, 0b10000000
  ; OUT (0x23), A


  ;; Load settings
  LD BC, 0x02
  LD HL, lcd_settings
  LD DE, PTR_LCD_MODE
  LDIR

  CALL _LCD_INIT
  
  ; LD A, LCD_IO_EN   ;; Enter data mode
  ; OUT (PPI_PORT_C), A

  LD HL, PTR_FUNC_ARGS
  PUSH HL         ;; ptr to arg
  LD (HL), 0x18

  LD BC, 0x21 ;; PORT
  PUSH BC


  LD BC, number ;; string
  PUSH BC

  CALL _PRINTF

  ; LD BC, 0x00FF
  ; XOR A
  ; CALL delay

  ; Print "Number"
  ; LD BC, number2 ;; args
  ; LD C, "A"

  JP main


main:
  ; RETURN LCD
  ; LD A, 0x02
  ; OUT (0x20), A

  ; Show Number
  IN A, (0x00)
  OUT (0x00), A

  ; PUSH AF

  LD HL, PTR_FUNC_ARGS
  LD (HL), A
  PUSH HL
  CALL _HEX

  ; POP AF

  ; POP HL     ;; Get return addr
  ; POP DE     ;; Get string addr
  ; POP BC     ;; Get output port
  ; EX (SP), HL;; Restore return arg & Get ptr to args

  ; LD BC, PTR_FUNC_ARGS

  JP main

#include "../lib/Hex.asm"
#include "../lib/Printf.asm"


#LCD_EXEC_CMD:
  ; PUSH AF          ;; Save reg A in stack
  ; LD A, 0          ;; Disable every IO device
  ; OUT (PPI_PORT_C), A
  ; POP AF           ;; Restore reg A
  OUT (0x20), A
  ; LD A, LCD_IO_CMD ;; Enable output to LCD in CMD mode
  ; OUT (PPI_PORT_C), A
  RET

_LCD_INIT:
  PUSH HL    ;; Save HL value
  LD HL, PTR_LCD_MODE ;; Load ptr to LCD MODE value
  LD A, (HL) ;; Load LCD MODE CMD
  CALL #LCD_EXEC_CMD
  LD HL, PTR_LCD_DISPLAY ;; Load ptr to LCD DISPLAY value
  LD A, (HL) ;; Load LCD DISPLAY CMD
  CALL #LCD_EXEC_CMD
  CALL _LCD_CLEAR 
  CALL _LCD_RESET_CURSOR
  POP HL     ;; Restore HL value
  RET

_LCD_CLEAR:
  LD A, LCD_CLEAR ;; Load LCD RESET CMD
  CALL #LCD_EXEC_CMD
  RET

_LCD_RESET_CURSOR:
  LD HL, PTR_LCD_CURSOR ;; Load ptr to LCD CURSOR value
  LD (HL), 0x00 ;; Reset cursor in memory
  LD A, LCD_RETURN ;; Load RESET LCD CURSOR
  CALL #LCD_EXEC_CMD
  RET

_LCD_CURSOR_MOVE_TO:
  LD HL, PTR_LCD_CURSOR ;; Load ptr to LCD CURSOR value
  LD B, (HL) ;; Load amount of offset
  LD A, LCD_RETURN ;; Load RESET LCD CURSOR
  CALL #LCD_EXEC_CMD
_LCD_CURSOR_MOVE_TO_lp:
  DJNZ _LCD_CURSOR_MOVE_TO_esc-$
  LD A, LCD_CURSOR_RIGHT ;; Load RESET LCD CURSOR
  CALL #LCD_EXEC_CMD
  JR _LCD_CURSOR_MOVE_TO_lp-$
_LCD_CURSOR_MOVE_TO_esc:
  RET

_LCD_CURSOR_MOVE_FROM:
  POP HL     ;; Get return addr
  EX (SP), HL;; Restore return addr & Get offset H - row, L - column
  ;; TODO:
  
_LCD_PRINTF:
  ; TODO: Change printf
