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
	ldi	XL,LOW(DataX)		; initialize X pointer
	ldi	XH,HIGH(DataX)		; to var address
	out PORTD, AddressY


loop:
	inc AddressX			;inrement the address register
	;st X+, DataL			;store the data value in the cpu ram
	;st X+, DataH
	cpi AddressX, 0x80		;check if the X adress is at his max
	breq max_X				;jump to max_X if it's the case 
	rjmp loop				;else, restart in the loop

max_X:
	clr AddressX			;clear the address X register
	add AddressY, r28		;increment the address Y register skipping the first two bits, it's TX and RX 
	out PORTD, AddressY		;output the address Y register
	ldi	XL,LOW(DataX)		; initialize X pointer
	ldi	XH,HIGH(DataX)		; to var address



output:
	in IsW, PINA			;read PORTA
	andi IsW, 0b00000001	;apply a mask to get the first bit
	subi IsW, 0x01			;substact 1
	brne loop				;jump to output if it's not nul, so when input is 0
	;ld DataL, X+			;load data from the cpu ram
	;ld DataH, X+
	out PORTC,DataL			;output the first 8bit data to PORTC	
	out PORTA, r29			;toggle the clock 
	out PORTC,DataH			;output the last 8bit data to PORTC	
	out PORTA, r31			;toggle the clock 
	add AddressX, r30		
	out PORTB,AddressX
	sub AddressX, r30
	out PORTB,AddressX
	inc AddressX
	cpi AddressX, 0x80
	breq end
	rjmp output

end:
	ldi AddressX, 0x00
	ldi	XL,LOW(DataX)		; initialize X pointer
	ldi	XH,HIGH(DataX)		; to var address
	add IsJSPL, r20
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	
	cpi IsJSPL, 0x02
	breq add1
	rjmp end
add1:
	inc ColorGrad
	
	cpi Color, 0
	breq red
	cpi Color, 1
	breq green
	cpi Color, 2
	breq blue
	rjmp loop

red:
	cpi ColorGrad, 0x20
	breq jsp
	mov DataL, ColorGrad
	ldi DataH, 0x00
	rjmp loop

green:
	cpi ColorGrad, 0x20
	breq jsp
	ldi DataL, 0x00
	ldi DataH, 0x00

	mov DataL, ColorGrad
	add DataL, DataL
	adc DataH, DataH
	add DataL, DataL
	adc DataH, DataH
	add DataL, DataL
	adc DataH, DataH
	add DataL, DataL
	adc DataH, DataH
	add DataL, DataL
	adc DataH, DataH
	add DataL, DataL
	adc DataH, DataH
	rjmp loop

blue:
	cpi ColorGrad, 0x20
	breq jsp
	ldi DataL, 0x00
	ldi DataH, 0x00
	mov DataH, ColorGrad
	add DataH, DataH
	add DataH, DataH
	add DataH, DataH
	rjmp loop

jsp:
	ldi r19, 0x00
	cpi Color, 2
	breq ClrColor
	inc Color
	rjmp loop

ClrColor:
	clr Color
	rjmp loop

		.dseg
		.org	SRAM_START
DataX:	.byte	0x100
