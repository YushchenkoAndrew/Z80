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
;; proc #RTC_CURRENT_TIME() -> reg HL;
;;   reg A  -- unaffected
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
#RTC_CURRENT_TIME:
  PUSH AF    ; Save reg AF in stack
  PUSH DE    ; Save reg DE in stack

  LD A, (RTC_HOUR) ; Get current hours value
  LD HL, 3600  ; Load amount of seconds in 1 hour
  CALL #MUL16x8; Multiply current amount of hours by 60
  EX DE, HL  ; Save result of multiplication in reg DE

  LD A, (RTC_MINUTES) ; Get current minutes value
  LD HL, 60  ; Load amount of seconds in 1 minute
  CALL #MUL16x8; Multiply current amount of minutes by 60
  ADD HL, DE ; Get total value of hours and minutes in seconds

  LD A, (RTC_SECONDS) ; Get current seconds value
  LD D, 0    ; Reset reg D
  LD E, A    ; Load into reg E current seconds value
  ADD HL, DE ; Get current time in reg HL

  POP AF     ; Restore reg AF from stack
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

  LD (RTC_REG_B), A ; Load initial settings byte to the RTC's reg B
  LD B, A    ; Load the original value of RTC's reg B
  AND RTC_REG_B_SET ; Check if update cycle in RTC has already stoped
  JR NZ, #RTC_SET_ALARM_bgn-$

  LD A, B    ; Load into reg A RTC's reg B
  OR RTC_REG_B_SET ; Temporary stop update cycle in RTC
  LD (RTC_REG_B), A ; Load modified RTC's reg B, aka temporary stop update cycle

#RTC_SET_ALARM_bgn:
  ; TODO: Impl this

  ; LD A, 60   ; Load divisior value, aka get amount of seconds
  ; CALL #DIV16x8; Divide reg HL / 60 => reg A - seconds & reg HL - minutes
  ; LD C, A   
  ; LD (RTC_ALARM_SECONDS), A ; Set amo

  ; LD A, 60   ; Load divisior value, aka get amount of minutes
  ; CALL #DIV16x8; Divide reg HL / 60 => reg A - minutes & reg HL - hours
  ; LD (RTC_ALARM_MINUTES), A ; Intialize  value

  ; LD A, L    ; Get amount of hours
  ; LD (RTC_ALARM_MINUTES), A ; Intialize second value


  POP BC     ; Restore reg BC from stack
  RET

; TODO: Impl proc #RTC_ALARM_PUSH 

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