

;;
;; Example:
;;  JP #SOUND_OUT; Create a constant sqaure wave sound
;;
;; proc SOUND_OUT() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#SOUND_OUT:
  PUSH BC    ;  Save reg BC in stack
  LD C, ~PPI_CS_CS0; Turn off sound counter clock
  LD B, 0x00 ; Do not enable anything
  OR A       ; Check if sound need to be disabled
  JR NZ, #SOUND_OUT_end-$

  LD B, PPI_CS_CS0; Enable sound counter clock
  LD A, PIT_RL_3 | PIT_MODE_3; Set up CT0 to create sqaure wave signal
  LD (PIT_PORT_CTL), A; Send reg to PIT

  LD A, 0x00 ; TODO: Impl this
  OUT (PIT_PORT_CTL), A; Send counter high byte
  LD A, 0x00 ; TODO: Impl this
  OUT (PIT_PORT_CTL), A; Send counter low byte

#SOUND_OUT_end
  RST 0x08   ; Disable sound output
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

; TODO:
NOTE_C8  					EQU 0x0125
NOTE_CS8 					EQU 0x0115
NOTE_D8  					EQU 0x0105
NOTE_DS8 					EQU 0x00F6
NOTE_E8  					EQU 0x00E8
NOTE_F8  					EQU 0x00DB
NOTE_FS8 					EQU 0x00CF
NOTE_G8  					EQU 0x00C3
NOTE_GS8 					EQU 0x00B8
NOTE_A8  					EQU 0x00AE
NOTE_AS8 					EQU 0x00A4
NOTE_B8  					EQU 0x009B