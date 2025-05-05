#include "Defs.asm"

ORG 0x0000
  XOR A             ; Reset reg A
  OUT (INT_PORT), A ; Disable all interrupts
  LD SP, STACK_ADDR ; Set Memory Paging RAM
  JR RST_INIT-$     ; Jump to the hardware SETUP

ORG 0x0008
RST08:       ;; Sets enable/disable word for Hardware devices
  IN A, (PPI_PORT_C);; Get current state of PPI reg C
  AND C      ;; Apply mask that will clear flag 
  XOR B      ;; Apply mask that will set or reset flags
  OUT (PPI_PORT_C), A;; Send updated reg to PPI
  RET

ORG 0x0010
RST10:       ;; aka SHOW CHAR
  ;; TODO: Populate
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
RST20:       ;; aka LOAD WORD
  LD A, (HL) ;; Load the low byte to Acc
  INC HL     ;; Move ptr to the high byte
  LD H, (HL) ;; Load the high byte to reg L
  LD L, A    ;; Complete word in reg HL
  RET


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
  CALL _INTR_LOCK; Lock interrupt from happening again
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
  INC HL      ; Move ptr of interrupt exec low byte addr
  LD A, (HL)  ; Get interrupt exec low byte addr
  INC HL      ; Move ptr of interrupt exec high byte addr
  PUSH HL     ; Save ptr of interrupt exec in stack
  JR Z, RST38_lp_end-$

  LD H, (HL)  ; Get interrupt exec high byte addr
  LD L, A     ; Complete in reg HL interrupt exec addr
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
  CALL _INTR_UNLOCK; Allow interrupt to happen
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

  LD (PTR_STDIN_BUF_BGN), A ; Reset stdin start of offset 
  LD (PTR_STDIN_BUF_END), A ; Reset stdin end of offset 
  LD (PTR_FILE_INODE), A; Reset interrupt lock counter
  LD (PTR_DIR_INODE), A; Reset interrupt lock counter
  LD (PTR_INTR_BYTE), A; Reset interrupt workd
  LD (PTR_INTR_LOCK), A; Reset interrupt lock counter
  LD (TASK_BUF), A; Reset task amount in queue
  LD (CT1_BUF), A ; Reset task amount in queue CT1
  LD (CT2_BUF), A ; Reset task amount in queue CT2

  CALL _TIMER_INIT ; Initialize timer
  CALL _RTC_INIT   ; Initialize Real Time Clock

  LD H, A     ; Reset reg H
  LD L, ALLOWED_INTERUPTS; Hardcoded allowed interrupts
  CALL #INTR_MASK_SET; Send allowed intrrupts to interrupt vector

  IM 1        ; Use interrupt Mode 1
  CALL _INTR_LOCK; Lock interrupt from happening again

  LD A, PIT_PITCH_6 | PIT_NOTE_E; Set note E in 6 pitch to Acc
  CALL #SOUND_OUT; Produce the sound

  LD A, 2      ; Hold the sound for 0.5 second
  LD DE, _SOUND_OFF; Function to run after exec
  LD HL, PIT_FREQ_10Hz; Set CT1 to freq 10Hz
  CALL #TIMER_PUSH; Add task to disable sound to timer queue

  LD A, EVENT_PRIO_IDLE; Set cmd exec as an idle task
  LD HL, MAIN; Load task to be an entrance point to the program
  CALL _EVENT_PUSH; Add buffer updates to task queue

  CALL _INTR_UNLOCK; Allow interrupt to happen

  CALL SETUP  ; Jump to program setup
  JP #EVENT_LOOP; Jump into task queue execution

;;
;; Example:
;;  LD H, A     ; Reset reg H
;;  LD L, ALLOWED_INTERUPTS; Hardcoded allowed interrupts
;;  CALL #INTR_MASK_SET; Turn off interrupt programly
;;
;; proc INTR_MASK_SET() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#INTR_MASK_SET:
  PUSH AF    ; Save reg AF in stack
  LD A, (PTR_INTR_BYTE); Get current allowed interrupt byte
  AND H      ; Apply mask that will clear flag 
  XOR L      ; Apply mask that will set or reset flags
  LD (PTR_INTR_BYTE), A; Save updated allowed interrupts
  OUT (INT_PORT), A; Send allowed intrrupts to interrupt vector
  POP AF     ; Restore reg AF
  RET

