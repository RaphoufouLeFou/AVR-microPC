; 3d renderer in assembler

; RAM addresses : 
;
;   0x0100 : X size
;   0x0101 : X position
;   0x0102 : Y size
;   0x0103 : Y position
;   0x0104 : Z size
;   0x0105 : Z position
;   0x0106 : fov
;   0x0107 : textureL
;   0x0108 : textureH

;   0x0109 : temp7  aka dx
;   0x010A : temp6  aka dy
;   0x010B : temp5  aka addressY buffer
;   0x010C : temp4
;   0x010D : temp3
;   0x010E : temp2
;   0x010F : temp1

;   0x0110 : ret1
;   0x0111 : ret2
;   0x0112 : ret3
;   0x0113 : ret4
;   0x0114 : ret5

;   0x0115 : temp   aka sx
;   0x0116 : temp   aka sy
;   0x0117 : temp
;   0x0118 : temp
;   0x0119 : temp
;   0x011A : temp
;   0x011B : temp
;   0x011C : temp
;   0x011D : temp
;   0x011E : temp
;   0x011F : temp

;   0x0120 : vertex0X
;   0x0121 : vertex0Y
;   0x0122 : vertex0Z
;   0x0123 : vertex1X
;   0x0124 : vertex1Y
;   0x0125 : vertex1Z
;   0x0126 : vertex2X
;   0x0127 : vertex2Y
;   0x0128 : vertex2Z
;   0x0129 : vertex3X
;   0x012A : vertex3Y
;   0x012B : vertex3Z
;   0x012C : vertex4X
;   0x012D : vertex4Y
;   0x012E : vertex4Z
;   0x012F : vertex5X
;   0x0130 : vertex5Y
;   0x0131 : vertex5Z
;   0x0132 : vertex6X
;   0x0133 : vertex6Y
;   0x0134 : vertex6Z
;   0x0135 : vertex7X
;   0x0136 : vertex7Y
;   0x0137 : vertex7Z

;   0x0138 : point0X
;   0x0139 : point0Y
;   0x013A : point1X
;   0x013B : point1Y
;   0x013C : point2X
;   0x013D : point2Y
;   0x013E : point3X
;   0x013F : point3Y
;   0x0140 : point4X
;   0x0141 : point4Y
;   0x0142 : point5X
;   0x0143 : point5Y
;   0x0144 : point6X
;   0x0145 : point6Y
;   0x0146 : point7X
;   0x0147 : point7Y

;   0x0148 : playerX
;   0x0149 : playerY
;   0x014A : playerZ



;debug :
;    ldi ZL, LOW(end)
;    ldi ZH, HIGH(end)
;    mov AddressX, temp4
;    mov AddressY, temp4
;    subi AddressX, 0
;    subi AddressY, 0
;    lsl AddressY
;    lsl AddressY
;    clr empty
;    rjmp outputPix



    .include "m16adef.inc"

    .def drem16uL=r15       ; Divitions registers
    .def dres16uL=r13       ;
    .def dres16uH=r12       ;
    .def dd16uL	=r13        ;
    .def dd16uH	=r12        ;
    .def dv16uL	=r9         ;
    .def dcnt16u =r10       ;
    .def itt = r7           ; Itterator
    .def shift = r5         ; Cube shift for each frame
    .def Rfov = r6          ; fov register

    .def PposX = r8         ; Player position registers
    .def PposY = r11        ;
    .def PposZ = r14        ;

	.def oLoopR = r4        ; Delay registers
	.def iLoopRl = r3       ;
	.def iLoopRh = r2       ;

    .def DataL = r22        ; Pixel color registers
	.def DataH = r21        ; 
	.def AddressX = r17     ; Pixel address registers
	.def AddressY = r18     ;
    .def writeEnable = r23  ; Write Constant enable register
    .def clock = r24        ; Clock Constant register
    .def IsW = r25          ; Input register buffer
	.def empty = r16        ; empty register
	.def temp2 = r20        ; temporary register
	.def temp = r19         ;
    .def temp3 = r25    ; duplicate !
    .def temp4 = r16    ; duplicate ! remember to clr after use


	.equ	oVal 	= 2		        ;Delay
	.equ	iVal 	= 0xFFFF		;Precision of delay

    clr itt                 ;reset registers to 0x00
    clr DataL				;
	clr DataH               ;
    clr AddressX            ;
	clr AddressY            ;
    clr empty               ;
    clr shift               ;   
    ldi writeEnable, 0b10000000 ;set write enable constant
    ldi clock, 0x02             ;set clock constant
    ldi temp, 0xFF          ;set temp to 0xFF

    out DDRC, temp			;set the output pin to output
	out DDRB, temp          ;
	out DDRD, temp          ;
    ldi temp, 0b00000010    ;
    out DDRA, temp          ;

    ldi temp, DefPposX      ;set the player position
    mov PposX, temp         ;
    ldi temp, DefPposY      ;
    mov PposY, temp         ;
    ldi temp, DefPposZ      ;
    mov PposZ, temp         ;

	ldi	ZL, LOW(starting)   ;set the Z pointer to the starting point
    ldi	ZH, HIGH(starting)  ;
    rjmp outputAll          ; clear the screen, because DataL and DataH are 0x00

drawLineTest:               ;debug function, draws a line from 20,20 to 50,10

    ldi DataL, 0xF0         ;set the color to red
    ldi DataH, 0x0F         ;
    ldi ZL, LOW(ici)        ;set the Z pointer to the return point
    ldi ZH, HIGH(ici)       ;
    ldi AddressX, 20        ;set the X position to 20
    ldi AddressY, 20        ;set the Y position to 20
    lsl AddressY            ;shift the Y position, to let the TX and RX pins free
    lsl AddressY            ;
    rjmp outputPix          ;output the pixel

ici:                        ;return point
    ldi ZL, LOW(ici2)       ;set the Z pointer to the second return point
    ldi ZH, HIGH(ici2)      ;
    ldi AddressX, 50        ;set the X position to 50
    ldi AddressY, 10        ;set the Y position to 10
    lsl AddressY            ;shift the Y position, to let the TX and RX pins free
    lsl AddressY            ;
    rjmp outputPix          ;output the pixel

ici2:
    ldi DataL, 0xFF         ;set the color to white
    ldi DataH, 0xFF         ;
    ldi XL, 0x07            ;set the X pointer to the texture
    ldi XH, 0x01            ;
    st X+, DataL            ;store the first 8bit of the texture
    st X, DataH             ;store the last 8bit of the texture
    ldi XL, 0x38            ;set the X pointer to the pixel address
    ldi temp, 20            ;set the position of the pixels
    st X+, temp             ;
    ldi temp, 20            ;
    st X+, temp             ;
    ldi temp, 70            ;
    st X+, temp             ;
    ldi temp, 50            ;
    st X+, temp             ;
    ldi temp, 20            ;
    st X+, temp             ;
    ldi temp, 20            ;
    st X+, temp             ;
    ldi temp, 50            ;
    st X+, temp             ;
    ldi temp, 10            ;
    st X+, temp             ;
    ldi temp, 30            ;
    st X+, temp             ;
    ldi temp, 50            ;
    st X+, temp             ;
    ldi temp, 80            ;
    st X+, temp             ;
    ldi temp, 60            ;
    st X+, temp             ;
    ldi temp, 90            ;
    st X+, temp             ;
    ldi temp, 60            ;
    st X+, temp             ;
    ldi temp, 100           ;
    st X+, temp             ;
    ldi temp, 60            ;
    st X, temp              ;
    ldi XL, 0x38
    ldi XH, 0x01    
    ;rjmp testPrintpoints
    rjmp beforeLine         ;jump to the line drawing function

here:                       ;debug function, to test the divition function
    ldi temp, 0x04          ;set some number to test
    mov dd16uL, temp        ;
    ldi temp, 0x02          ;
    mov dv16uL, temp        ;
    clr dd16uH              ;
    rcall	div16u          ;call the divition function
            
	ldi	ZL, LOW(end)        ;set the Z pointer to the end point
    ldi	ZH, HIGH(end)       ;
    ldi DataH, 0xFF         ;set the color to white
    ldi DataL, 0xFF         ;
    clr empty
    mov AddressX, dres16uL  ;set the X position to the result of the divition
    mov AddressY, dres16uL  ;set the Y position to the result of the divition
    subi AddressX, 2        ;set the exepted result, so if they are equal, the pixel will be at 0,0
    subi AddressY, 2        ;
    lsl AddressY            ;shift the Y position, to let the TX and RX pins free
    lsl AddressY            ;
    rjmp outputPix          ;output the pixel

GetInput:
    in temp, PINA           ;get the input from PORTA
    andi temp, 0b11111100   ;apply a mask to get the first bit
    mov temp2, temp         ;store the input in temp2
    andi temp2, 0b00000100  ;apply a mask to get the third bit
    cpi temp2, 0x00         ;check if the second bit is 0
    breq skipDoZ            ;if it's 0, skip the Z position change
    dec PposZ               ;decrease the Z position
skipDoZ:
    mov temp2, temp         ;store the input in temp2
    andi temp2, 0b00001000  ;apply a mask to get the fourth bit
    cpi temp2, 0x00         ;check if the fourth bit is 0
    breq skipUpZ            ;if it's 0, skip the X position change
    inc PposZ               ;decrease the X position
skipUpZ:
    mov temp2, temp         ;store the input in temp2
    andi temp2, 0b01000000  ;apply a mask to get the seventh bit
    cpi temp2, 0x00         ;check if the second bit is 0
    breq skipDoX            ;if it's 0, skip the X position change
    inc PposX               ;decrease the X position
skipDoX:
    mov temp2, temp         ;store the input in temp2
    andi temp2, 0b00010000  ;apply a mask to get the eighth bit
    cpi temp2, 0x00         ;check if the fourth bit is 0
    breq skipUpX            ;if it's 0, skip the X position change
    dec PposX               ;decrease the X position
skipUpX:
    mov temp2, temp         ;store the input in temp2
    andi temp2, 0b10000000  ;apply a mask to get the second bit
    cpi temp2, 0x00         ;check if the second bit is 0
    breq skipDoY            ;if it's 0, skip the Y position change
    inc PposY               ;decrease the Y position
skipDoY:
    mov temp2, temp         ;store the input in temp2
    andi temp2, 0b00100000  ;apply a mask to get the fifth bit
    cpi temp2, 0x00         ;check if the fourth bit is 0
    breq skipUpY            ;if it's 0, skip the Y position change
    dec PposY               ;decrease the Y position
skipUpY:
    ijmp                    ;return to the Z pointer address


starting:
    ldi XL, 0x00            ;Get the X size and position
    ldi XH, 0x01            ;
    ldi temp, DefXsize    ;X size
    st X+, temp
    ldi temp, DefXpos     ;X position
    st X+, temp
    ldi temp, DefYsize    ;Y size
    st X+, temp
    ldi temp, DefYpos     ;Y position
    st X+, temp
    ldi temp, DefZsize    ;Z size
    st X+, temp
    ldi temp, DefZpos     ;Z position
    st X+, temp
    ldi temp, Deffov    ;fov
    mov Rfov, temp
    st X+, temp
    ldi temp, LOW(Deftexture)  ;textureL
    st X+, temp
    ldi temp, HIGH(Deftexture)  ;textureH
    st X+, temp
    rjmp drawRect       ;jump to the drawRect function
    



end:
    ;jmp end
    ldi ZL, LOW(endGetInput)   ;set the Z pointer to the endGetInput point
    ldi ZH, HIGH(endGetInput)  ;
    rjmp GetInput              ;get the input from the user
endGetInput:
    dec shift           ;decrease the shift
    ldi temp, -20       
    cp shift, temp      ;compare the shift to -20
    brne skipclearItt   ;if it's not -20, jump to the whait loop
    ldi temp, 20        ;if it's -20, set the shift to 20
    mov shift, temp     ;
    

skipclearItt:
    ;jmp starting
    ldi temp, oVal      ;set the delay value
	mov	oLoopR, temp    ;set the delay register

oLoop:	

    ldi temp, LOW(iVal)     ;set the precision of the delay
    mov	iLoopRl, temp       ;set the delay register
    ldi temp, HIGH(iVal)    ;
	mov	iLoopRh, temp       ;

iLoopl:	

    dec	iLoopRl             ;decrease the delay register
    brne iLoopl             ;if it's not 0, jump to the next iteration
    ldi temp, LOW(iVal)     ;set the precision of the delay
    mov	iLoopRl, temp       ;set the delay register
iLooph:

    dec	iLoopRh	            ;decrease the delay register
	brne iLoopl	            ;if it's not 0, jump to the next iteration
	dec	oLoopR	            ;decrease the delay register
	brne oLoop	            ;if it's not 0, jump to the next iteration

    rjmp starting           ;jump to the starting point


outputPix:                  ;output the pixel to the screen
	in IsW, PINA			;read PORTA
	andi IsW, 0b00000001	;apply a mask to get the first bit
	subi IsW, 0x01			;substact 1
	brne outputPix			;jump to output if it's not nul, so when input is 0
	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, clock		;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, empty		;toggle the clock 
	out PORTD, AddressY		;output the Y address to PORTD
	add AddressX, writeEnable	;add the write enable constant to the X address
	out PORTB,AddressX          ;output the X address to PORTB
	sub AddressX, writeEnable   ;substract the write enable constant from the X address
	out PORTB, empty            ;Clear the ports
	out PORTD, empty            ;
	out PORTC, empty            ;
	out PORTA, clock			;toggle the clock 
	out PORTA, empty			;toggle the clock 
	ijmp                    ;return to the Z pointer address


outputAll:

	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, clock		;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, empty		;toggle the clock 
whait:
	in IsW, PINA			;read PORTA
	andi IsW, 0b00000001	;apply a mask to get the first bit
	subi IsW, 0x01			;substact 1
	brne whait				;jump to output if it's not nul, so when input is 0
	out PORTD, AddressY     ;output the Y address to PORTD
	add AddressX, writeEnable   ;add the write enable constant to the X address
	out PORTB,AddressX          ;output the X address to PORTB
	sub AddressX, writeEnable   ;substract the write enable constant from the X address
	out PORTB,AddressX      ;output the X address to PORTB
	inc AddressX            ;increase the X address
	cpi AddressX, 0x80      ;compare the X address to 0x80
	brne whait              ;if it's not 0x80, jump to the whait loop
    
    clr AddressX            ;reset the X address to 0
	cpi AddressY, 0xFC      ;compare the Y address to 0xFC
	out PORTC, empty	    ;Clear the ports
	breq start              ;if it's 0xFC, jump to the start point
    subi AddressY, -0b00000100  ;add 4 from the Y address
	rjmp whait              ;jump to the whait loop
start: 
	clr AddressX            ;reset the address to 0
	clr AddressY            ;
	ijmp                    ;return to the Z pointer address

drawRect:               ;draw a cube
    ldi XL, 0x10        ;set the X pointer 
    ldi XH, 0x01
    st X+, ZL           ;store the Z pointer 
    st X, ZH
    ldi DataL, 0x00     ;set the color to black
    ldi DataH, 0x00     
    clr AddressX        ;reset the X address to 0
    clr AddressY        
    ldi ZL, LOW(retDrawTerrain)     ;set the Z pointer to the draw terrain function
    ldi ZH, HIGH(retDrawTerrain)    
    rjmp outputAll      ; clear the screen, because DataL and DataH are 0x00

retDrawTerrain:

    ; X vertex calculation

    ldi XL, 0x00    ;get X size and position
    ld temp, X+     ;temp2 = X size
    ld temp2, X     ;temp = X position
    
    lsr temp
    add temp2, temp ;temp2 = X size + X position/2

    ldi XL, 0x23    ;store the X vertex coordinates
    st X, temp2     ;store the vertex1X
    ldi XL, 0x29    
    st X, temp2     ;store the vertex3X
    ldi XL, 0x2F    
    st X, temp2     ;store the vertex5X
    ldi XL, 0x35    
    st X, temp2     ;store the vertex7X

    ldi XL, 0x00    ;get X size and position
    ld temp, X+    ;temp2 = X size
    ld temp2, X      ;temp = X position

    lsr temp
    sub temp2, temp ;temp2 = X size - X position/2

    ldi XL, 0x20    ;store the X vertex coordinates
    st X, temp2     ;store the vertex0X
    ldi XL, 0x26    
    st X, temp2     ;store the vertex2X
    ldi XL, 0x2C
    st X, temp2     ;store the vertex4X
    ldi XL, 0x32
    st X, temp2     ;store the vertex6X

    ; Y vertex calculation

    ldi XL, 0x02    ;get Y size and position
    ld temp, X+    ;temp2 = Y size
    ld temp2, X      ;temp = Y position

    lsr temp
    add temp2, temp ;temp2 = Y size + Y position/2

    ldi XL, 0x27    ;store the Y vertex coordinates
    st X, temp2     ;store the vertex2Y
    ldi XL, 0x2A
    st X, temp2     ;store the vertex3Y
    ldi XL, 0x33
    st X, temp2     ;store the vertex6Y
    ldi XL, 0x36
    st X, temp2     ;store the vertex7Y

    ldi XL, 0x02    ;get Y size and position
    ld temp, X+    ;temp2 = Y size
    ld temp2, X      ;temp = Y position

    lsr temp
    sub temp2, temp ;temp2 = Y size - Y position/2

    ldi XL, 0x21    ;store the Y vertex coordinates
    st X, temp2     ;store the vertex0Y
    ldi XL, 0x24 
    st X, temp2     ;store the vertex1Y
    ldi XL, 0x2D
    st X, temp2     ;store the vertex4Y
    ldi XL, 0x30 
    st X, temp2     ;store the vertex5Y

    ; Z vertex calculation

    ldi XL, 0x04    ;get Z size and position
    ld temp, X+    ;temp2 = Z size
    ld temp2, X      ;temp = Z position

    lsr temp
    add temp2, temp ;temp2 = Z size + Z position/2

    ldi XL, 0x2E    ;store the Z vertex coordinates
    st X, temp2     ;store the vertex4Z
    ldi XL, 0x31
    st X, temp2     ;store the vertex5Z
    ldi XL, 0x34 
    st X, temp2     ;store the vertex6Z
    ldi XL, 0x37 
    st X, temp2     ;store the vertex7Z

    ldi XL, 0x04    ;get Z size and position
    ld temp, X+    ;temp2 = Z size
    ld temp2, X      ;temp = Z position

    lsr temp
    sub temp2, temp ;temp2 = Z size - Z position/2

    ldi XL, 0x22    ;store the Z vertex coordinates
    st X, temp2     ;store the vertex0Z
    ldi XL, 0x25 
    st X, temp2     ;store the vertex1Z
    ldi XL, 0x28 
    st X, temp2     ;store the vertex2Z
    ldi XL, 0x2B 
    st X, temp2     ;store the vertex3Z

    ldi XL, 0x06    ;get fov
    ld AddressY, X  ;store the fov
    ldi XL, 0x38    ;set the X pointer to the address of the start of the pixel array
    clr itt         ;reset the itterator

endLoadColor:
    ldi XL, 0x07   ;set the X pointer to the texture
    ld DataL, X+    ;store the first 8bit of the texture
    ld DataH, X     ;store the last 8bit of the texture
    ;rjmp beforeLine

    ldi XL, 0x38    ;set the X pointer to the address of the start of the pixel array
    ldi YL, 0x20    ;set the Y pointer to the address of the start of the vertex array
    ldi YH, 0x01    ;

proj:
    ; Projection of the vertices on the screen
    
    ld temp, Y+     ;get the vertex X
    ld temp2, Y+    ;get the vertex Y
    ld temp3, Y+    ;get the vertex Z

    sub temp , PposX       ;Get the relative position of the vertex to the player
    sub temp2, PposY       ; 
    sub temp3, PposZ       ;

    cpi temp2, 0        ;Check if the vertex is behind the player
    breq isZero         ;if it's 0, jump to the isZero function
    cpi temp2, 0        ;Get the absolute value of the vertex
    brmi isNeg          ;if it's negative, jump to the isNeg function
    rjmp calcproj       ;if it's positive, jump to the calcproj function

isZero:
    ldi temp2, 1        ;set the vertex to 1, so it will not divide by 0
isNeg:
    neg temp2           ;get the absolute value of the vertex

calcproj:
    clr temp4           ;reset temp4 to 0
    cpi temp, 0x80      ;check if the vertex is negative
    brmi isPos          ;if it's posotive, jump to the isPos function
    neg temp            ;get the absolute value of the vertex
    ldi temp4, 0x01     ;set temp4 to 1, so it will remember that the vertexX was negative
isPos:

    mul Rfov, temp      ;multiply the vertex by the fov
    mov temp, temp2     ;store the vertex in temp

    mov dd16uL, r0      ;store the multiplication result in the divition register
    mov dd16uH, r1      ;
    mov dv16uL, temp    ;store the vertexY in the divisor register

    rcall div16u        ;call the divition function

retFirstDiv:
    
    ldi temp, 64        ;set the offset to 64
    cpi temp4, 0x01     ;check if the vertexX was negative
    breq isNpo3         ;if it's negative, jump to the isNpo3 function
    neg dres16uL        ;get the absolute value of the result
isNpo3:
    add dres16uL, temp  ;add the offset to the result, so it will be in the middle of the screen
    st X+, dres16uL     ; Store the projected vertices

    clr temp4           ;reset temp4 to 0
    cpi temp3, 0x80     ;check if the vertex is negative
    brmi isPos2         ;if it's posotive, jump to the isPos2 function
    neg temp3           ;get the absolute value of the vertex
    ldi temp4, 0x01     ;set temp4 to 1, so it will remember that the vertexX was negative
isPos2:
    mul temp3, Rfov     ;multiply the vertex by the fov
    mov dd16uL, r0      ;store the multiplication result in the divition register
    mov dd16uH, r1      ;
    rcall div16u        ;call the divition function
retSecondDiv:
    ; Store the projected vertices

    ldi temp, 32        ;set the offset to 32
    cpi temp4, 0x01     ;check if the vertexX was negative
    breq isNeg4         ;if it's negative, jump to the isNeg4 function
    neg dres16uL        ;get the absolute value of the result
isNeg4:

    add dres16uL, temp  ;add the offset to the result, so it will be in the middle of the screen
    st X+, dres16uL     ; Store the projected vertices
    clr dres16uL        ;reset the result to 0
    inc itt             ;increase the itterator
    ldi temp, 8         
    cp itt, temp        ;check if the itterator is 8
    brne proj           ;if it's not 8, jump to the proj function
    clr itt             ;reset the itterator to 0
    ldi XL, 0x38        ;set the X pointer to the address of the start of the pixel array
    ldi XH, 0x01        ;
    rjmp beforeLine     ;jump to the line drawing function


;-------------------------PART 1-------------------------;





beforeLine:             ;draw the lines between the vertices
    clr itt             ;reset the itterator to 0
    ldi YL, 0x38        ;set the Y pointer to the address of the start of the pixel array
    ldi YH, 0x01        ;
drawLine:

    
    ld AddressX,Y+      ;get the X address of the first vertex
    ld AddressY,Y+      ;get the Y address of the first vertex
    ld temp,    Y+      ;get the X address of the second vertex
    ld temp2,   Y       ;get the Y address of the second vertex
    mov temp3, temp     ;store the X address of the second vertex
    mov temp4, temp2    ;store the Y address of the second vertex
    sub temp, AddressX  ;get the distance between the two vertices
    brpl skipAbs        ;if it's positive, jump to the skipAbs function
    neg temp            ;get the absolute value of the distance
skipAbs:
    sub temp2, AddressY ;get the distance between the two vertices

    brpl skipAbs21      ;if it's positive, jump to the skipAbs2 function
    neg temp2           ;get the absolute value of the distance
skipAbs21:


    ldi XL, 0x09        ;store the distance between the two vertices
    st X+, temp         ;
    st X, temp2         ;
    clr temp            ;reset temp to 0
    cp temp3, AddressX  ;check if the second vertex is less the first vertex
    brmi skipneg1       ;if it's negative, jump to the skipneg1 function
    ldi temp, 0x02      ;set temp to 2, so it will be 1 or -1 after a subi 1
skipneg1:
    subi temp, 0x01     ;substract 1 from temp
    clr temp2           ;reset temp2 to 0
    cp temp4, AddressY  ;check if the second vertex is less the first vertex
    brmi skipneg2       ;if it's negative, jump to the skipneg2 function
    ldi temp2, 0x02     ;set temp2 to 2, so it will be 1 or -1 after a subi 1
skipneg2:
    subi temp2, 0x01    ;substract 1 from temp2
    ldi XL, 0x15        ;store the distance between the two vertices
    st X+, temp         ;
    st X, temp2         ;
    ldi XL, 0x09        ;get the distance between the two vertices
    ld temp, X+         ;store the distance between the two vertices
    ld temp2, X         ;
    sub temp, temp2     ;get the difference between the two distances
lineloop:
    ldi ZL, LOW(lineRetLoop)    ;set the Z pointer to the return point
    ldi ZH, HIGH(lineRetLoop)   ;
    ldi XL, 0x0B
    st X, AddressY
    cpi AddressY, 0x3F
    brpl lineRetLoop
    lsl AddressY        ;shift the Y address, to let the TX and RX pins free
    lsl AddressY        ;
    clr empty           ;reset empty to 0
    clr temp3           ;reset temp3 to 0
    rjmp outputPix      ;output the pixel
lineRetLoop:
    ;jmp end
    ld AddressY, X      ;restore the Y address

    mov temp2, temp     ;store the distance between the two vertices
    lsl temp2           ;multiply the distance by 2
    ldi XL, 0x0A        ;get the temp value
    ld temp3, X         ;
    ldi XL, 0x15        ;
    ld temp4, X         ;


    cpi temp2, 0x80     ;check if the distance is negative
    brmi first          ;if it's negative, jump to the first function
    neg temp2           ;get the absolute value of the distance
    neg temp3           ;get the absolute value of the temp value
    cp temp2, temp3     ;check if the distance is less then the temp value
    neg temp2           ;get the absolute value of the distance
    neg temp3           ;get the absolute value of the temp value
    brpl endfirst       ;if it's positive, jump to the endfirst function
first:

    sub temp, temp3     ;remove the temp value from the error 
    add AddressX, temp4 ;add the temp value to the X address (-1 or 1)
endfirst:

    ldi XL, 0x09        ;get the temp value
    ld temp3, X         ;
    ldi XL, 0x16        ;
    ld temp4, X         ;

    cpi temp3, 0x80     ;check if the temp value is negative
    brpl second         ;if it's positive, jump to the second function
    cp temp2, temp3     ;check if the distance is less then the temp value
    brpl endsecond      ;if it's positive, jump to the endsecond function
second:
    add temp, temp3     ;add the temp value to the error
    add AddressY, temp4 ;add the temp value to the Y address (-1 or 1)
    
endsecond:

    ;jmp lineloop
    ld temp3, -Y        ;get the distance between the two vertices
    ld temp3, Y+        ;
    ld temp4, Y         ;
    cpi AddressX, 0x80  ;check if the X address is overflown
    brpl beforeLine2          ;if it's overflown, jump to the beforeLine2 function
    cp AddressX, temp3  ;check if the X address is equal to the distance between the two vertices
    brne lineloop            ;lineloop
    cp AddressY, temp4  ;check if the Y address is equal to the distance between the two vertices
    brne lineloop            ;lineloop
goEnd:

    inc itt             ;increase the itterator
    ld temp4, Y+        ;increase the Y pointer by 1
    ldi temp4, 4        ;set temp4 to 4
    cp itt, temp4       ;check if the itterator is 4
    brpl beforeLine2    ;if it's 4, jump to the beforeLine2 function
    rjmp drawLine       ;else jump to the drawLine function




;-------------------------PART 2-------------------------;
; same as above, but with different vertex coordinates

beforeLine2:
    clr itt
    ldi YL, 0x38
    ldi YH, 0x01
drawLine2:

    
    ld AddressX,Y+
    ld AddressY,Y+
    ld temp,    Y+
    ld temp2,   Y+
    ld temp,    Y+
    ld temp2,   Y
    mov temp3, temp
    mov temp4, temp2
    sub temp, AddressX
    brpl skipAbs2
    neg temp
skipAbs2:
    sub temp2, AddressY

    brpl skipAbs22
    neg temp2
skipAbs22:


    ldi XL, 0x09
    st X+, temp
    st X, temp2
    clr temp
    cp temp3, AddressX
    brmi skipneg12
    ldi temp, 0x02
skipneg12:
    subi temp, 0x01
    clr temp2
    cp temp4, AddressY
    brmi skipneg22
    ldi temp2, 0x02
skipneg22:
    subi temp2, 0x01
    ldi XL, 0x15
    st X+, temp
    st X, temp2
    ldi XL, 0x09
    ;add XL, itt
    ld temp, X+
    ld temp2, X
    sub temp, temp2
lineloop2:
    ldi ZL, LOW(lineRetLoop2)
    ldi ZH, HIGH(lineRetLoop2)
    ldi XL, 0x0B
    st X, AddressY
    cpi AddressY, 0x3F
    brpl lineRetLoop2
    lsl AddressY
    lsl AddressY
    clr empty
    clr temp3
    rjmp outputPix
lineRetLoop2:
    ;jmp end
    ld AddressY, X      ;restore the Y address

    mov temp2, temp
    lsl temp2
    ldi XL, 0x0A
    ld temp3, X
    ldi XL, 0x15
    ld temp4, X


    cpi temp2, 0x80
    brmi first2
    neg temp2
    neg temp3
    cp temp2, temp3 
    neg temp2
    neg temp3
    brpl endfirst2
first2:

    sub temp, temp3
    add AddressX, temp4
endfirst2:

    ldi XL, 0x09
    ld temp3, X
    ldi XL, 0x16
    ld temp4, X

    cpi temp3, 0x80
    brpl second2
    cp temp2, temp3
    brpl endsecond2
second2:
    add temp, temp3
    add AddressY, temp4
endsecond2:

    ;jmp lineloop
    ld temp3, -Y
    ld temp3, Y+
    ld temp4, Y
    cpi AddressX, 0x80  ;check if the X address is overflown
    brpl beforeLine3          ;if it's overflown, jump to the beforeLine2 function
    cp AddressX, temp3
    brne lineloop2            ;lineloop
    cp AddressY, temp4 
    brne lineloop2            ;lineloop
goEnd2:

    inc itt
    ld temp4, -Y
    ld temp4, -Y
    ld temp4, -Y
    ldi temp4, 2
    cp itt, temp4
    brpl beforeLine3
    rjmp drawLine2




;-------------------------PART 3-------------------------;
; same as above, but with different vertex coordinates




beforeLine3:
    clr itt
    ldi YL, 0x40
    ldi YH, 0x01
drawLine3:

    
    ld AddressX,Y+
    ld AddressY,Y+
    ld temp,    Y+
    ld temp2,   Y+
    ld temp,    Y+
    ld temp2,   Y
    mov temp3, temp
    mov temp4, temp2
    sub temp, AddressX
    brpl skipAbs3
    neg temp
skipAbs3:
    sub temp2, AddressY

    brpl skipAbs23
    neg temp2
skipAbs23:


    ldi XL, 0x09
    st X+, temp
    st X, temp2
    clr temp
    cp temp3, AddressX
    brmi skipneg13
    ldi temp, 0x02
skipneg13:
    subi temp, 0x01
    clr temp2
    cp temp4, AddressY
    brmi skipneg23
    ldi temp2, 0x02
skipneg23:
    subi temp2, 0x01
    ldi XL, 0x15
    st X+, temp
    st X, temp2
    ldi XL, 0x09
    ;add XL, itt
    ld temp, X+
    ld temp2, X
    sub temp, temp2
lineloop3:
    ldi ZL, LOW(lineRetLoop3)
    ldi ZH, HIGH(lineRetLoop3)
    ldi XL, 0x0B
    st X, AddressY
    cpi AddressY, 0x3F
    brpl lineRetLoop3
    lsl AddressY
    lsl AddressY
    clr empty
    clr temp3
    rjmp outputPix
lineRetLoop3:
    ;jmp end
    ld AddressY, X      ;restore the Y address

    mov temp2, temp
    lsl temp2
    ldi XL, 0x0A
    ld temp3, X
    ldi XL, 0x15
    ld temp4, X


    cpi temp2, 0x80
    brmi first3
    neg temp2
    neg temp3
    cp temp2, temp3 
    neg temp2
    neg temp3
    brpl endfirst3
first3:

    sub temp, temp3
    add AddressX, temp4
endfirst3:

    ldi XL, 0x09
    ld temp3, X
    ldi XL, 0x16
    ld temp4, X

    cpi temp3, 0x80
    brpl second3
    cp temp2, temp3
    brpl endsecond3
second3:
    add temp, temp3
    add AddressY, temp4
endsecond3:

    ;jmp lineloop
    ld temp3, -Y
    ld temp3, Y+
    ld temp4, Y
    cpi AddressX, 0x80  ;check if the X address is overflown
    brpl beforeLine4          ;if it's overflown, jump to the beforeLine2 function
    cp AddressX, temp3
    brne lineloop3            ;lineloop
    cp AddressY, temp4 
    brne lineloop3            ;lineloop
goEnd3:

    inc itt
    ld temp4, -Y
    ld temp4, -Y
    ld temp4, -Y
    ldi temp4, 2
    cp itt, temp4
    brpl beforeLine4
    rjmp drawLine3




;-------------------------PART 4-------------------------;
; same as above, but with different vertex coordinates


beforeLine4:
    clr itt
    ldi YL, 0x38
    ldi YH, 0x01
drawLine4:

    
    ld AddressX,Y+
    ld AddressY,Y+
    ld temp,    Y+
    ld temp2,   Y+
    ld temp,    Y+
    ld temp2,   Y+
    ld temp,    Y+
    ld temp2,   Y+
    ld temp,    Y+
    ld temp2,   Y
    mov temp3, temp
    mov temp4, temp2
    sub temp, AddressX
    brpl skipAbs4
    neg temp
skipAbs4:
    sub temp2, AddressY

    brpl skipAbs24
    neg temp2
skipAbs24:


    ldi XL, 0x09
    st X+, temp
    st X, temp2
    clr temp
    cp temp4, AddressX
    brmi skipneg14
    ldi temp, 0x02
skipneg14:
    subi temp, 0x01
    clr temp2
    cp temp4, AddressY
    brmi skipneg24
    ldi temp2, 0x02
skipneg24:
    subi temp2, 0x01
    ldi XL, 0x15
    st X+, temp
    st X, temp2
    ldi XL, 0x09
    ;add XL, itt
    ld temp, X+
    ld temp2, X
    sub temp, temp2
lineloop4:
    ldi ZL, LOW(lineRetLoop4)
    ldi ZH, HIGH(lineRetLoop4)
    ldi XL, 0x0B
    st X, AddressY
    cpi AddressY, 0x3F
    brpl lineRetLoop4
    lsl AddressY
    lsl AddressY
    clr empty
    clr temp3
    rjmp outputPix
lineRetLoop4:
    ;jmp end
    ld AddressY, X      ;restore the Y address

    mov temp2, temp
    lsl temp2
    ldi XL, 0x0A
    ld temp3, X
    ldi XL, 0x15
    ld temp4, X


    cpi temp2, 0x80
    brmi first4
    neg temp2
    neg temp3
    cp temp2, temp3 
    neg temp2
    neg temp3
    brpl endfirst4
first4:

    sub temp, temp3
    add AddressX, temp4
endfirst4:

    ldi XL, 0x09
    ld temp3, X
    ldi XL, 0x16
    ld temp4, X

    cpi temp3, 0x80
    brpl second4
    cp temp2, temp3
    brpl endsecond4
second4:
    add temp, temp3
    cpi AddressY, 0x00
    brpl lineRetLoop
    add AddressY, temp4
endsecond4:

    ;jmp lineloop
    ld temp3, -Y
    ld temp3, Y+
    ld temp4, Y
    cpi AddressX, 0x80  ;check if the X address is overflown
    brpl PointDraw          ;if it's overflown, jump to the beforeLine2 function
    cp AddressX, temp3
    brne lineloop4            ;lineloop
    cp AddressY, temp4 
    brne lineloop4            ;lineloop
goEnd4:

    inc itt
    ld temp4, -Y
    ld temp4, -Y
    ld temp4, -Y
    ld temp4, -Y
    ld temp4, -Y
    ld temp4, -Y
    ld temp4, -Y
    ldi temp4, 4
    cp itt, temp4
    brpl PointDraw
    rjmp drawLine4





PointDraw:            ;draw the points at the vertices in red

    ldi DataH, HIGH(0b0000011111100000) ;set the color to red
    ldi DataL, LOW(0b0000011111100000)  ;
    ldi XL, 0x38            ;set the X pointer to the address of the start of the pixel array
    ldi XH, 0x01            ;
    clr itt                 ;reset the itterator

testPrintpoints:
    ld AddressX, X+     ;get the X address of the vertex
    ld AddressY, X+     ;
    clr empty           ;reset empty to 0
    ldi ZL, LOW(endPrintpoints)     ;set the Z pointer to the return point
    ldi ZH, HIGH(endPrintpoints)    ;
    lsl AddressY    ;shift the Y address, to let the TX and RX pins free
    lsl AddressY    ;
    rjmp outputPix  ;output the pixel

endPrintpoints:
    inc itt         ;increase the itterator
    ldi temp, 8     ;set temp to 8
    cp itt, temp    ;check if the itterator is 8
    brne testPrintpoints    ;if it's not 8, jump to the testPrintpoints function
    rjmp end        ;else jump to the end function


div16u:	
   ;ldi temp, 8
   ;mov dd16uL, temp
   ;ldi temp, 2
   ;mov dv16uL, temp

    clr	drem16uL		;clear remainder Low byte
	ldi r24, 17
    mov	dcnt16u, r24		;init loop counter
    ldi r24, 0x02
d16u_1:	rol	dd16uL			;shift left dividend
	rol	dd16uH
	dec	dcnt16u			;decrement counter
	brne	d16u_2			;if done

	ret				;    return
d16u_2:	rol	drem16uL		;shift dividend into remainder
	sub	drem16uL,dv16uL		;remainder = remainder - divisor
	brcc	d16u_3			;if result negative
	add	drem16uL,dv16uL		;    restore remainder
	clc				;    clear carry to be shifted into result
	rjmp	d16u_1			;else
d16u_3:	sec				;    set carry to be shifted into result
	rjmp	d16u_1


.dseg                   ;RAM allocation
    .org 0x0100

Xsize:  .byte 1
Xpos:   .byte 1
Ysize:  .byte 1
Ypos:   .byte 1
Zsize:  .byte 1
Zpos:   .byte 1
fov:    .byte 1
texture:.byte 2
tempA:  .byte 7
returnA:.byte 5
temp2A: .byte 11
vertexs:.byte 24
points: .byte 16
Ppos:   .byte 3


.cseg                   ;Cube parameters

.equ DefXsize = 10
.equ DefYsize = 10
.equ DefZsize = 10
.equ DefXpos = 0
.equ DefYpos = 20
.equ DefZpos = 0
.equ Deffov = 40
.equ Deftexture = 0xFFFF

;Player parameters

.equ DefPposX = 0
.equ DefPposY = 0
.equ DefPposZ = 0