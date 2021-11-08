; Second pass errors

DATALABEL: .asciz "My String.$#"
MYSTRING: .asciz "My other string"
MYDATA: .db 5,4,3,2,1
X: .asciz "XXXXX"

.extern EXTERNLABEL
.entry DATALABEL
.entry CODE
CODE: move $0, $1

; external label cannot be defined as an entry one as well
.entry EXTERNLABEL

call MYSTRINGS
call MYSTRIN
call YSTRING
call MYSTRING123
call MYSTRING0

call XX
call Y
call X0
call X

.entry SOMEUNKNOWNLABEL
.entry MYSTRING
.entry MYDATA
.entry MYDATA0
.entry 0MYDATA
.entry 0MYSTRING
.entry 0
.entry NULL

; Assembler will ignore
EMPTYLABEL:

jmp EMPTYLABEL

.entry LONGLONGLONGLONGLONGLONGLONGLONGLABEL1234