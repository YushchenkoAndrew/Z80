#include "Defs.asm"

;;
;; Example:
;;  JP _TIMER_INIT; Initialize timers
;;
;; fun _TIMER_INIT() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_TIMER_INIT:
  PUSH AF    ; Temp save reg AF in stack
  LD A, PIT_RL_3 | PIT_MODE_3; Set up CT0 to create sqaure wave signal
  OUT (PIT_PORT_CTL), A; Send reg to PIT

  LD A, PIT_CS_1 | PIT_RL_3 | PIT_MODE_2; Setup CT1 to create interrupt signal
  OUT (PIT_PORT_CTL), A; Send conter configuration to PIT

  LD A, PIT_CS_2 | PIT_RL_3 | PIT_MODE_2; Setup CT2 to create interrupt signal
  OUT (PIT_PORT_CTL), A; Send conter configuration to PIT
  POP AF     ; Restore reg AF from stack
  RET

;;
;; Example:
;;  LD B, IM_CT1; Enable interrupt for CT1
;;  LD C, PIT_PORT_CT1
;;  LD DE, PIT_FREQ_10Hz; Set CT1 to freq 10Hz
;;  CALL #TIMER_WR; Create a interrupt CT1 signal
;;
;; proc TIMER_WR() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
#TIMER_WR:
  PUSH HL    ; Temp save reg HL in stack
  OUT (C), E ; Send counter low byte
  OUT (C), D ; Send counter high byte
  LD L, B    ; Save interrupt mask to reg H
  LD C, ~PPI_CS_CS1; Turn off counter clock
  LD B, PPI_CS_CS1; Turn on counter clock
  RST 0x08   ; Enable counter output
  LD A, L    ; Restore interrupt mask to Acc
  CPL        ; Make inverted mask to re-enable CT interrupt
  LD H, A    ; Re-enable interrupt for CT
  CALL #INTR_MASK_SET; Send allowed intrrupts to interrupt vector
  POP HL     ; Restore reg HL from stack
  RET

;;
;; Example:
;;  JP _SOUND_OFF; Turn off sound
;;
;; fun _SOUND_OFF() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_SOUND_OFF:
  PUSH BC    ; Save reg BC in stack
  PUSH AF    ; Save reg AF in stack
  LD B, 0x00 ; Do not enable anything
  LD C, ~PPI_CS_CS0; Turn off sound counter clock
  RST 0x08   ; Disable sound output
  POP AF     ; Restore reg AF
  POP BC     ; Restore reg BC from stack
  RET

;;
;; Sound Generator
;;
;;  A = 0010 0111
;;      |      \
;;    div   index of the note
;;
;; Example:
;;  LD A, 0x27
;;  JP #SOUND_OUT; Create a constant sqaure wave sound
;;
;; proc SOUND_OUT() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#SOUND_OUT:
  PUSH BC    ; Save reg BC in stack
  PUSH HL    ; Save reg HL in stack
  LD H, A    ; Temp save info about the note in reg H
  LD B, 0x00 ; Do not enable anything
  LD C, ~PPI_CS_CS0; Turn off sound counter clock
  LD A, H    ; Restore info about the note
  AND 0x0F   ; Check if sound need to be disabled
  JR Z, #SOUND_OUT_esc-$

  LD B, PPI_CS_CS0; Enable sound counter clock
  LD A, H    ; Restore info about the note
  PUSH BC    ; Save PPI settings in stack

  DEC A      ; Cacl valid note ofset
  LD B, A    ; Save info about note in the reg B
  LD HL, .NOTE_BLOCK; Load ptr to the start of Note Block
  AND 0x0F   ; Get only the index from note info
  ADD A, A   ; Multiply offset by 2
  ADD A, L   ; Calculate ptr offset for note
  LD L, A    ; Save calculated ptr offset in reg HL
  JR NC, #SOUND_OUT_bgn-$
  INC H      ; Make adjustments on carry bit
#SOUND_OUT_bgn:
  RST 0x20   ; Completelly load note to the reg HL
  LD A, B    ; Restore full info about note
  AND 0xF0   ; Get the note div part
  JR Z, #SOUND_OUT_end-$

