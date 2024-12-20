#include "Defs.asm"

;;
;; Example:
;;  CALL _RTC_INIT; Initialize RTC to the date = 00:00:00  Fri 13 Dec
;;
;; func _RTC_INIT() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- unaffected
_RTC_INIT:
  PUSH AF    ; Temp save reg AF in stack

  LD A, RTC_REG_B_SET; Temporary stop update cycle in RTC
  LD (RTC_REG_B), A ; Load initial settings byte to the RTC's reg B

  LD A, RTC_REG_A_DV1 ; Set crystal freqency to be 32768 Hz 
  LD (RTC_REG_A), A ; Load initial settings byte to the RTC's reg A

  XOR A      ; Reset reg A
  LD (RTC_SECONDS), A ; Intialize second value to be 0
  LD (RTC_MINUTES), A ; Intialize minute value to be 0
  LD (RTC_HOUR),    A ; Intialize hour value to be 0

  LD A, 5    ; Set reg A to be a Fri
  LD (RTC_DAY_OF_WEEK), A ; Intialize day of the week to be a Fri

  LD A, 13   ; Set reg A to 13 Dec
  LD (RTC_DATE_OF_MONTH), A ; Intialize date of the month to be 13 Dec

  LD A, 12   ; Set reg A to Dec
  LD (RTC_MONTH), A ; Intialize month to be Dec

  LD A, 24   ; Set reg A to be 2024
  LD (RTC_YEAR), A ; Intialize year to be 2024

  LD A, RTC_REG_B_DM | RTC_REG_B_24 ; Count time in binary mode and use 24h
  LD (RTC_REG_B), A ; Release update RTC update cycle, count time in binary mode and use 24h

  POP AF     ; Restore reg AF from stack
  RET


;;
;; Example:
;;  LD A, 32   ; Seconds 0-59
;;  LD B, 50   ; Minutes 0-59
;;  LD C, 12   ; Hours   0-23
;;  CALL #RTC_SET_TIME; Set RTC time, 12:50:32
;;
;; func #RTC_SET_TIME() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- unaffected
;;   reg HL -- unaffected
#RTC_SET_TIME:
  PUSH DE    ; Save reg DE in stack

  LD E, A    ; Move seconds value to the reg E 
  LD (RTC_REG_B), A ; Load initial settings byte to the RTC's reg B
  LD D, A    ; Load the original value of RTC's reg B
  AND RTC_REG_B_SET ; Check if update cycle in RTC has already stoped
  JR NZ, #RTC_SET_TIME_bgn-$

  LD A, D    ; Load into reg A RTC's reg B
  OR RTC_REG_B_SET ; Temporary stop update cycle in RTC
  LD (RTC_REG_B), A ; Load modified RTC's reg B, aka temporary stop update cycle

#RTC_SET_TIME_bgn:
  LD A, E    ; Load amount of seconds to Acc
  LD (RTC_SECONDS), A ; Intialize second value

  LD A, B    ; Load amount of minutes to Acc
  LD (RTC_MINUTES), A ; Intialize minute value

  LD A, C    ; Load amount of hours to Acc
  LD (RTC_HOUR), A ; Intialize hour value

  LD A, D    ; Load into reg A RTC's reg B
  LD (RTC_REG_B), A ; Restore original value from RTC's reg B

  POP DE     ; Restore reg DE from stack
  RET

;;
;; Example:
;;  CALL #RTC_CURRENT_TIME; Get RTC current time in seconds
;;
;; proc #RTC_CURRENT_TIME() -> reg A & reg HL (aka 24 bit);
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#RTC_CURRENT_TIME:
  PUSH DE    ; Save reg DE in stack

  LD A, (RTC_SECONDS) ; Get current seconds value
  LD D, 0    ; Reset reg D
  LD E, A    ; Load into reg E current seconds value

  LD A, (RTC_MINUTES) ; Get current minutes value
  LD HL, 60  ; Load amount of seconds in 1 minute
  CALL #MUL16x8; Multiply current amount of minutes by 60
  ADD HL, DE ; Get total value of seconds and minutes in seconds
  EX DE, HL  ; Save value of seconds and minutes in seconds to reg DE

  LD A, (RTC_HOUR) ; Get current hours value
  LD HL, 3600  ; Load amount of seconds in 1 hour
  CALL #MUL16x8; Multiply current amount of hours by 60
  ADD HL, DE ; Get current time in reg HL
  LD E, 0    ; Reset reg E
  ADC A, E   ; Add carry bit to reg A

  POP DE     ; Restore reg DE from stack
  RET

