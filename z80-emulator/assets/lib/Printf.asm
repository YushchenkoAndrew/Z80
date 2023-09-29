#include "Defs.asm"

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
;; func PRINTF(str*, word, ...args) -> void;
;;   str* -- ptr to string
;;   word -- low byte is port to send chars
;;   args -- Additional variables
;;           %c - low byte of the word
;;           %s - ptr to another str
;;           %x - word
;;           %d - TODO:
_PRINTF:
  POP DE     ;; Get return addr
  POP HL     ;; Get string addr
  POP BC     ;; Get output port

_PRINTF_lp:
  LD A, (HL) ;; Get curr char
  INC HL     ;; Inc string pointer
  OR A       ;; Check if line is ended (Set flag Z)
  JR Z, _PRINTF_esc-$
  CP "%"     ;; Check if char need to be replaced
  JR Z, _PRINTF_chg-$
_PRINTF_out:
  OUT (C), A ;; Display char
  JR _PRINTF_lp-$

_PRINTF_chg:
  LD A, (HL) ;; Get char parametter
  INC HL     ;; Inc string pointer

_PRINTF_chg_c:
  CP "c"     ;; Check if parm is char
  JR NZ, _PRINTF_chg_s-$ 
  EX (SP), HL;; Save string addr & Get char
  OUT (C), L ;; Display char
  POP HL     ;; Restore string addr
  JR _PRINTF_lp-$

_PRINTF_chg_s:
  CP "s"     ;; Check if parm is string
  JR NZ, _PRINTF_chg_d-$
  EX (SP), HL;; Next string addr and save curr
  PUSH DE    ;; Save return addr
  LD DE, _PRINTF_chg_s_esc ;; Load to reg DE return addr
  JR _PRINTF_lp-$
_PRINTF_chg_s_esc:
  POP DE     ;; Restore return addr
  POP HL     ;; Restore string addr
  JR _PRINTF_lp-$

_PRINTF_chg_d:
  CP "d"     ;; Check if parm is decimal
  JR NZ, _PRINTF_chg_x-$
  EX (SP), HL;; Save string addr & Get char
  PUSH HL    ;; Move back number
  LD HL, 0x00;; Reset reg HL
  ADD HL, SP ;; Load curr stack address

  LD A, (HL) ;; Get low byte
  DAA        ;; Convert to decimal
  LD B, A
  INC HL
  LD A, (HL) ;; Get low byte
  ADC A, 0
  DAA        ;; Convert to decimal
  LD H, A
  LD L, B
  PUSH HL

  OUT (C), L ;; Display char
  POP HL     ;; Restore string addr
  JR _PRINTF_lp-$
  ; ADD 0x00  ;; Add 0 to reg A only to set flags
  ; DAA       ;; Convert reg A from hex to number
  ; TODO: impl this with DAA

_PRINTF_chg_x:
  CP "x"
  JR  NZ, _PRINTF_out-$
  EX (SP), HL;; Save string addr & Get number
  PUSH HL    ;; Move back number
  LD HL, 0x00;; Reset reg HL
  ADD HL, SP ;; Load curr stack address
  INC HL     ;; Move HL to high byte num val

  LD B, 0x02 ;; Total amount of bytes to display
_PRINTF_chg_x_lp_init:
  PUSH BC    ;; Save curr reg B index (aka how much bytes to display)
  LD B, 0x02 ;; Aka display high & low bits
_PRINTF_chg_x_lp:
  XOR A      ;; Reset reg A
  RLD        ;; Load hight bits from (HL) -> A
  ADD A, 0x90;; Acc = 90h - 9Fh
  DAA        ;; Acc = 90h - 99h OR 00h - 15h
  ADC A, 0x40;; Add char 'A'
  DAA        ;; Acc = 30h - 39h (aka ascii 0 - 9) OR 40h - 45h (aka ascii 'A' - 'F')
  OUT (C), A ;; Display char
  DJNZ _PRINTF_chg_x_lp-$
_PRINTF_chg_x_lp_esc:
  DEC HL     ;; Get next byte
  POP BC     ;; Restore reg B total loop cnt
  DJNZ _PRINTF_chg_x_lp_init-$

  POP HL     ;; Get number, now this is useless
  POP HL     ;; Restore string addr
  JR _PRINTF_lp-$

_PRINTF_esc:  
  PUSH DE    ;; Restore return addr
  RET
