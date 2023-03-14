	.include "m328pdef.inc"
	.def oLoopR = r18
	.def iLoopR = r24
	.def mLoopR = r25

	.equ oVal 	= 142
	.equ mVal	= 251
	.equ iVal	= 224

	.equ F_CPU = 16000000
	.equ baud	= 9600					; baudrate
	.equ bps	= (F_CPU/16/baud) - 1	; baud prescale
	ldi	r19,'a'							; load char 'a' into r19

start:
	ldi	r16,LOW(bps)					; load baud prescale
	ldi	r17,HIGH(bps)					; into r17:r16
	rcall	initUART					; call initUART subroutine
	ldi	ZL,LOW(2*Str)					; load Z pointer with
	ldi	ZH,HIGH(2*Str)					; myStr address
	rcall	puts						; transmit string

	ldi mLoopR,mVal
	jmp delay1s
	
initUART:
	sts	UBRR0L,r16						; load baud prescale
	sts	UBRR0H,r17						; to UBRR0

	ldi	r16,(1<<RXEN0)|(1<<TXEN0)		; enable transmitter
	sts	UCSR0B,r16						; and receiver

	ret									; return from subroutine


putc:	
	lds	r17,UCSR0A						; load UCSR0A into r17
	sbrs	r17,UDRE0					; wait for empty transmit buffer
	rjmp	putc						; repeat loop

	sts	UDR0,r19						; transmit character

	ret									; return from subroutine

puts:	lpm	r16,Z+						; load character from pmem
	cpi	r16,$00							; check if null
	breq	puts_end					; branch if null

puts_wait:
	lds	r17,UCSR0A						; load UCSR0A into r17
	sbrs	r17,UDRE0					; wait for empty transmit buffer
	rjmp	puts_wait					; repeat loop

	sts	UDR0,r16						; transmit character
	rjmp	puts						; repeat loop

puts_end:
	ret									; return from subroutine

delay1s:								;delay 1 second
		ldi oLoopR,oVal

oLoop:
		ldi iLoopR,iVal

iLoop:
		dec iLoopR
		brne iLoop
		dec oLoopR
		brne oLoop
		dec mLoopR
		brne delay1s
		rjmp start
	
Str:	.db	"Hello world !",$00