#SOUND_OUT_lp:
  RR H       ; Divide high byte of the note by 2
  RR L       ; Divide low byte of the note by 2
  SUB 0x10   ; Decriment division part by 1
  JR NZ, #SOUND_OUT_lp-$
  
#SOUND_OUT_end:
  LD A, L    ; Load the final value of note low byte 
  OUT (PIT_PORT_CT0), A; Send counter low byte
  LD A, H    ; Load the final value of note high byte
  OUT (PIT_PORT_CT0), A; Send counter high byte
  POP BC     ; Restore PPI settings from stack

#SOUND_OUT_esc:
  RST 0x08   ; Disable sound output
  POP HL     ; Restore reg HL
  POP BC     ; Restore reg BC from stack
  RET


;;
;; Example:
;;  CALL _TIMER_CT1_OFF; Turn off CT1 interrupt
;;
;; func TIMER_CT1_OFF() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_TIMER_CT1_OFF:
  PUSH HL    ; Save reg HL in stack
  LD H, ~IM_CT1; Disable interrupt for CT1
  JR _TIMER_CT2_OFF_bgn-$

_TIMER_CT2_OFF:
  PUSH HL    ; Save reg HL in stack
  LD H, ~IM_CT2; Disable interrupt for CT2

_TIMER_CT2_OFF_bgn:
  CALL #TIMER_OFF
  POP HL     ; Restore reg HL
  RET

;;
;; Example:
;;  LD H, ~IM_CT2; Disable interrupt for CT2
;;  CALL #TIMER_OFF; Turn off CT2 interrupt
;;
;; proc TIMER_OFF() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#TIMER_OFF:
  PUSH BC    ; Save reg BC in stack
  LD B, 0    ; Do not re-enable counter clock
  LD C, ~PPI_CS_CS1; Turn off counter clock
  RST 0x08   ; Disable counter output
  LD L, B    ; Restore interrupt mask to Acc
  CALL #INTR_MASK_SET; Send allowed intrrupts to interrupt vector
  POP BC     ; Restore reg BC
  RET

;;
;; Example:
;;  LD A, 5     ; Hold the sound for 0.5 second
;;  LD DE, _SOUND_OFF; Function to run after exec
;;  LD HL, PIT_FREQ_10Hz; Set CT1 to freq 10Hz
;;  CALL #TIMER_PUSH; Create a interrupt CT1 signal
;;
;; proc TIMER_PUSH() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- as defined
;;   reg HL -- as defined
#TIMER_PUSH:
  CALL _INTR_LOCK; Lock interrupt from happening again
  PUSH BC    ; Save reg BC in stack
  PUSH AF    ; Temporary save cycle amount
  PUSH HL    ; Temporary save counter divider in stack

  LD B, IM_CT1; Enable interrupt for CT1
  LD C, PIT_PORT_CT1; Setup CT1 to create interrupt signal
  LD HL, CT1_BUF_MAP; Load ptr to the CT1 task buffer

  LD A, (CT2_BUF_MAP); Load the amount of CT2 tasks
  ADD A, (HL); Get the sum of taks for CT1 & CT2 counter
  AND 0x01   ; Check if the last bit is set
  JR Z, #TIMER_PUSH_bgn-$

  LD B, IM_CT2; Enable interrupt for CT2
  LD C, PIT_PORT_CT2; Setup CT2 to create interrupt signal
  LD HL, CT2_BUF_MAP; Load ptr to the CT2 task buffer

