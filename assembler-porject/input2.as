.entry LIST
.extern W

MAIN:	nor $3, $7,$11
STR: .asciz "Hello!"
LIST: subi $9,-45,$8
.entry MAIN

jmp STR
call STR
bgt $7,$11, W
DATA: .dh 90,-11
sb $7,-28,$18
lh $9,34,$2
la LIST
	stop
	
blt $5,$24, LIST