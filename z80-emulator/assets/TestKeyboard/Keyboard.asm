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
;;  LD A, 0x0E
;;  CALL #SCAN_CODE_ASCII
;;
;; proc SCAN_CODE_ASCII() -> reg A;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#SCAN_CODE_ASCII:
  PUSH HL     ; Save HL reg in stack
  PUSH BC     ; Save BC reg in stack
  LD BC, SCAN_CODE_ASCII_VEC_SIZE; Get vector size
  LD HL, .SCAN_CODE_ASCII_VEC_ST
  PUSH HL     ; Save scan code vector ptr
  CPIR        ; Find index in SCAN_CODE_ASCII
  POP BC      ; Restore scan code vector ptr to reg BC
  JP PO, #SCAN_CODE_ASCII_bg
  XOR A       ; Reset Acc
  JR #SCAN_CODE_ASCII_esc-$

#SCAN_CODE_ASCII_bg:
  XOR A       ; Reset Acc & flags
  SBC HL, BC  ; Get offset from the start of vector
  LD A, (KEY_SHIFT) ; Get shift status
  DEC A       ; Check if shift key has stat pressed
  JR Z, #SCAN_CODE_ASCII_up-$
  DEC A       ; Check if shift key has stat hold
  JR NZ, #SCAN_CODE_ASCII_lw-$
#SCAN_CODE_ASCII_up:
  LD BC, .UPPERCASE_VEC_ST
  JR #SCAN_CODE_ASCII_lw_ed-$

#SCAN_CODE_ASCII_lw:
  LD BC, .LOWERCASE_VEC_ST
#SCAN_CODE_ASCII_lw_ed:
  ADD HL, BC  ; ASCII char location
  LD A, (HL)  ; Get ASCII char

#SCAN_CODE_ASCII_esc:
  POP BC      ; Restore BC reg
  POP HL      ; Restore HL reg
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
  PUSH BC     ; Save BC reg in stack
  LD C, A     ; Save in reg C scan code
  CP 0xF0     ; Check if key is been released
  JR Z, #SCAN_CODE_IM_esc-$

  LD HL, SCAN_KEY_MAP ;; Load scan code mapper area
  LD L, A     ; Load offset 
  LD A, (HL)  ; Get prev state of key
  OR A        ; Set flag Z if empty (state 000 = NULL)
  LD B, 0b001 ; Set state PRESSED if curr == 000
  JR Z, #SCAN_CODE_cmp_st-$
  DEC A       ; Check if state is 001 = Pressed, flag Z will be set
  LD B, 0b010 ; Set state HOLD if curr == 001
  JR Z, #SCAN_CODE_cmp_st-$
  DEC A       ; Check if state is 010 = Hold, flag Z will be set
  LD B, 0b010 ; Set state HOLD if curr == 010
  JR Z, #SCAN_CODE_cmp_st-$
  DEC A       ; Check if state is 011 = Released, flag Z will be set
  LD B, 0b001 ; Set state NULL if key isn't been nulled
  ; JR #SCAN_CODE_nxt_st-$

#SCAN_CODE_cmp_st:
  LD A, (PTR_PREV_SCAN_CODE); Get prev value from ptr
  CP 0xF0     ; Check if key is been released
  JR Z, #SCAN_CODE_st_rels-$

  PUSH HL     ; Save HL reg in stack
  LD HL, SCAN_KEY_BUF ; Load buffer count addr, (aka offset)
  LD L, (HL)  ; Load buf offset
  LD A, C     ; Restore scan code to reg A
  CALL #SCAN_CODE_ASCII
  OR A        ; Check if ASCII was found
  JR Z, #SCAN_CODE_cmp_buf_esc-$
  LD (HL), A  ; Save scan code in buffer
  LD HL, SCAN_KEY_BUF ; Load buffer count addr, (aka offset)
  XOR A       ; Reset reg A
  RRD         ; Load to reg A only low bit
  INC A       ; Inc offset by one
  AND 0x0F    ; Get low bits
  JR NZ, #SCAN_CODE_cmp_buf_ed-$
  INC A       ; Start offset from 1
#SCAN_CODE_cmp_buf_ed:
  RLD         ; Restore offset value, but incr it by one
#SCAN_CODE_cmp_buf_esc:
  POP HL      ; Restore HL reg
  JR #SCAN_CODE_nxt_st-$

#SCAN_CODE_st_rels:
  LD B, 0b011 ; Set state RELEASED if curr == 001 AND prev scan code == 0xF0
#SCAN_CODE_nxt_st:
  LD (HL), B  ; Save key state

#SCAN_CODE_IM_esc:
  LD A, C     ; Restore from scan code from reg C
  LD (PTR_PREV_SCAN_CODE), A ; Save curr key in prev ptr
  POP BC      ; Restore BC reg
  POP HL      ; Restore HL reg
  RET

.SCAN_CODE_ASCII_VEC_ST:
  db 0x0E, 0x16, 0x1E, 0x26, 0x25, 0x2E, 0x36, 0x3D, 0x3E, 0x46, 0x45
  db 0x4E, 0x55, 0x15, 0x1D, 0x24, 0x2D, 0x2C, 0x35, 0x3C, 0x43, 0x44
  db 0x4D, 0x54, 0x5B, 0x5D, 0x1C, 0x1B, 0x23, 0x2B, 0x34, 0x33, 0x3B
  db 0x42, 0x4B, 0x4C, 0x1A, 0x22, 0x21, 0x2A, 0x32, 0x31, 0x3A, 0x41
  db 0x49, 0x4A, 0x29, 0x0D, 0x52 
.SCAN_CODE_ASCII_VEC_ED:

.LOWERCASE_VEC_ST:
  db "`1234567890-=qwertyuiop[]\asdfghjkl;zxcvbnm,./ ", 9, 39

.UPPERCASE_VEC_ST:
  db "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:ZXCVBNM<>? ", 9, 34


;; Variables
SCAN_CODE_ASCII_VEC_SIZE        EQU  .SCAN_CODE_ASCII_VEC_ED-.SCAN_CODE_ASCII_VEC_ST