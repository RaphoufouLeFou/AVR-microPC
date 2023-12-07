ldi r17 0
ldi r18 0
rjmp outputAll
ldi r21 255
ldi r22 255
rjmp outputPix
inc r18
mov r16 r18
andi r16 1
cpi r16 1
breq -5
cpi r18 64
brpl 2
rjmp outputPix
jmp 6
inc r17
mov r16 r17
andi r16 1
mov r18 r16
jmp 6