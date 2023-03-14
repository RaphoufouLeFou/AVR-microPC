	.include "m16adef.inc"

	.def DataL = r22
	.def DataH = r21
	.def AddressX = r17
	.def AddressY = r18
	.def IsW = r25
	.def IsOk = r26
	.def IsPressed = r27
	.def button = r24
	.def pixX = r23
	.def pixY = r22

	.equ BackColor = 0xffff	;BBBBBGGGGGGRRRRR  (5-6-5)

	clr DataL				;reset registers to 0x00
	clr DataH
	clr AddressX
	clr AddressY
	ldi pixX, 0x40
	ldi pixY, 0x80
	ldi r19, 0xff			;load some useful registers
	ldi r29, 0x02
	ldi r26, 0x17
	ldi r20, 0x03
	ldi r16, 0x04
	ldi r30, 0b10000000
	ldi r28, 0b00000100

	out DDRC, r19			;set the output pin to output
	out DDRB, r19
	out DDRD, r19
	out DDRA, r29
	ldi	DataL, LOW(BackColor)
	ldi	DataH, HIGH(BackColor)
	st Z, r26
	ldi r26, 0x00
	rjmp outputAll

loop:

	in button, PINA
	andi button, 0b00000100
	cpi button, 0b00000100
	breq IsPress1
	in button, PINA
	andi button, 0b00001000
	cpi button, 0b00001000
	breq IsPress2
	in button, PINA
	andi button, 0b00010000
	cpi button, 0b00010000
	breq IsPress3
	in button, PINA
	andi button, 0b00100000
	cpi button, 0b00100000
	breq IsPress4
	ldi IsPressed, 0x00
	rjmp loop

IsPress1:
	cpi IsPressed, 0x00
	breq action1
	rjmp loop

IsPress2:
	cpi IsPressed, 0x00
	breq action2
	rjmp loop

IsPress3:
	cpi IsPressed, 0x00
	breq action3
	rjmp loop

IsPress4:
	cpi IsPressed, 0x00
	breq action4
	rjmp loop

action1:
	ldi IsPressed, 0x01
	inc pixX
	mov AddressX, pixX
	ldi AddressY, 0x85
	ldi	DataL, 0x00
	ldi	DataH, 0x00
	rjmp outputPix

action2:
	ldi IsPressed, 0x01
	dec pixX
	mov AddressX, pixX
	ldi AddressY, 0x85
	ldi	DataL, 0x00
	ldi	DataH, 0x00
	rjmp outputPix

action3:
	ldi IsPressed, 0x01
	add pixY, r28
	mov AddressY, pixY
	ldi	DataL, 0x00
	ldi	DataH, 0x00
	rjmp outputPix

action4:
	ldi IsPressed, 0x01
	sub pixY, r28
	mov AddressY, pixY
	ldi	DataL, 0x00
	ldi	DataH, 0x00
	rjmp outputPix



outputAll:
	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, r29			;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, r26			;toggle the clock 

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
	out PORTB, r26
	out PORTD, r26
	out PORTC, r26	
	out PORTA, r29			;toggle the clock 
	out PORTA, r26			;toggle the clock 
	cpi AddressX, 0x80
	breq upY
	rjmp outputAll

upY: 
	add AddressY, r28
	clr AddressX
	cpi AddressY, 0xFC
	out PORTC, r26	
	breq loopR
	rjmp outputAll

loopR:
	ijmp

outputPix:

	in IsW, PINA			;read PORTA
	andi IsW, 0b00000001	;apply a mask to get the first bit
	subi IsW, 0x01			;substact 1
	brne outputPix			;jump to output if it's not nul, so when input is 0
	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, r29			;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, r26			;toggle the clock 
	out PORTD, AddressY
	add AddressX, r30		
	out PORTB,AddressX
	sub AddressX, r30
	out PORTB, r26
	out PORTD, r26
	out PORTC, r26	
	out PORTA, r29			;toggle the clock 
	out PORTA, r26			;toggle the clock 
	rjmp loop