#TIMER_PUSH_bgn:
  INC (HL)   ; Increment task amount for ct
  LD A, (HL) ; Load amount of task in CT
  ADD A, A   ; Multiply task amount by 2
  ADD A, A   ; Multiply task amount by 4
  ADD A, (HL); Multiply task amount by 5
  LD L, A    ; Create in reg HL addr that points to current last task
  LD (HL), D ; Load the high byte of the task func
  DEC HL     ; Move task ptr to the low addr of the task func
  LD (HL), E ; Load the low byte of the task func
  POP DE     ; Restore counter divider from stack
  DEC HL     ; Move task ptr to the high addr of the div
  LD (HL), D ; Load the high byte of the counter div
  DEC HL     ; Move task ptr to the low addr of the conter div
  LD (HL), E ; Load the low byte of the counter div
  DEC HL     ; Move task ptr to the value of how many time to repeat cycle
  POP AF     ; Restore cycles amount 
  LD (HL), A ; Load the cycles byte of the ct task 
  XOR A      ; Reset reg A
  LD L, A    ; Create in reg HL addr that points to the start of CT task buffer, aka amount of task
  LD A, (HL) ; Get amount of tasks in CT task buffer
  DEC A      ; Check if there is only one task
  CALL Z, #TIMER_WR; If so then, send timer config & enable interrupt
  POP BC     ; Restore reg BC from stack
  CALL _INTR_UNLOCK; Allow interrupt to happen
  RET

;;
;; Example:
;;  LD HL, PTR_CT1_CONF; Load in reg HL ptr to the CT1 configuration 
;;  LD (HL), 2 ; Load amount of times counter need to be decrimented
;;  INC HL     ; Move ptr to the high byte of the word
;;  LD (HL), 0 ; Load high byte of the func addr
;;  INC HL     ; Move ptr to the high byte of the word
;;  LD (HL), 0x38;Load low byte of the func addr
;;  JP #TIMER_CT0_EXEC; Create a constant sqaure wave sound
;;
;; proc TIMER_CT1_EXEC() -> void;
;; proc TIMER_CT2_EXEC() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#TIMER_CT1_EXEC:
  PUSH HL    ; Save reg HL in stack
  PUSH DE    ; Save reg DE in stack
  PUSH BC    ; Save reg BC in stack
  LD HL, CT1_BUF_MAP+1; Load in reg HL ptr to the CT1 task queue
  LD B, IM_CT1; Disable interrupt for CT1
  LD C, PIT_PORT_CT1; Setup CT1 to create interrupt signal
  JR #TIMER_EXEC-$

#TIMER_CT2_EXEC:
  PUSH HL    ; Save reg HL in stack
  PUSH DE    ; Save reg DE in stack
  PUSH BC    ; Save reg BC in stack
  LD HL, CT2_BUF_MAP+1; Load in reg HL ptr to the CT2 task queue
  LD B, IM_CT2; Disable interrupt for CT2
  LD C, PIT_PORT_CT2; Setup CT1 to create interrupt signal

#TIMER_EXEC:
  DEC (HL)   ; Decriment counter
  JR NZ, #TIMER_EXEC_end-$; If not zero then return
  PUSH BC    ; Tempory save interrupt mask & ct port in stack
  PUSH HL    ; Save task ptr in stack

  INC HL     ; Move ptr to the low byte of the divider 
  INC HL     ; Move ptr to the high byte of the divider 
  INC HL     ; Move ptr to the low byte of addr func
  RST 0x20   ; Load the func addr to reg HL
  LD A, EVENT_PRIO_TIMER; Set exec as timer priority
  CALL _EVENT_PUSH; Add func to task queue

  POP HL     ; Restore task ptr from stack
  PUSH HL    ; Save task ptr in stack
  PUSH HL    ; Temp save task ptr in stack for copy
  POP DE     ; Copy task ptr from stack for use in LDIR
  DEC HL     ; Move ptr to the ct task amount
  DEC (HL)   ; Remove first task from the list
  JR Z, #TIMER_EXEC_off-$

  LD A, (HL) ; Load amount of task in CT
  ADD A, A   ; Multiply task amount by 2
  ADD A, A   ; Multiply task amount by 4
  ADD A, (HL); Multiply task amount by 5
  LD C, A    ; Load amount of bytes that need to be shifted
  LD B, 0    ; Reset reg BC
  LD L, 6    ; Move task ptr to the next task in ct
  LDIR       ; Shift qunue by one task

  POP HL     ; Restore task ptr from stack
  POP BC     ; Restore interrupt mask & ct port
  INC HL     ; Move ptr to low byte of the divider
  LD E, (HL) ; Load the low byte of the divider
  INC HL     ; Move ptr to high byte of the divider
  LD D, (HL) ; Load the high byte of the divider
  CALL #TIMER_WR; Write divider info the the CT
  JR #TIMER_EXEC_end-$

