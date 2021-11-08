; First pass errors 

LABEL: .db -0,5,6,7,2,6
CODELABEL: move $23,$1
LOCALCODE: add $0,$1,$4
LOCALDATA: .asciz "Some local data"
LABEL: .asciz "Same label twice!"
CODELABEL: .dh "Again!"
.extern LOCALCODE
.extern LOCALDATA
.extern WHATEVEREXT
.extern LONGLONGLONGLONGLONGLONGLONGLONGLABEL
.extern 1nvalidLabel
.extern Label@With4t
.extern 
.add $5, $9, $22
WHATEVEREXT: .asciz "Another definition!"
VERY_VERY_VERY_VERY_VERY_LONG_LABEL_DEFINITION_SO_ITS_ILLEGAL: .dh 0
SOME*INVALID&CHARS%HERE: .dh 5
2tartWithNumISILLEGAL: .dh -6
ALLVALIDBUTLASTI$: .asciz "more invalid!"

.asciz "No closing
.asciz No opening"
.asciz Not at all
.dh ,5,6,-7
.dh 5,,6,-7
.dh 5,6,,-7
.dh 5,6,-7,
.dh 5 6 -7
.dh 5 6 , -7
.dh 5 , 6 -7,
.dh 0.5
.dh 5.9
.dh -0.1
.dh .5


add $1, $2
move
move $0
move $0,LABEL,5
add
add $7
sub
sub $5
jmp
jmp somewhere,around
stop myprog
stop myprog,yourprog

add 5,6,7
add $1, $2, LABEL
blt LABEL, $1, LABEL
lh $9, $5, $6
lw $9, LABEL, $5
jmp -40
call $9
la 90
la $10
addi $9, LABEL, $8
mvlo 40, $4

.db -129
.db 129
.dh -32770
.dh 32770
.dw 2147483649

.db 270 
.dh 7000000


add $50, $4, $6
.extern REALLYREALLYREALLYREALLYREALLYREALLYLONG
op operand,operand
label : illegal
lable!:illegal
label: .asciz "Hello World!"
labell: .dh "Hello World!"
label .lh "Hello World!" mov $0,5

string: .asciz "mystr"
entry: .dh 12
extern: .dh +5

TOOOOOOOOOOOOOOOOOOOOOOLOOOOOOOOOOOOOOOOOOONGGGGGGGGGGGGGGGGGGTOOOOOOPROCESSSSSSS
.extern HELLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLOO

