#include "Defs.asm"

ORG 0x0000
  XOR A             ; Reset reg A
  OUT (INT_PORT), A ; Disable all interrupts
  LD SP, STACK      ; Set Memory Paging RAM
  JR RST_INIT-$     ; Jump to the hardware SETUP

ORG 0x0008
RST08:       ; Sets enable/disable word for Hardware devices
  IN A, (PPI_PORT_C) ; Get current state of PPI reg C
  AND C      ; Apply mask that will clear flag 
  XOR B      ; Apply mask that will set or reset flags
  OUT (PPI_PORT_C), A ; Send updated reg to PPI
  RET

ORG 0x0010
RST10:
  JP #LCD_OUT

;; NOTE: This 6 bytes can be used

ORG 0x0018
RST18:       ;; aka PRINT
  LD A, (HL) ;; Get curr char
  OR A       ;; Check if line is ended (Set flag Z)
  RET Z      ;; Return if str is ended 
  INC HL     ;; Inc arg pointer
  RST 0x10   ;; Output the char
  JR RST18-$

ORG 0x0020
RST20:       ;; aka SOUND
  JP #SOUND_OUT

;; NOTE: This 6 bytes can be used

;;
;; Interrupt handler
;;
;; func RST38() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
ORG 0x0038
RST38:
  DI          ; Disable interrupt from triggering
  PUSH AF     ; Save Acc & flags
  PUSH BC     ; Save reg BC
  PUSH HL     ; Save reg HL

  LD HL, .IM_VEC_ST; Load ptr to interrupt exec
  IN A, (INT_PORT); Get interrupt vector, 0 is where interrupt occured
  CPL         ; Invert Acc for easier work
  LD C, A     ; Save interrupt vector in reg C
  LD B, IM_VEC_SIZE; Load amount of avaliable interrupt exec

RST38_lp:
  LD A, (HL)  ; Get the bit map of the interrupt
  AND C       ; Check if current interrupt has occured
  INC HL      ; Move ptr of interrupt exec high byte addr
  LD A, (HL)  ; Get interrupt exec high byte addr
  INC HL      ; Move ptr of interrupt exec low byte addr
  PUSH HL     ; Save ptr of interrupt exec in stack
  LD L, (HL)  ; Get interrupt exec low byte addr
  JR Z, RST38_lp_end-$

  LD H, A     ; Complete in reg HL interrupt exec addr
  PUSH HL     ; Temp save interrupt exec addr in stack
  LD HL, RST38_lp_end; Load custom return addr
  EX (SP), HL ; Manually create CALL and load in reg HL interrupt exec addr
  JP (HL)     ; Jump to the interrupt execution

RST38_lp_end:
  POP HL      ; Restore ptr of interrupt exec
  INC HL      ; Move ptr of interrupt exec to the next masked byte
  DJNZ RST38_lp-$

  POP HL      ; Restore reg HL 
  POP BC      ; Restore reg BC
  POP AF      ; Restore reg AF
  EI          ; Restore interrupts
RST38_end:
  RET

;;
;; Example:
;;  JP #RST_INIT; Initialize hardware to default settings
;;
;; func RST_INIT() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
RST_INIT:
  LD A, 0x80  ; Set MODE 0;  A: OUTPUT; B: OUTPUT; C: OUTPUT
  OUT (PPI_PORT_CTRL), A ; Send instruction to PPI

  XOR A       ; Reset reg Acc
  OUT (PPI_PORT_A), A ; Reset PPI reg A
  OUT (PPI_PORT_B), A ; Reset PPI reg B, (MMU = 0)
  OUT (PPI_PORT_C), A ; Reset PPI reg C

  LD A, ALLOWED_INTERUPTS
  OUT (INT_PORT), A

  CALL SETUP  ; Jump to program setup

  LD A, EVENT_PRIO_IDLE; Set cmd exec as an idle task
  LD HL, MAIN; Load task to be an entrance point to the program
  CALL _EVENT_PUSH; Add buffer updates to task queue

  IM 1        ; Use interrupt Mode 1
  EI          ; Enable interrupts
  JP #EVENT_LOOP; Jump into task queue execution


.IM_VEC_ST:
  db IM_KEYBOARD, #SCAN_CODE_IM[2]
  db IM_RxRDY,    #CMD_CLEAR[2] ;; FIXME
  db IM_TxRDY,    #CMD_CLEAR[2] ;; FIXME
  db IM_RLT,      #CMD_CLEAR[2] ;; FIXME
  db IM_CT1,      #CMD_CLEAR[2] ;; FIXME
  db IM_CT2,      #CMD_CLEAR[2] ;; FIXME
  db IM_NONE1,    RST38_end[2]
  db IM_NONE2,    RST38_end[2]
.IM_VEC_ED:

;; Variables
IM_VEC_SIZE          EQU  8