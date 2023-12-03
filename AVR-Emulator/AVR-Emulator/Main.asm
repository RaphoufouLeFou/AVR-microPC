ldi r17 0
ldi r18 0
rjmp outputAll
ldi r21 255
ldi r22 255
mov r22 r18
mov r21 r17
rjmp outputPix
inc r18
cpi r18 65
breq 2
rjmp outputPix
jmp 5
inc r17
jmp 5