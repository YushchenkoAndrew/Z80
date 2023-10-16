#include "Command.asm"

;;
;; Example:
;;  CALL _ACK_BUFFERS
;;
;; func _ACK_BUFFERS() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- as defined
_ACK_BUFFERS:
  LD HL, SCAN_KEY_BUF ; Load scan key buf offset ptr
  LD A, (HL)  ; Load scan key buf offset val
  AND 0x0F    ; Get only low bites
  DEC A       ; Check if offset is reseted, flag Z
  RET Z       ; If nothing happend, then return
  LD B, A     ; Load amount of char in reg B

  RRD         ; Load to reg A only low bit
  LD A, 0x01  ; Reset offset value to 1
  RLD         ; Reset offset value, aka start from 1

  INC HL      ; Move buf ptr to the start of chars
  LD DE, TEXT_BUF_MAP; Get ptr to allocated text arae
  LD A, (PTR_TEXT_BUFF_END); Get buf-ptr offset
  LD E, A     ; Move text ptr to empty char
  
  CALL #WR_BUFFER  ; Check char, save it in buffer & output it
  RET


;;
;; Example:
;;  LD HL, number
;;  LD DE, number2  ; ptr in ram
;;  LD B, 5         ; str size
;;  CALL WR_BUFFER
;; 
;; number:
;;  db "test2"
;;
;; proc WR_BUFFER() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- unaffected
;;   reg HL -- as defined
#WR_BUFFER:
  LD A, (HL)  ; Load char from the str ptr
  OR A        ; Check if value is empty
  JR Z, #WR_BUFFER_nxt-$
  CP CARRY_RET; Check if char is '\r'
  JR Z, #WR_BUFFER_nxt-$
  CP BACKSPACE; Check if need to del prev char
  JR NZ, #WR_BUFFER_out-$
  DEC E       ; Move dst string ptr, back by one char
  RST 0x10    ; Output the char
  JR #WR_BUFFER_nxt_esc-$

#WR_BUFFER_out:
  RST 0x10    ; Output the char

#WR_BUFFER_nxt:
  LD (DE), A  ; Save curr char in buffer
  INC E       ; Move dst string ptr, reg DE will reset after 0xFF

#WR_BUFFER_nxt_esc:
  PUSH HL     ; Save src string ptr in Stack
  LD HL, (PTR_INPUT_TO_ADDR); Load ptr to jump addr
  XOR A       ; Reset Acc
  OR H        ; Bit or to 0 reg H
  OR L        ; Bit or to reg H + reg L
  JR Z, #WR_BUFFER_cmd-$
  PUSH HL     ; Temp save jump addr in stakc
  LD HL, #WR_BUFFER_end_of_txt_end; Load return addr
  EX (SP), HL ; Save return addr & get in reg HL jump addr
  JP (HL)     ; Manual call
#WR_BUFFER_end_of_txt_end:
  POP HL      ; Restore src string ptr in Stack
  JR #WR_BUFFER_end-$

#WR_BUFFER_cmd:
  POP HL      ; Restore src string ptr in Stack
  LD A, (HL)  ; Load char from the str ptr
  CP END_OF_TXT ; Check if char was Ctrl+L
  CALL Z, _CMD_EXEC_ESC
  CP FORM_FEED ; Check if char was Ctrl+L
  CALL Z, _CMD_EXEC
  CP LINE_FEED ; Check if char was '\n'
  CALL Z, _CMD_EXEC

#WR_BUFFER_end:
  INC HL      ; Move src string ptr
  LD A, E     ; Get next offset text buffer offset
  LD (PTR_TEXT_BUFF_END), A; Save offset in ptr
  DJNZ #WR_BUFFER-$
  RET
