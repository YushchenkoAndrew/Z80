;####################################################
; PROGRAM NAME: - Test
; DISCRIPTION:  - Test Real-Time Clock
; DATE: 20.03.21
; AUTHOR: Andrew Yushchenko
;####################################################

 ORG 0x0000
 JP SETUP

  ; ###################################################
  ; ##################   SETUP    #####################
  ; ###################################################
SETUP:
 LD SP, STACK      ; Set Memory Paging RAM

 JP MAIN

;; Initialize Real-Time Clock
INIT_RTC:
 ;; This need just for temporary disable Clock Update
 LD HL, RTC_REG_B_ADDR
 LD A, 0x80
 LD (HL), A


 ;; Set Seconds to 00s in BCD Mode (Binary-Coded-Decimal)
 LD HL, RTC_ADDR            ;; Load Address of the RTC
 LD A, 0x00
 LD (HL), A

 ;; Set Seconds Alaram to 00s in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Seconds Alarm Addr 0x01
 LD A, 0x00
 LD (HL), A

 ;; Set Minutes to 25m in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Minutes Addr 0x02
 LD A, 0x25
 LD (HL), A

 ;; Set Minutes Alaram to 00m in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Minutes Alarm Addr 0x03
 LD A, 0x00
 LD (HL), A

 ;; Set Hour to 01h in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Hour Addr 0x04
 LD A, 0x01
 LD (HL), A

 ;; Set Hour Alarm to 01h in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Hour Alarm Addr 0x05
 LD A, 0x01
 LD (HL), A

 ;; Set Day of the Week in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Day of the Week Addr 0x06
 LD A, 0x07
 LD (HL), A

 ;; Set Day of the Month in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Day of the Month Addr 0x07
 LD A, 0x20
 LD (HL), A

 ;; Set Month in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Month Addr 0x08
 LD A, 0x03
 LD (HL), A

 ;; Set Year in BCD Mode (Binary-Coded-Decimal)
 INC HL                ;; Move to Year Addr 0x09
 LD A, 0x21
 LD (HL), A


 ;; Configure Reg A
 LD HL, RTC_REG_A_ADDR
 LD A, 0x20              ;; UIP - Read-Only | 010 - DV2,DV1,DV0  =  32.768kHz | 0000 - RS3,RS2,RS1,RS0  =  None
 LD (HL), A

 ;; Configure Reg B
 LD HL, RTC_REG_B_ADDR
 LD A, 0x03              ;; 0 - SET | 0 - PIE | 0 - AIE | 0 - UIE | 0 - SQWE | 0 - DM (BCD) | 1 - 24/12 (24) | 1 - DSE
 LD (HL), A


 ;; Test RAM
 LD HL, RTC_RAM_ADDR
 LD A, 0x7F
 LD (HL), A
 RET

  ; ###################################################
  ; ##################    MAIN     ####################
  ; ###################################################
MAIN:
 IN A, (SWITCH_PORT)
 OUT (LED_PORT), A

 ;; Check If
 BIT 7, A
 JP NZ, INIT_RTC

 LD HL, RTC_ADDR
 LD L, A
 LD A, (HL)
 CALL DISPLAY_BYTE
 JP MAIN

GET_NUM:
 LD BC, 0x00    ;; Reset reg BC value
 LD HL, NUM    ;; Get addr of NUM
 AND 0x0F     ;; Get only low bites
 LD C, A      ;; Set addr offset for NUM
 ADD HL, BC    ;; Get the correct pointer position
 LD A, (HL)    ;; Get the value
 RET


DISPLAY_BYTE:
 LD D, A      ;; Save reg 'A' value in reg 'D'

 CALL GET_NUM   ;; Input/Output value is located in reg 'A'
 OUT (HEX_PORT), A   ;; Show LOW bites

 LD A, D      ;; Restore save value

 ;; Shift Reg by 4 bit
 RR A
 RR A
 RR A
 RR A

 CALL GET_NUM   ;; Input/Output value is located in reg 'A'
 OR 0x80      ;; Set to show in second display
 OUT (0x20), A   ;; Show HIGHT bites

 LD A, D
 RET


; Variables name
STACK            EQU 0x1FFF

RTC_ADDR        EQU 0x3000
RTC_SEC_ADDR      EQU 0x3000   ;; Seconds
RTC_SEC_AL_ADDR   EQU 0x3001   ;; Seconds Alarm
RTC_MIN_ADDR      EQU 0x3002   ;; Minutes
RTC_MIN_AL_ADDR   EQU 0x3003   ;; Minutes Alarm
RTC_HOUR_ADDR     EQU 0x3004   ;; Hours
RTC_HOUR_AL_ADDR  EQU 0x3005   ;; Hours Alarm
RTC_DAY_W_ADDR    EQU 0x3006   ;; Day of the Week
RTC_DAY_M_ADDR   EQU 0x3007   ;; Day of the Month
RTC_MONTH_ADDR    EQU 0x3008   ;; Month
RTC_YEAR_ADDR    EQU 0x3009   ;; Year
RTC_REG_A_ADDR    EQU 0x300A   ;; Register A, Read/Write Register except UIP
RTC_REG_B_ADDR    EQU 0x300B   ;; Register B, Read/Write Register
RTC_REG_C_ADDR    EQU 0x300C   ;; Register C, Read-Only Register
RTC_REG_D_ADDR    EQU 0x300D   ;; Register D, Read-Only Register
RTC_RAM_ADDR     EQU 0x300E   ;; RAM


TIMER_VAR         EQU 0x1000
MUSIC_PTR         EQU 0x1001


;; Ports
SWITCH_PORT      EQU 0x00
LED_PORT         EQU 0x10
HEX_PORT         EQU 0x20
DISPLAY_PORT     EQU 0x30
KEYBOARD_PORT    EQU 0x40
VECTOR_PORT      EQU 0x50
UART_PORT        EQU 0x60
MMU_PORT         EQU 0x70
TIMER_C0_PORT    EQU 0x80
TIMER_C1_PORT    EQU 0x81
TIMER_C2_PORT    EQU 0x82
TIMER_MODE_PORT  EQU 0x83

;; Interrupt Vector Bits
KEY_BIT      EQU 7
UART_Rx_BIT    EQU 6
UART_Tx_BIT    EQU 5
RTC_BIT      EQU 4
TIMER_BIT     EQU 3

NUM:   ; This array of values need for display HEX number
 db 0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0E