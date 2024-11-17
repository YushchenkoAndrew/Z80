#include "Defs.asm"


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
  LD A, PIT_RL_3 | PIT_MODE_3; Set up CT0 to create sqaure wave signal
  OUT (PIT_PORT_CTL), A; Send reg to PIT
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
  LD A, (HL) ; Get the high byte of the note
  INC HL     ; Move ptr to the low byte of the note
  LD L, (HL) ; Get the low byte of the note in reg L
  LD H, A    ; Completelly load note to the reg HL
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
  OUT (PIT_PORT_CT0), A; Send counter high byte
  LD A, H    ; Load the final value of note high byte
  OUT (PIT_PORT_CT0), A; Send counter low byte
  POP BC     ; Restore PPI settings from stack

#SOUND_OUT_esc:
  RST 0x08   ; Disable sound output
  POP HL     ; Restore reg HL
  POP BC     ; Restore reg BC from stack
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

.NOTE_BLOCK:
  ;; ==============================================================================================
  ;;   1        2      3        4      5       6        7      8        9       A       B      C
  ;;   C0      C#0     D0      D#0     E0      F0      F#0     G0      G#0     A0      A#0    B0
  db 0x7771, 0X70BD, 0x6A6A, 0x6471, 0x5ECD, 0x597b, 0x5475, 0x4FB8, 0x4B3E, 0x4705, 0x4309, 0x3F46
.NOTE_BLOCK_ED:
