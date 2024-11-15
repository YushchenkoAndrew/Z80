#include "Defs.asm"

ORG 0x0000
  XOR A             ; Reset reg A
  OUT (INT_PORT), A ; Disable all interrupts
  LD SP, STACK      ; Set Memory Paging RAM
  JR SETUP-$        ; Jump to the program SETUP

ORG 0x0008
RST8:
  ; TODO: Think about this
  CALL _ACK_BUFFERS; Check and reset state of the buf
  EI          ; Restore interrupts
  RET
  

ORG 0x0010
RST10:
  CALL #LCD_OUT
  RET


ORG 0x0018
RST18:       ;; aka PRINT
  LD A, (HL) ;; Get curr char
  OR A       ;; Check if line is ended (Set flag Z)
  RET Z      ;; Return if str is ended 
  INC HL     ;; Inc arg pointer
  RST 0x10   ;; Output the char
  JR RST18-$

;;
;; Interrupt handler
;;
;; func _IM_EXEC() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
ORG 0x0038
_IM_EXEC:
  DI          ; Disable interrupt from triggering
  PUSH AF     ; Save Acc & flags
  PUSH BC     ; Save reg BC
  PUSH HL     ; Save reg HL

  LD HL, .IM_VEC_ST; Load ptr to interrupt exec
  IN A, (INT_PORT); Get interrupt vector, 0 is where interrupt occured
  CPL         ; Invert Acc for easier work
  LD C, A     ; Save interrupt vector in reg C
  LD B, IM_VEC_SIZE; Load amount of avaliable interrupt exec

_IM_EXEC_lp:
  LD A, (HL)  ; Get the bit map of the interrupt
  AND C       ; Check if current interrupt has occured
  INC HL      ; Move ptr of interrupt exec high byte addr
  LD A, (HL)  ; Get interrupt exec high byte addr
  INC HL      ; Move ptr of interrupt exec low byte addr
  PUSH HL     ; Save ptr of interrupt exec in stack
  LD L, (HL)  ; Get interrupt exec low byte addr
  JR Z, _IM_EXEC_lp_end-$

  LD H, A     ; Complete in reg HL interrupt exec addr
  PUSH HL     ; Temp save interrupt exec addr in stack
  LD HL, _IM_EXEC_lp_end; Load custom return addr
  EX (SP), HL ; Manually create CALL and load in reg HL interrupt exec addr
  JP (HL)     ; Jump to the interrupt execution

_IM_EXEC_lp_end:
  POP HL      ; Restore ptr of interrupt exec
  INC HL      ; Move ptr of interrupt exec to the next masked byte
  DJNZ _IM_EXEC_lp-$

  POP HL      ; Restore reg HL 
  POP BC      ; Restore reg BC
  POP AF      ; Restore reg AF
  RET


.IM_VEC_ST:
  db 0x80, #SCAN_CODE_IM[2]
  db 0x40, #CMD_CLEAR[2]
  db 0x20, #CMD_CLEAR[2]
  db 0x10, #CMD_CLEAR[2]
  db 0x08, #CMD_CLEAR[2]
  db 0x04, #CMD_CLEAR[2]
  db 0x02, #CMD_CLEAR[2]
  db 0x01, #CMD_CLEAR[2]
.IM_VEC_ED:

;; Variables
IM_VEC_SIZE          EQU  8