#include "Defs.asm"

;; NOTE: Inspired by this idea
;; https://www.studytonight.com/operating-system/priority-scheduling
;;
;; Example:
;;  CALL #EVENT_LOOP
;;
;; proc #EVENT_LOOP() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- as defined
#EVENT_LOOP:
  LD HL, TASK_BUF_MAP; Load ptr to the queue start
  LD A, (HL)  ; Load amount of task in queue
  OR A        ; Check if there is no task in queue
  JR Z, #EVENT_LOOP-$; If nothing happend

  XOR A       ; Reset Acc
  LD B, A     ; Reset reg B
  DEC (HL)    ; Remove one task from queue
  LD A, (HL)  ; Set the amount tasks remaning in the queue
  ADD A, A    ; Mutltiply by 2 the amount of bytes to copy
  ADD A, (HL) ; Mutltiply by 3 the amount of bytes to copy
  LD C, A     ; Save amount of bytes in reg C 

  INC HL      ; Move queue ptr to the priority byte
  INC HL      ; Move queue ptr to the high addr
  LD D, (HL)  ; Load the high byte of the func addr
  INC HL      ; Move queue ptr to the low addr of func
  LD E, (HL)  ; Load the low byte of the func addr
  PUSH DE     ; Temporary save func addr in stack
  INC HL      ; Move queue ptr to the next 
  LD DE, TASK_BUF_MAP+1; Load ptr to the start of the queue
  JR Z, #EVENT_LOOP_exec-$; Current task is the last one, then jump to exec it
  LDIR        ; Shift all tasks by one addr

#EVENT_LOOP_exec:
  LD HL, #EVENT_LOOP_end; Load custom return addr
  EX (SP), HL ; Manually create CALL and load in reg HL func addr
  JP (HL)     ; Jump to the func execution

#EVENT_LOOP_end:
  JR #EVENT_LOOP-$


;;
;; Example:
;;  LD A, 0     ; Load task priority
;;  LD HL, #ACK_BUFFERS; Load task addr
;;  CALL _EVENT_PUSH
;;
;; func _EVENT_PUSH() -> void;
;;   reg A  -- as defined
;;   reg BC -- unaffected
;;   reg DE -- unaffected
;;   reg HL -- as defined
_EVENT_PUSH:
  PUSH BC     ; Save reg BC in stack
  PUSH DE     ; Save reg BC in stack
  PUSH HL     ; Temp save task addr in stack
  PUSH AF     ; Temp save task priority in stack

  LD HL, TASK_BUF_MAP
  PUSH HL     ; Temp save start of task queue addr in Stack
  POP DE      ; Load task queue addr into reg DE
  LD B, (HL)  ; Load amount of task in queue
  INC (HL)    ; Add new task to the queue counter
                                                                                                                                      
  LD A, (HL)  ; Load amount of task in queue
  ADD A, A    ; Multiply task amount by 2
  ADD A, (HL) ; Multiply task amount by 3
  LD E, A     ; Create in reg DE addr that points to the new last task position

  POP AF      ; Restore task priority
  LD (DE), A  ; Temporary save priority in final task addr
  LD A, E     ; Restore task offset in Acc
  SUB 3       ; Calc offset for current last task
  LD L, A     ; Create in reg HL addr that points to current last task
  LD A, (DE)  ; Restore priority value in Acc
  JR Z, _EVENT_PUSH_wr-$ ; Check if there are no task in queue, if so then just write without shift

  PUSH HL     ; Save in stack current last task position
  LD HL, TASK_BUF_MAP+1 ; Load in reg HL ptr to the first task priority byte

_EVENT_PUSH_lp:
  CP (HL)     ; Check task priority with requested one
  INC HL      ; Move queue ptr to the low addr
  INC HL      ; Move queue ptr to the high addr
  JR C, _EVENT_PUSH_srl-$ ;  Check if the requested priority > current task priority, then JUMP
  INC HL      ; Move queue ptr to the next priority byte
  DJNZ _EVENT_PUSH_lp-$

_EVENT_PUSH_srl:
  POP HL      ; Restore ptr to the last current task
  LD A, B     ; Load to Acc the difference of task that needed to be shifted
  ADD A, A    ; Multiply the differnce by 2
  ADD A, B    ; Multiply the differnce by 3
  LD C, A     ; Save the difference of bytes that needed to be shifted
  XOR A       ; Reset Acc
  LD B, A     ; Reset reg B
  OR C        ; Check if difference of bytes is zero
  LD A, (DE)  ; Restore priority value in Acc
  JR Z, _EVENT_PUSH_wr-$
  LDDR        ; Shift tasks to right byte one task

_EVENT_PUSH_wr:
  POP HL      ; Restore func addr
  EX DE, HL   ; Move func addr  & task addr
  LD (HL), E  ; Save in queue to the low addr of func
  DEC HL      ; Move queue ptr to the high addr
  LD (HL), D  ; Save in queue to the high addr of func
  DEC HL      ; Move queue ptr to the priority byte
  LD (HL), A  ; Save in queue to priority byte

  POP DE      ; Restore reg DE
  POP BC      ; Restore reg BC
  RET


EVENT_PRIO_SYS           EQU 0x00
EVENT_PRIO_IO            EQU 0x01
EVENT_PRIO_TIMER         EQU 0x02
EVENT_PRIO_HIGH          EQU 0x03
EVENT_PRIO_MEDIUM        EQU 0x04
EVENT_PRIO_LOW           EQU 0x05
EVENT_PRIO_IDLE          EQU 0x06
EVENT_PRIO_BG            EQU 0x07