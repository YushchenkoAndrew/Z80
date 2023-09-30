#include "Utils.asm"

;;
;; Example:
;;  LD BC, 0x000A
;;  PUSH BC
;;
;;  LD BC, 0x20 ;; PORT
;;  PUSH BC
;;
;;  LD BC, number ;; string
;;  PUSH BC
;;
;;  CALL _PRINTF
;;
;; number:
;;   db "Number: '%x'", 0
;;
;;
;; func PRINTF(str*, word, ...args) -> byte*;
;;   str* -- ptr to string
;;   word -- low byte is port to send chars
;;   args*-- ptr to args 
;;           %c - low byte of the word
;;           %s - ptr to another str
;;           %x - display byte in HEX
;;           %X - display word in HEX
;;           %d - display dec 00-99
_PRINTF:
  POP HL     ;; Get return addr
  POP DE     ;; Get string addr
  POP BC     ;; Get output port
  EX (SP), HL;; Restore return arg & Get ptr to args
  EX DE, HL  ;; Move ptr to args in reg DE & reg HL has string ptr

  XOR A                  ;; Reset reg A
  LD (PTR_PRINTF_RET), A ;; Reset string length

_PRINTF_lp:
  LD A, (HL) ;; Get curr char
  INC HL     ;; Inc string pointer
  OR A       ;; Check if line is ended (Set flag Z)
  RET Z      ;; Return when char is '\0'
  CP "%"     ;; Check if char need to be replaced
  JR Z, _PRINTF_chg-$
_PRINTF_out:
  OUT (C), A ;; Display char
  CALL #PRINTF_LEN_ADD1
  JR _PRINTF_lp-$

_PRINTF_chg:
  LD A, (HL) ;; Get char parametter
  INC HL     ;; Inc string pointer
  EX DE, HL  ;; Load in reg HL ptr to args

_PRINTF_chg_c:
  CP "c"     ;; Check if parm is char
  JR NZ, _PRINTF_chg_s-$ 
  LD A, (HL) ;; Get char from a ptr
  OUT (C), A ;; Display char
  CALL #PRINTF_LEN_ADD1
  INC HL     ;; Move ptr by 1 byte
  JR _PRINTF_chg_esc-$

_PRINTF_chg_s:
  CP "s"     ;; Check if parm is string
  JR NZ, _PRINTF_chg_d-$
_PRINTF_chg_s_lp:
  LD A, (HL) ;; Get curr char
  INC HL     ;; Inc arg pointer
  OR A       ;; Check if line is ended (Set flag Z)
  JR Z, _PRINTF_chg_esc-$
  OUT (C), A ;; Display char
  CALL #PRINTF_LEN_ADD1
  JR _PRINTF_chg_s_lp-$

_PRINTF_chg_d:
  CP "d"     ;; Check if parm is decimal
  JR NZ, _PRINTF_chg_X-$
  LD A, (HL) ;; Load to Acc byte which will be converted to BCD
  CALL #BIN_BCD
  LD (HL), A ;; Save in ptr result of BCD
  CALL #HEX_ASCII
  CALL #PRINTF_LEN_ADD2
  INC HL     ;; Move ptr by 1 byte
  JR _PRINTF_chg_esc-$

_PRINTF_chg_X:
  CP "X"
  JR  NZ, _PRINTF_chg_x-$
  LD B, 0x02 ;; Total amount of bytes to display
  JR _PRINTF_chg_x_lp-$

_PRINTF_chg_x:
  CP "x"
  JR  NZ, _PRINTF_chg_esc-$
  LD B, 0x01 ;; Total amount of bytes to display
_PRINTF_chg_x_lp:
  PUSH BC    ;; Save curr reg B index (aka how much bytes to display)
  CALL #HEX_ASCII
  CALL #PRINTF_LEN_ADD2
_PRINTF_chg_x_lp_esc:
  INC HL     ;; Get next byte
  POP BC     ;; Restore reg B total loop cnt
  DJNZ _PRINTF_chg_x_lp-$

_PRINTF_chg_esc:
  EX DE, HL   ;; Restore string addr & arg ptr
  CP "%"
  JR Z, _PRINTF_out-$
  JR _PRINTF_lp-$


#PRINTF_LEN_ADD1:
  LD B, 0x01
  JR #PRINTF_LEN-$
#PRINTF_LEN_ADD2:
  LD B, 0x02
#PRINTF_LEN:
  LD A, (PTR_PRINTF_RET) ;; Get length of the string
  ADD A, B
  LD (PTR_PRINTF_RET), A ;; Save length
  RET