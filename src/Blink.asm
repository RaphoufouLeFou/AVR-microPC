		.include "m328pdef.inc"

		.def LedPin = r16
		.def State = r17
		.def oLoopR = r18
		.def iLoopR = r24
		.def mLoopR = r25

		.equ oVal 	= 142
		.equ mVal	= 251
		.equ iVal	= 112

		clr State
		ldi LedPin,(1<<PINB5)
		out DDRB,LedPin
	
start: 	eor State,LedPin
		out PORTB,State

		ldi mLoopR,mVal

mLoop:
		ldi oLoopR,oVal

oLoop:
		ldi iLoopR,iVal

iLoop:
		dec iLoopR
		brne iLoop
		dec oLoopR
		brne oLoop
		dec mLoopR
		brne mLoop
		rjmp start
	
