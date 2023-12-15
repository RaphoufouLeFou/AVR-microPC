
	.def DataL = r22
	.def DataH = r21
	.def AddressX = r17
	.def AddressY = r18
	.def IsW = r25
	.def IsOk = r26
	.def button = r24

	.equ BackColor = 0xFFFF	;BBBBBGGGGGGRRRRR  (5-6-5)

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

	out DDRC, r19			;set the output pin to output
	out DDRB, r19
	ldi r19, 0b00111111
	out DDRD, r19
	out DDRA, r29
	ldi	DataL, LOW(BackColor)
	ldi	DataH, HIGH(BackColor)
	rjmp outputAll


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
	breq loopR
	rjmp outputAll

loopR:
	jmp loopR