;;
;; Example:
;;  LD HL, 3502; Create a delay in seconds
;;  CALL #RTC_SET_ALARM; Set RTC alarm time to 00:58:22 + <current_time>
;;
;; func #RTC_SET_ALARM() -> void;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#RTC_SET_ALARM:
  PUSH BC    ; Save reg BC in stack
  PUSH DE    ; Save reg DE in stack

  LD (RTC_REG_B), A ; Load initial settings byte to the RTC's reg B
  LD B, A    ; Load the original value of RTC's reg B
  AND RTC_REG_B_SET ; Check if update cycle in RTC has already stoped
  JR NZ, #RTC_SET_ALARM_bgn-$

  LD A, B    ; Load into reg A RTC's reg B
  OR RTC_REG_B_SET ; Temporary stop update cycle in RTC
  LD (RTC_REG_B), A ; Load modified RTC's reg B, aka temporary stop update cycle

#RTC_SET_ALARM_bgn:
  EX DE, HL  ; Load in reg DE offset in seconds
  CALL #RTC_CURRENT_TIME ; Get current time in reg HL
  ADD HL, DE ; Calc next alarm time in seconds
  LD E, 0    ; Reset reg E
  ADC A, E   ; Add carry bit to the reg A
  PUSH AF    ; Save reg A from A:HL (24 bits) in stack

  LD A, 60   ; Load divisior value, aka get amount of seconds
  CALL #DIV16x8; Divide reg HL / 60 => reg A - seconds & reg HL - minutes
  LD (RTC_ALARM_SECONDS), A ; Set amount of seconds

  LD A, 60   ; Load divisior value, aka get amount of minutes
  CALL #DIV16x8; Divide reg HL / 60 => reg A - minutes & reg HL - hours
  LD (RTC_ALARM_MINUTES), A ; Set amount of minutes

  POP AF     ; Restore reg A value from 24 bits of current time
  OR A       ; Check if the reg is empty
  LD A, L    ; Load remaning amount of hours in Acc
  JR Z, #RTC_SET_ALARM_hour-$; If reg doesn't some value, it means then reg HL was not overflowed with hours
  ADD A, 18  ; Adjust hours overflow, max hours from (2^16 - 1) is 18

#RTC_SET_ALARM_hour:
  LD D, A    ; Load amount hours to set next alarm
  LD E, 24   ; Load divisior value, aka get amount of hours
  CALL #DIV8 ; In reg A will return remainder of hours, aka reg D % 24 = reg A
  LD (RTC_ALARM_HOUR), A ; Set amount of hours

  POP DE     ; Restore reg DE from stack
  POP BC     ; Restore reg BC from stack
  RET

;;
;; Example:
;;  LD A, 5     ; Hold the sound for 0.5 second
;;  LD DE, _SOUND_OFF; Function to run after exec
;;  LD HL, 2    ; Set alarm offset to 2 seconds
;;  CALL #RTC_ALARM_PUSH; Create a interrupt CT1 signal
;;
;; proc RTC_ALARM_PUSH() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- as defined
;;   reg HL -- as defined
#RTC_ALARM_PUSH:
  ; TODO: Impl proc #RTC_ALARM_PUSH 
  RET

#RTC_ALARM_EXEC:
  ; TODO: Write IRQ exec logic
  RET

RTC_SECONDS          EQU RTC_ADDR
RTC_ALARM_SECONDS    EQU RTC_ADDR + 1
RTC_MINUTES          EQU RTC_ADDR + 2
RTC_ALARM_MINUTES    EQU RTC_ADDR + 3
RTC_HOUR             EQU RTC_ADDR + 4
RTC_ALARM_HOUR       EQU RTC_ADDR + 5
RTC_DAY_OF_WEEK      EQU RTC_ADDR + 6
RTC_DATE_OF_MONTH    EQU RTC_ADDR + 7
RTC_MONTH            EQU RTC_ADDR + 8
RTC_YEAR             EQU RTC_ADDR + 9
RTC_REG_A            EQU RTC_ADDR + 10
RTC_REG_B            EQU RTC_ADDR + 11
RTC_REG_C            EQU RTC_ADDR + 12
RTC_REG_D            EQU RTC_ADDR + 13

RTC_REG_A_UIP        EQU 0x80
RTC_REG_A_DV2        EQU 0x40
RTC_REG_A_DV1        EQU 0x20
RTC_REG_A_DV0        EQU 0x10
RTC_REG_A_RS3        EQU 0x08
RTC_REG_A_RS2        EQU 0x04
RTC_REG_A_RS1        EQU 0x02
RTC_REG_A_RS0        EQU 0x01

RTC_REG_B_SET        EQU 0x80
RTC_REG_B_PIE        EQU 0x40
RTC_REG_B_AIE        EQU 0x20
RTC_REG_B_UIE        EQU 0x10
RTC_REG_B_SQWE       EQU 0x08
RTC_REG_B_DM         EQU 0x04
RTC_REG_B_24         EQU 0x02
RTC_REG_B_DSE        EQU 0x01

RTC_REG_C_IRQF       EQU 0x80
RTC_REG_C_PF         EQU 0x40
RTC_REG_C_AF         EQU 0x20
RTC_REG_C_UF         EQU 0x10

RTC_REG_D_VRT        EQU 0x80