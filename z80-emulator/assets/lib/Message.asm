
;;
;; Example:
;;  CALL #MSG_OK
;;
;; proc MSG_OK() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#MSG_OK:
  LD HL, MSG_OK
  RST 0x18    ; Print string
  RET

MSG_OK:
  db "OK", 0

;;
;; Example:
;;  CALL #MSG_NUM_ERR
;;
;; proc MSG_OK() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#MSG_NUM_ERR:
  LD HL, MSG_INVALID_ADDRESS
  RST 0x18    ; Print string
  RET 

MSG_INVALID_ADDRESS:
  db "invalid address", 0

;;
;; Example:
;;  CALL #MSG_ARG_ERR
;;
;; proc MSG_OK() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#MSG_ARG_ERR:
  LD HL, MSG_INVALID_COMMAND_ARG
  RST 0x18    ; Print string
  RET 

MSG_INVALID_COMMAND_ARG:
  db "invalid command arg", 0


MSG_COMMAND_NOT_FOUND:
  db "command not found: '", 0