#TIMER_EXEC_off:
  POP HL     ; Restore task ptr from stack, now is useless
  POP AF     ; Restore interrupt mask in reg A
  CPL        ; Make inverted mask to disable CT interrupt
  LD H, A    ; Load inverted mask to disable CT to reg H
  CALL #TIMER_OFF; Turn off interrupt programly 

#TIMER_EXEC_end:
  IN A, (PIT_PORT_CT0); Read counter high byte, aka reset counter interrupt
  IN A, (PIT_PORT_CT0); Read counter low byte, aka reset counter interrupt
  POP BC     ; Restore reg BC from stack
  POP DE     ; Restore reg DE from stack
  POP HL     ; Restore reg HL from stack
  RET

PIT_MODE_0     EQU 0x00 ;; Interrupt on Terminal Count
PIT_MODE_1     EQU 0x02 ;; Programmable One-Shot
PIT_MODE_2     EQU 0x04 ;; Rate Generator
PIT_MODE_3     EQU 0x06 ;; Sqaure Wave Generator
PIT_MODE_4     EQU 0x08 ;; Software Triggered Strobe
PIT_MODE_5     EQU 0x0A ;; Hardware Triggered Strobe

PIT_RL_0       EQU 0x00 ;; Save current counter state
PIT_RL_1       EQU 0x10 ;; Read/Write high byte
PIT_RL_2       EQU 0x20 ;; Read/Write low byte
PIT_RL_3       EQU 0x30 ;; Read/Write low byte, high byte

PIT_CS_0       EQU 0x00 ;; Counter 0
PIT_CS_1       EQU 0x40 ;; Counter 1
PIT_CS_2       EQU 0x80 ;; Counter 2

PIT_FREQ_10Hz  EQU 0xC350

PIT_NOTE_C     EQU 0x01
PIT_NOTE_C#    EQU 0x02
PIT_NOTE_D     EQU 0x03
PIT_NOTE_D#    EQU 0x04
PIT_NOTE_E     EQU 0x05
PIT_NOTE_F     EQU 0x06
PIT_NOTE_F#    EQU 0x07
PIT_NOTE_G     EQU 0x08
PIT_NOTE_G#    EQU 0x09
PIT_NOTE_A     EQU 0x0A
PIT_NOTE_A#    EQU 0x0B
PIT_NOTE_B     EQU 0x0C

PIT_PITCH_0    EQU 0x00 ; 16Hz - 30Hz - !!! Doesn't work with buzzer
PIT_PITCH_1    EQU 0x10 ; 32Hz - 61Hz - !!! Doesn't work with buzzer
PIT_PITCH_2    EQU 0x20 ; 65Hz - 123Hz - !!! Doesn't work with buzzer
PIT_PITCH_3    EQU 0x30 ; 130HZ - 246Hz - !!! Doesn't work with buzzer
PIT_PITCH_4    EQU 0x40 ; 261Hz - 493Hz - !!! Doesn't work with buzzer
PIT_PITCH_5    EQU 0x50 ; 523Hz - 987Hz
PIT_PITCH_6    EQU 0x60 ; 1046Hz - 1975Hz
PIT_PITCH_7    EQU 0x70 ; 2093Hz - 3951Hz
PIT_PITCH_8    EQU 0x80 ; 4186Hz - 7902Hz
PIT_PITCH_9    EQU 0x90 ; 8372Hz - 15804Hz
PIT_PITCH_A    EQU 0xA0 ; 16744Hz - 31608Hz - !!! Human hear limit
PIT_PITCH_B    EQU 0xB0 ; 33488Hz - 63217Hz - !!! Human hear limit

.NOTE_BLOCK:
  ;; ==============================================================================================
  ;;   1        2      3        4      5       6        7      8        9       A       B      C
  ;;   C0      C#0     D0      D#0     E0      F0      F#0     G0      G#0     A0      A#0    B0
  dw 0x7771, 0X70BD, 0x6A6A, 0x6471, 0x5ECD, 0x597b, 0x5475, 0x4FB8, 0x4B3E, 0x4705, 0x4309, 0x3F46
.NOTE_BLOCK_ED:
