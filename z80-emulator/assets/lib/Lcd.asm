#include "Defs.asm"

;;
;; Example:
;;  LD A, "t"
;;  CALL #LCD_EXEC
;; 
;; proc LCD_EXEC() -> void;
;;   reg A  -- as defined
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
#LCD_EXEC:
  ; PUSH AF          ;; Save reg A in stack
  ; LD A, 0          ;; Disable every IO device
  ; OUT (PPI_PORT_C), A
  ; POP AF           ;; Restore reg A
  ;; TODO: Update to support PPI
  OUT (0x20), A
  ; LD A, LCD_IO_CMD ;; Enable output to LCD in CMD mode
  ; OUT (PPI_PORT_C), A
  RET


;;
;; Example:
;;  LD A, "t"
;;  CALL #LCD_WR
;; 
;; proc LCD_WR() -> void;
;;   reg A  -- as defined
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
#LCD_WR:
  ;; TODO: Update to support PPI
  OUT (0x21), A
  ; LD A, LCD_IO_CMD ;; Enable output to LCD in CMD mode
  ; OUT (PPI_PORT_C), A
  RET


;;
;; Example:
;;  CALL _LCD_INIT
;; 
;; func LCD_INIT() -> void;
;;   reg A  -- unaffected
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
_LCD_INIT:
  PUSH AF    ; Save reg A in stack
  LD A, LCD_DEFAULT_MODE;; Load default LCD MODE value
  CALL #LCD_EXEC
  LD A, LCD_DEFAULT_DISPLAY ;; Load default LCD DISPLAY CMD
  CALL #LCD_EXEC
  CALL _LCD_CLEAR 
  POP AF     ; Restore reg A & flags
  RET

;;
;; Example:
;;  CALL _LCD_CLEAR
;; 
;; func LCD_CLEAR() -> void;
;;   reg A  -- unaffected
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
_LCD_CLEAR:
  PUSH AF    ; Save reg A in stack
  LD A, LCD_CLEAR ;; Load LCD RESET CMD
  CALL #LCD_EXEC
  LD A, LCD_RETURN ;; Load RESET LCD CURSOR
  CALL #LCD_EXEC
  XOR A      ; Reset reg A
  LD (PTR_LCD_CURSOR), A; Reset cursor pos to x: 0 y: 0
  POP AF     ; Restore reg A & flags
  RET


;;
;; Example:
;;  LD A, "t"
;;  CALL #LCD_OUT
;; 
;; proc LCD_OUT() -> void;
;;   reg A  -- as defined
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
#LCD_OUT:
  CP BACKSPACE; Check if char is backspace
  JR Z, _LCD_BACKSPACE-$
  CP LINE_FEED; Check if char is "\n"
  JR Z, _LCD_NEW_LINE-$
  CP CARRY_RET; Check if char is "\r"
  JR Z, _LCD_LINE_ST-$
  CP FORM_FEED; Check if char aka is screen clear
  JR Z, _LCD_CLEAR-$

  PUSH HL    ; Save reg HL in stack
  LD HL, PTR_LCD_CURSOR; Load cursor pos ptr
  INC (HL)   ; Move cursor pos to the right by one
  POP HL     ; Restore reg HL
  JP #LCD_WR ; Output the char


;;
;; Example:
;;  CALL _LCD_BACKSPACE
;; 
;; func LCD_BACKSPACE() -> void;
;;   reg A  -- unaffected
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
_LCD_BACKSPACE:
  PUSH AF    ; Save reg A in stack
  LD A, LCD_CURSOR; Move cursor to the left by one
  CALL #LCD_EXEC
  LD A, " "  ; Manualy make deleted char invisible
  CALL #LCD_WR
  LD A, LCD_CURSOR; Move cursor to the left by one, one more time
  CALL #LCD_EXEC
  POP AF     ; Restore reg A & flags
  RET


;;
;; Example:
;;  CALL _LCD_NEW_LINE
;; 
;; func LCD_NEW_LINE() -> void;
;;   reg A  -- unaffected
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
_LCD_NEW_LINE:
  PUSH AF    ; Save reg A in stack
  PUSH HL    ; Save reg HL in stack
  LD HL, PTR_LCD_CURSOR; Load cursor pos ptr
  LD A, (HL) ; Get current cursor pos
  AND 0x0F   ; Get only low bites, aka X axios
  JR Z, _LCD_NEW_LINE_end-$; If value is 0 aka cursor at the start of new line, do nothing
  NEG        ; Calc amount of right shift to do
  AND 0x0F   ; Get only low bites, aka X axios
  PUSH BC    ; Save reg B in stack
  LD B, A    ; Load counter to the reg B
  LD A, " "  ; Manualy make deleted char invisible
_LCD_NEW_LINE_lp:
  INC (HL)   ; Move cursor pos to the right by one
  CALL #LCD_WR; Output empty char
  DJNZ _LCD_NEW_LINE_lp-$
  POP BC     ; Restore reg B
_LCD_NEW_LINE_end:
  POP HL     ; Restore reg HL
  POP AF     ; Restore reg A & flags
  RET


;;
;; Example:
;;  CALL _LCD_LINE_ST
;; 
;; func LCD_LINE_ST() -> void;
;;   reg A  -- unaffected
;;   reg B  -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
;;
_LCD_LINE_ST:
  PUSH AF    ; Save reg A in stack
  PUSH HL    ; Save reg HL in stack
  LD HL, PTR_LCD_CURSOR; Load cursor pos ptr
  LD A, (HL) ; Get current cursor pos
  AND 0x0F   ; Get only low byte, aka X axios
  JR Z, _LCD_LINE_ST_end; If value is 0 aka cursor at the start of new line, do nothing
  PUSH BC    ; Save reg B in stack
  LD B, A    ; Load counter to the reg B
  LD A, LCD_CURSOR; Move cursor to the left by one
_LCD_LINE_ST_lp:
  DEC (HL)   ; Move cursor pos to the left by one
  CALL #LCD_EXEC; Exec shift command
  DJNZ _LCD_LINE_ST_lp-$
  POP BC     ; Restore reg B
_LCD_LINE_ST_end:
  POP HL     ; Restore reg HL
  POP AF     ; Restore reg A & flags
  RET
  

;; Commands bit
LCD_CLEAR           EQU 0x01
LCD_RETURN          EQU 0x02

LCD_MODE            EQU 0x04
LCD_MODE_INC        EQU 0x02
LCD_MODE_SHIFT      EQU 0x01

LCD_DISPLAY         EQU 0x08
LCD_DISPLAY_LIGHT   EQU 0x04
LCD_DISPLAY_CURSOR  EQU 0x02
LCD_DISPLAY_BLINK   EQU 0x01

LCD_CURSOR          EQU 0x10
LCD_CURSOR_SHIFT    EQU 0x08
LCD_CURSOR_RIGHT    EQU 0x04

LCD_FUNCTION        EQU 0x20
LCD_FUNCTION_LENGTH EQU 0x10
LCD_FUNCTION_LINES  EQU 0x08
LCD_FUNCTION_FONT   EQU 0x04

LCD_CGRAM           EQU 0x40
LCD_DDRAM           EQU 0x80


;; Default init state
LCD_DEFAULT_MODE    EQU LCD_MODE | LCD_MODE_INC | LCD_MODE_SHIFT
LCD_DEFAULT_DISPLAY EQU LCD_DISPLAY | LCD_DISPLAY_CURSOR | LCD_DISPLAY_BLINK