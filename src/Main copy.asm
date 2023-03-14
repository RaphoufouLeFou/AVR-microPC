	.include "m16adef.inc"

	.def DataL = r17
	.def DataH = r21
	.def AddressX = r16
	.def AddressY = r18
	.def IsW = r25
	.def IsJSPL = r22
	.def Color = r23
	.def ColorGrad = r19

	clr Color
	clr DataL				;reset registers to 0x00
	clr DataH
	clr AddressX
	clr AddressY
	ldi r19, 0xff			;load some useful registers
	ldi r29, 0x02
	ldi r31, 0x00
	ldi r20, 0x01
	ldi r30, 0b10000000
	ldi r28, 0b00000100
	out DDRC, r19			;set the output pin to output
	out DDRB, r19
	out DDRD, r19
	out DDRA, r29
	out PORTD, AddressY


loop:

	ldi	DataL, 0xff
	ldi	DataH, 0xff

	;st X+, DataL			;store the data value in the cpu ram
	;st Y+, DataH
	inc AddressX			;inrement the address register
	cpi AddressX, 0x80		;check if the X adress is at his max
	breq max_X				;jump to max_X if it's the case 
	

output:
	in IsW, PINA			;read PORTA
	andi IsW, 0b00000001	;apply a mask to get the first bit
	subi IsW, 0x01			;substact 1
	brne loop				;jump to output if it's not nul, so when input is 0

	;ld DataL, X+			;load data from the cpu ram
	;ld DataH, Y+
	cpi AddressX, 0x2a
	breq IsaddY
else:
	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, r29			;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, r31			;toggle the clock 
finish:	
	add AddressX, r30		
	out PORTB,AddressX
	sub AddressX, r30
	out PORTB,AddressX
	rjmp loop


max_X:

	clr AddressX			;clear the address X register
	add AddressY, r28		;increment the address Y register skipping the first two bits, it's TX and RX 
	out PORTD, AddressY		;output the address Y register
	;ldi	XL,LOW(DataXL)		; initialize X pointer
	;ldi	XH,HIGH(DataXL)		; to var address
	;ldi	YL,LOW(DataXH)		; initialize X pointer
	;ldi	YH,HIGH(DataXH)		; to var address
	cpi AddressY, 0xfc
	breq Max_Y
	rjmp output				;else, restart in the loop
	

end:
	ldi AddressX, 0x00
	;ldi	XL,LOW(DataXL)		; initialize X pointer
	;ldi	XH,HIGH(DataXL)		; to var address
	;ldi	YL,LOW(DataXH)		; initialize X pointer
	;ldi	YH,HIGH(DataXH)		; to var address
	rjmp loop

IsaddY:
	cpi AddressY, 0x1c
	breq Cond
	cpi AddressY, 0x1a
	breq Cond
	rjmp else 
Cond:
	ldi	DataL, 0x1f
	ldi	DataH, 0x00
	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, r29			;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, r31			;toggle the clock 
	rjmp finish 

Max_Y:
	clr AddressX
	rjmp output



		.dseg
		.org	SRAM_START
DataXL:	.byte	0x80
DataXH:	.byte	0x80