;; Example:
;;  CALL _INTR_LOCK; Locks interrupt from executing
;;
;; fun _INTR_LOCK() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_INTR_LOCK:
  DI         ; Disable interrupt programly
  PUSH HL    ; Save reg HL in stack
  LD HL, PTR_INTR_LOCK; Load ptr to the interrupt lock
  INC (HL)   ; Incr lock counter, aka shows how many times need to run unlock
  POP HL     ; Restore reg HL
  RET

;; Example:
;;  CALL _INTR_UNLOCK; Unlock interrupt, aka allow interrupt to happend again
;;
;; func INTR_MASK_SET() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_INTR_UNLOCK:
  PUSH HL    ; Save reg HL in stack
  LD HL, PTR_INTR_LOCK; Load ptr to the interrupt lock
  DEC (HL)   ; Decr lock counter, aka shows how many times need to run unlock
  JR NZ, _INTR_UNLOCK_end-$
  EI         ; Restore interrupt after counter reached zero

_INTR_UNLOCK_end:
  POP HL     ; Restore reg HL
  RET

;;
;; Example:
;;  LD A, 't'
;;  CALL #SCAN_CODE_ACK; Update STDIN index and 
;;
;; proc SCAN_CODE_ACK() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#SCAN_CODE_ACK:
  LD A, (PTR_STDIN_BUF_END); Load prev buf offset to Acc
  INC A       ; Inc offset by one
  LD (PTR_STDIN_BUF_END), A; Restore offset value, but incr it by one
  CALL _ACK_STDIN; Create task that will process stdin



;; Example:
;;  LD A, 't'
;;  CALL _ACK_STDIN; Check ASCII code, update stdin index & create file system exec proc if needed
;;
;; func ACK_BUFFER() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_ACK_STDIN:
  CALL _INTR_LOCK; Lock interrupt from happening again
  PUSH HL      ; Save reg HL in stack

  ; CP FORM_FEED ; Check if char was Ctrl+L
  CP END_OF_TXT; Check if char was Ctrl+C
  JR Z, _ACK_STDIN_erase; On Ctrl-C reset stdin buffer index
  CP LINE_FEED; Check if char was '\n'

  PUSH AF     ; Temp save ASCII code in stack

  LD HL, STDIN_BUF ; Load buffer count addr, (aka offset)
  LD A, (PTR_STDIN_BUF_END); Load buf offset to Acc
  LD L, A     ; Move buffer ptr to the end
  INC A       ; Inc stdin end offset
  LD (PTR_STDIN_BUF_END), A; Update stdin offset value

  POP AF      ; Restore ASCII code from stack
  LD (HL), A  ; Save scan code in buffer
  JR NZ, _ACK_STDIN_end-$; If the ASCII is '/n' the create filesystem proc to exec cmd

  LD A, EVENT_PRIO_IO; Set task priority as input/output one
  LD HL, _FS_EXEC; Load task to exec comands
  CALL _EVENT_PUSH; Add buffer updates to task queue
  JR NZ, _ACK_STDIN_end-$; If the ASCII is '/n' the create filesystem proc to exec cmd

_ACK_STDIN_erase:
  LD A, (PTR_STDIN_BUF_END); Get the stdin end offset
  LD (PTR_STDIN_BUF_BGN), A; Reset stdin start offset to be the same as end

_ACK_STDIN_end:
  POP HL       ; Restore reg HL from stack
  CALL _INTR_UNLOCK; Allow interrupt to happen
  RET


.IM_VEC_ST:
  db IM_KEYBOARD dw #SCAN_CODE_IM[2]
  db IM_RxRDY    dw RST38_end[2] ;; FIXME
  db IM_TxRDY    dw RST38_end[2] ;; FIXME
  db IM_RTC      dw #RTC_ALARM_EXEC[2]
  db IM_CT1      dw #TIMER_CT1_EXEC[2]
  db IM_CT2      dw #TIMER_CT2_EXEC[2]
  db IM_NONE1    dw RST38_end[2]
  db IM_NONE2    dw RST38_end[2]
.IM_VEC_ED:

;; Variables
IM_VEC_SIZE          EQU  8