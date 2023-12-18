    ldi     r17,        0 
    ldi     r18,        0
    ldi     r21,        0b11111000
    rjmp    outputAll           ;blablabla
    ldi     r21,        HIGH(0xFFFF)
    ldi     r22,        LOW(0xFFFF)	    ;blablabla
loop:
    rjmp    outputPix
    inc     r18                 ;blablabla
    mov     r16,        r18
    andi    r16,        1
    cpi     r16,        1       ;blablabla
    breq    -5                  ;blablabla
    cpi     r18,        64
    brpl    2
    rjmp    outputPix           ;blablabla
    jmp     loop
    inc     r17
    .def LALALALA = r18
    mov     BIT,        r17     ;blablabla
    andi    r16,        1
    mov     LALALALA,        r16
    jmp     loop
    .def BIT = r16