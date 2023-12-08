    ldi     r17,        0 
    ldi     r18,        0
    rjmp    outputAll           ;blablabla
    ldi     r21,        255
    ldi     r22,        255	    ;blablabla
    rjmp    outputPix
    inc     r18                 ;blablabla
    mov     r16,        r18
    andi    r16,        1
    cpi     r16,        1       ;blablabla
    breq    -5                  ;blablabla
    cpi     r18,        64
    brpl    2
    rjmp    outputPix           ;blablabla
    jmp     6
    inc     r17
    mov     r16,        r17     ;blablabla
    andi    r16,        1
    mov     r18,        r16
    jmp     6

