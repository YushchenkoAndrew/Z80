;;
;; Example:
;;  LD DE, .STRING; Load ptr to the string
;;  LD B, 60    ; Load the string length, can be 0 for escape when '\0' has been meet
;;  CALL #STR_STRIP ; Will retrun reg DE that points to the stripted string
;;
;; proc STR_STRIP(...regs) -> reg B & reg DE & reg F;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
#STR_STRIP:
  LD A, (DE) ; Load current ASCII char
  CP SPACE   ; Check if char is ' '
  RET NZ     ; Escape if char is not the ' '
  INC E      ; Increment ptr to the next ASCII char
  DJNZ #STR_STRIP-$
  RET

;;
;; Example:
;;  LD HL, .MSG_FILE_NOT_FOUND; Load ptr to the string message
;;  CALL #MSG_OK
;;
;; proc MSG_OK() -> void;
;;   reg A  -- as defined
;;   reg BC -- as defined
;;   reg DE -- as defined
;;   reg HL -- unaffected
;;
#STR_PRINT:
  LD A, (HL) ;; Get curr char
  OR A       ;; Check if line is ended (Set flag Z)
  RET Z      ;; Return if str is ended 
  INC HL     ;; Inc arg pointer
  RST 0x10   ;; Output the char
  JR #STR_PRINT-$

  ; LD A, (HL) ;; Get curr char
  ; OR A       ;; Check if line is ended (Set flag Z)
  ; RET Z      ;; Return if str is ended 
  ; INC HL     ;; Inc arg pointer
  ; RST 0x10   ;; Output the char
  ; JR RST18-$