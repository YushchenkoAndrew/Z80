#include "../lib/Defs.asm"

;;
;; Example:
;;  CALL _SCAN_CODE_INIT
;;
;; func SCAN_CODE_INIT() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_SCAN_CODE_INIT:
  PUSH HL     ; Save HL reg in stack
  PUSH BC     ; Save BC reg in stack
  LD B, 0     ; Set cnt to the max val aka 256
  LD HL, SCAN_KEY_MAP ;; Load scan code mapper area
_SCAN_CODE_INIT_lp:
  LD (HL), 0  ; Set key state to NULL
  INC HL      ; Go to the next addr
  DJNZ _SCAN_CODE_INIT_lp-$
  POP BC      ; Restore reg BC
  POP HL      ; Restore reg HL
  RET

;;
;; Example:
;;  IN A, (0x30) ; Reset RS-Trigger (Reset Initerrupt)
;;  CALL #SCAN_CODE_IM
;;
;;  States
;;  000 - NULL
;;  001 - PRESSED
;;  010 - HOLD
;;  011 - RELEASED
;;
;; proc SCAN_CODE_IM(...regs) -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#SCAN_CODE_IM:
  PUSH HL     ; Save HL reg in stack
  PUSH AF     ; Save scan code in stack
  CP 0xF0     ; Check if key is been released
  JR Z, #SCAN_CODE_IM_esc-$

  PUSH BC     ; Save BC reg in stack
  LD HL, SCAN_KEY_MAP ;; Load scan code mapper area
  LD L, A     ; Load offset 
  LD A, (HL)  ; Get prev state of key
  OR A        ; Set flag Z if empty (state 000 = NULL)
  LD B, 0b001 ; Set state PRESSED if curr == 000
  JR Z, #SCAN_CODE_nxt_st-$
  DEC A       ; Check if state is 001 = Pressed, flag Z will be set
  JR Z, #SCAN_CODE_cmp_st-$
  DEC A       ; Check if state is 010 = Hold, flag Z will be set
  JR Z, #SCAN_CODE_cmp_st-$
  DEC A       ; Check if state is 011 = Released, flag Z will be set
  LD B, 0b001 ; Set state PRESSED if key isn't been nulled
  JR #SCAN_CODE_nxt_st-$

#SCAN_CODE_cmp_st:
  LD B, 0b010 ; Set state HOLD if curr == 001 || curr == 010
  LD A, (PTR_PREV_SCAN_CODE); Get prev value from ptr
  CP 0xF0     ; Check if key is been released
  JR NZ, #SCAN_CODE_nxt_st-$
  INC B       ; Set state RELEASED if curr == 001 AND prev scan code == 0xF0
#SCAN_CODE_nxt_st:
  LD (HL), B  ; Save key state
  POP BC      ; Restore BC reg

#SCAN_CODE_IM_esc:
  POP AF      ; Restore scan code
  POP HL      ; Restore HL reg
  LD (PTR_PREV_SCAN_CODE), A ; Save curr key in prev ptr
  RET

#include "../lib/Hex.asm"

_SCAN_CODE_HANDLE:
  LD B, 0     ; Set cnt to the max val aka 256
  LD HL, SCAN_KEY_MAP ;; Load scan code mapper area
_SCAN_CODE_INIT_lp:
  LD A, (HL)  ; Load key state to Acc
  OR A        ; Check if state is NULL
  JR Z, _SCAN_CODE_INIT_nxt-$

  ; FIXME: quick check
  PUSH HL
  CP 0x01
  JR NZ, _SCAN_CODE_INIT_evt-$
  PUSH BC
  LD BC, 49
  LD A, L
  LD HL, .SCAN_CODE_ASCII_VEC_ST
  CPIR
  POP BC

  LD A, L
  LD HL, .LOWERCASE_VEC_ST
  LD L, A
  LD A, (HL)

  LD HL, PTR_FUNC_ARGS
  LD (HL), A
  PUSH HL
  CALL _HEX

_SCAN_CODE_INIT_evt:
  POP HL

_SCAN_CODE_INIT_nxt:
  INC HL      ; Go to the next addr
  DJNZ _SCAN_CODE_INIT_lp-$
  RET


ORG 0x0300
.SCAN_CODE_ASCII_VEC_ST:
  db 0x0E, 0x16, 0x1E, 0x26, 0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46, 0x45
  db 0x4E, 0x55, 0x15, 0x1D, 0x24, 0x2D, 0x2C, 0x35, 0x3C, 0x43, 0x44
  db 0x4D, 0x54, 0x5B, 0x5D, 0x1C, 0x1B, 0x23, 0x2B, 0x34, 0x33, 0x3B
  db 0x42, 0x4B, 0x4C, 0x1A, 0x22, 0x21, 0x2A, 0x32, 0x31, 0x3A, 0x41
  db 0x49, 0x4A, 0x29, 0x0D, 0x52 
.SCAN_CODE_ASCII_VEC_ED:

ORG 0x0400
.LOWERCASE_VEC_ST:
  db "`1234567890-=qwertyuiop[]\asdfghjkl;zxcvbnm,./ ", 9, 39
.LOWERCASE_VEC_ED:

.UPPERCASE_VEC_ST:
  db "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:ZXCVBNM<>? ", 9, 34
.UPPERCASE_VEC_ED: