	.include "m16adef.inc"

	.def DataL = r22
	.def DataH = r21
	.def AddressX = r17
	.def AddressY = r18
	.def IsW = r25
	.def lettre = r24
	.def Pixel = r23

	.equ posX = 50
	.equ posY = 100

	clr Pixel
	Clr lettre
	clr DataL				;reset registers to 0x00
	clr DataH
	clr AddressX
	clr AddressY
	ldi r19, 0xff			;load some useful registers
	ldi r29, 0x02
	ldi r31, 0x00
	ldi r20, 0x03
	ldi r16, 0x04
	ldi r30, 0b10000000
	ldi r28, 0b00000100
	ldi r26, posX
	ldi r27, posY
	mul r27, r16
	mov r27, r0

	out DDRC, r19			;set the output pin to output
	out DDRB, r19
	out DDRD, r19
	out DDRA, r29
	ldi	DataL, 0x00
	ldi	DataH, 0x00
	rjmp outputAll

strat:
	ldi	DataL, 0xff
	ldi	DataH, 0xff
	cpi lettre, 0
	breq rH
	cpi lettre, 1
	breq rE
	cpi lettre, 2
	breq rL
	cpi lettre, 3
	breq rL
	cpi lettre, 4
	breq rO
	cpi lettre, 5
	breq rBl
	cpi lettre, 6
	breq rL
	cpi lettre, 7
	breq rE
	cpi lettre, 8
	breq rO
	cpi lettre, 9
	breq rBl
	cpi lettre, 10
	breq rL
	cpi lettre, 11
	breq rO
	cpi lettre, 12
	breq rL
	rjmp loop

rH: jmp H
rE: jmp E
rL: jmp L
rO: jmp O
rBL:add AddressX, r20
	inc AddressX
	inc lettre
	rjmp strat

loop:
	rjmp loop

outputPix:

	in IsW, PINA			;read PORTA
	andi IsW, 0b00000001	;apply a mask to get the first bit
	subi IsW, 0x01			;substact 1
	brne outputPix			;jump to output if it's not nul, so when input is 0
	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, r29			;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, r31			;toggle the clock 
	out PORTD, AddressY
	add AddressX, r30		
	out PORTB,AddressX
	sub AddressX, r30
	out PORTB, r31
	out PORTD, r31
	out PORTC, r31	
	out PORTA, r29			;toggle the clock 
	out PORTA, r31			;toggle the clock 
	rjmp strat

outputAll:

	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, r29			;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, r31			;toggle the clock 
whait:
	in IsW, PINA			;read PORTA
	andi IsW, 0b00000001	;apply a mask to get the first bit
	subi IsW, 0x01			;substact 1
	brne whait				;jump to output if it's not nul, so when input is 0
	out PORTD, AddressY
	add AddressX, r30		
	out PORTB,AddressX
	sub AddressX, r30
	out PORTB,AddressX
	inc AddressX
	out PORTB, r31
	out PORTD, r31
	out PORTC, r31	
	out PORTA, r29			;toggle the clock 
	out PORTA, r31			;toggle the clock 
	cpi AddressX, 0x80
	breq upY
	rjmp outputAll

upY: 
	add AddressY, r28
	clr AddressX
	cpi AddressY, 0xFC
	out PORTC, r31	
	breq start
	rjmp outputAll
start: 
	jmp strat

	
H:	cpi Pixel, 0
	breq H1
	cpi Pixel, 1
	breq H2
	cpi Pixel, 2
	breq H3
	cpi Pixel, 3
	breq H4
	cpi Pixel, 4
	breq H5
	cpi Pixel, 5
	breq H6
	cpi Pixel, 6
	breq H7
	cpi Pixel, 7
	breq H8
	cpi Pixel, 8
	breq H9
	cpi Pixel, 9
	breq H10
	cpi Pixel, 10
	breq H11
	cpi Pixel, 11
	breq H12

H1:	mov AddressX, r26
	mov AddressY, r27
	inc Pixel
	rjmp outputPix

H2:	add AddressY, r16
	inc Pixel
	rjmp outputPix
H3:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
H4:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
H5: 
	add AddressY, r16
	inc Pixel
	rjmp outputPix

H6:	inc AddressX
	subi AddressY, 8
	inc Pixel
	rjmp outputPix

H7:	inc AddressX
	inc Pixel
	rjmp outputPix

H8:	inc AddressX
	inc Pixel
	rjmp outputPix
H9:	
	subi AddressY, 8
	inc Pixel
	rjmp outputPix
H10:
	add AddressY, r16
	inc Pixel
	rjmp outputPix
H11:
	add AddressY, r16
	inc Pixel
	rjmp outputPix
H12:
	add AddressY, r16
	ldi Pixel, 0
	inc lettre
	rjmp outputPix

E:	cpi Pixel, 0
	breq E1
	cpi Pixel, 1
	breq E2
	cpi Pixel, 2
	breq E3
	cpi Pixel, 3
	breq E4
	cpi Pixel, 4
	breq E5
	cpi Pixel, 5
	breq E6
	cpi Pixel, 6
	breq E7
	cpi Pixel, 7
	breq E8
	cpi Pixel, 8
	breq E9
	cpi Pixel, 9
	breq E10
	cpi Pixel, 10
	breq E11
	cpi Pixel, 11
	breq E12

E1:	add AddressX, r20
	mov AddressY, r27
	inc Pixel
	rjmp outputPix
E2:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
E3:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
E4:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
E5: 
	add AddressY, r16
	inc Pixel
	rjmp outputPix
E6:	inc AddressX
	subi AddressY, 8
	inc Pixel
	rjmp outputPix

E7:	subi AddressY, 8
	inc Pixel
	rjmp outputPix

E8:	add AddressY, r16
	add AddressY, r16
	add AddressY, r16
	add AddressY, r16
	inc Pixel
	rjmp outputPix

E9:	inc AddressX
	subi AddressY, 8
	inc Pixel
	rjmp outputPix

E10:add AddressY, r16
	add AddressY, r16
	inc Pixel
	rjmp outputPix

E11:subi AddressY, 8
	inc Pixel
	rjmp outputPix

E12:subi AddressY, 8
	
	ldi Pixel, 0
	inc lettre
	rjmp outputPix
	


L:	cpi Pixel, 0
	breq L1
	cpi Pixel, 1
	breq L2
	cpi Pixel, 2
	breq L3
	cpi Pixel, 3
	breq L4
	cpi Pixel, 4
	breq L5
	cpi Pixel, 5
	breq L6
	cpi Pixel, 6
	breq L7

L1:	add AddressX, r20
	mov AddressY, r27
	inc Pixel
	rjmp outputPix
L2:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
L3:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
L4:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
L5: 
	add AddressY, r16
	inc Pixel
	inc AddressX
	rjmp outputPix
L6:	
	inc Pixel
	inc AddressX
	rjmp outputPix
L7:	
	ldi Pixel, 0
	inc lettre
	
	rjmp outputPix

O:	cpi Pixel, 0
	breq O1
	cpi Pixel, 1
	breq O2
	cpi Pixel, 2
	breq O3
	cpi Pixel, 3
	breq O4
	cpi Pixel, 4
	breq O5
	cpi Pixel, 5
	breq O6
	cpi Pixel, 6
	breq O7
	cpi Pixel, 7
	breq O8
	cpi Pixel, 8
	breq O9
	cpi Pixel, 9
	breq O10

O1:	add AddressX, r20
	mov AddressY, r27
	add AddressY, r16
	inc Pixel
	rjmp outputPix
O2:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
O3:	
	add AddressY, r16
	inc Pixel
	rjmp outputPix
	
O4:	inc AddressX
	subi AddressY, 12
	inc Pixel
	rjmp outputPix
O5: 
	add AddressY, r16
	add AddressY, r16
	add AddressY, r16
	add AddressY, r16
	inc Pixel
	rjmp outputPix

O6:	inc AddressX
	subi AddressY, 16
	inc Pixel
	rjmp outputPix
O7:	
	add AddressY, r16
	add AddressY, r16
	add AddressY, r16
	add AddressY, r16
	inc Pixel
	rjmp outputPix
	
O8:	inc AddressX
	subi AddressY, 8
	inc Pixel
	rjmp outputPix
O9:	
	subi AddressY, 4
	inc Pixel
	rjmp outputPix
O10:
	add AddressY, r16
	add AddressY, r16
	ldi Pixel, 0
	inc lettre
	rjmp outputPix
