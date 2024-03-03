#include <avr/io.h>


;***** AVR_Init *****
;   input:
;
;   Using :
;   r25

.global AVR_Init
AVR_Init:
    

    ldi     r25,   0xFF					;set tmp to 0xFF
    out     _SFR_IO_ADDR(DDRB),   r25	;set the output pin to output
	out     _SFR_IO_ADDR(DDRC),   r25	;
	ldi     r25,   0xFC					;
	out     _SFR_IO_ADDR(DDRD),   r25	;
    ldi     r25,   0b10					;
    out     _SFR_IO_ADDR(DDRA),   r25	;
	ret

;***** end of AVR_Init *****


;***** AVR_Output_Pixel *****
;   input:
;   r25, DataL
;   r24, DataH
;   r22, AddressX
;   r20, AddressY
;   
;   Using :
;   r23


.global AVR_Output_Pixel
AVR_Output_Pixel:                      	;output the pixel to the screen
    #define    tmp    r23

	in      tmp,    _SFR_IO_ADDR(PINA)	;read PORTA
	andi    tmp,    0b00000001			;apply a mask to get the first bit
	subi    tmp,    0x01				;substact 1
	brne    AVR_Output_Pixel			;jump to output if it's not nul, so when input is 0
    ldi     tmp,    0x02
	
	out     _SFR_IO_ADDR(PORTC),  r25	;output the first 8bit data to PORTC	
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 
    clr     tmp
	
	out     _SFR_IO_ADDR(PORTC),  r24	;output the last 8bit data to PORTC	
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 

	lsl    r20                 			;shift the X address to the left
	lsl    r20                 			;shift the X address to the left
    out     _SFR_IO_ADDR(PORTD),  r20   ;output the Y address to PORTD
    
	ori    r22,   0x80					;add the write enable constant to the X address
	out     _SFR_IO_ADDR(PORTB),  r22   ;output the X address to PORTB

	out     _SFR_IO_ADDR(PORTB),  tmp   ;Clear the ports
	out     _SFR_IO_ADDR(PORTD),  tmp   ;
	out     _SFR_IO_ADDR(PORTC),  tmp   ;
    ldi     tmp,    0x02
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 
    clr     tmp
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 
    ret


;***** end of AVR_Output_Pixel *****



;***** AVR_Output_All *****
;   input:
;   r25, DataL
;   r24, DataH
;
;   Using :
;   r23

.global AVR_Output_All
AVR_Output_All:

    #define    tmp    r23

    ldi     tmp,    0x02
	out     _SFR_IO_ADDR(PORTC),  r25	;output the first 8bit data to PORTC	
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 
    clr     tmp
	out     _SFR_IO_ADDR(PORTC),  r24 	;output the last 8bit data to PORTC	
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 

    #define   AdX r25
    #define   AdY r24

    clr    AdX
    clr    AdY

outputAllLoop:
	in      tmp,    _SFR_IO_ADDR(PINA)	;read PORTA
	andi    tmp,    0b00000001			;apply a mask to get the first bit
	subi    tmp,    0x01				;substact 1
	brne    outputAllLoop				;jump to output if it's not nul, so when input is 0
	out     _SFR_IO_ADDR(PORTD),  AdY   ;output the Y address to PORTD
	subi    AdX,    -0b10000000 		;add the write enable constant to the X address
	out     _SFR_IO_ADDR(PORTB),  AdX   ;output the X address to PORTB
	subi    AdX,    0b10000000  		;substract the write enable constant from the X address
	out     _SFR_IO_ADDR(PORTB),  AdX   ;output the X address to PORTB
	inc     AdX                 		;increase the X address
	cpi     AdX,    0x80        		;compare the X address to 0x80
	brne    outputAllLoop       		;if it's not 0x80, jump to the outputAllLoop loop
    
    clr     AdX                 		;reset the X address to 0
    clr     tmp
	out     _SFR_IO_ADDR(PORTC), tmp	;Clear the ports
    cpi     AdY, 0xFC           		;compare the Y address to 0xFC
	breq    outputAllEnd        		;if it's 0xFC, jump to the outputAllEnd point
    subi    AdY, -0b00000100    		;add 4 from the Y address
	rjmp    outputAllLoop       		;jump to the outputAllLoop loop
outputAllEnd: 
    ret

;***** end of AVR_Output_All *****

;***** AVR_Draw_Fill_Rect *****
;   input:
;   r24, DataL
;   r25, DataH
;   r22, Point1X
;   r20, Point1Y
;   r18, SizeX
;   r16, SizeY
;
;   Using :

.global AVR_Draw_Fill_Rect

AVR_Draw_Fill_Rect:

	#define    tmp    r23

    ldi     tmp,    0x02
	out     _SFR_IO_ADDR(PORTC),  r25	;output the first 8bit data to PORTC	
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 
    clr     tmp
	out     _SFR_IO_ADDR(PORTC),  r24 	;output the last 8bit data to PORTC	
	out     _SFR_IO_ADDR(PORTA),  tmp	;toggle the clock 

    #define   AdX r25
    #define   AdY r24

	#define   MaxX r21
	#define   MaxY r19

    mov    	AdX, r22
    mov    	AdY, r20
	mov   	MaxX, AdX
	add   	MaxX, r18
	lsl   	AdY
	lsl   	AdY
	lsl   	r16 
	lsl 	r16                 			
	mov   	MaxY, AdY
	add   	MaxY, r16


drawFillRectLoop:
	in      tmp,    _SFR_IO_ADDR(PINA)	;read PORTA
	andi    tmp,    0b00000001			;apply a mask to get the first bit
	subi    tmp,    0x01				;substact 1
	brne    drawFillRectLoop			;jump to output if it's not nul, so when input is 0
	out     _SFR_IO_ADDR(PORTD),  AdY   ;output the Y address to PORTD
	subi    AdX,    -0b10000000 		;add the write enable constant to the X address
	out     _SFR_IO_ADDR(PORTB),  AdX   ;output the X address to PORTB
	subi    AdX,    0b10000000  		;substract the write enable constant from the X address
	out     _SFR_IO_ADDR(PORTB),  AdX   ;output the X address to PORTB
	inc     AdX                 		;increase the X address
	cp     	AdX,	MaxX        		;compare the X address to MaxX
	brne    drawFillRectLoop       		;if it's not 0x80, jump to the drawFillRectLoop loop
    
    clr     AdX                 		;reset the X address to 0
    clr     tmp
	out     _SFR_IO_ADDR(PORTC), tmp	;Clear the ports
    cp     	AdY, MaxY           		;compare the Y address to MaxY
	brge    drawFillRectEnd        		;if it's 0xFC, jump to the drawFillRectEnd point
    subi    AdY, -0b00000100    		;add 4 from the Y address
	rjmp    drawFillRectLoop       		;jump to the drawFillRectLoop loop
drawFillRectEnd: 
    ret

;***** end of AVR_Draw_Fill_Rect *****


;***** AVR_Read_Inputs *****
;   input:
;
;   Using :
;   r24

.global AVR_Read_Inputs

AVR_Read_Inputs:
	#define    tmp    r24
	in      tmp,    _SFR_IO_ADDR(PINA)	;read PORTA
	andi    tmp,    0b11111100			;apply a mask to get the first bit
	ret


;***** end of AVR_Read_Inputs *****


;***** AVR_Draw_Line ***** (NOT WORKING !!!)
;   input:
;   r25, DataL
;   r24, DataH
;   r22, Point1X
;   r20, Point1Y
;   r18, Point2X
;   r16, Point2Y
;
;   Using :
;   a lot of them (r13-r25)

.global AVR_Draw_Line
AVR_Draw_Line:

	#define dx r23
	#define minusdy r13
	#define dy r21
	#define sx r19
	#define sy r17
	#define err r15
	#define e2 r14

	#define x1 r22
	#define y1 r20
	#define x2 r18
	#define y2 r16

	; calculate the differences

	mov dx, x2
	sub dx, x1
	mov dy, y2
	sub dy, y1

	; calculate the direction

	cpi dx, 0x80
	brlt dxPositive
	neg dx
	ldi sx, -1
	rjmp dxDone
dxPositive:
	ldi sx, 1
dxDone:
	cpi dy, 0x80
	brlt dyPositive
	neg dy
	ldi sy, -1
	rjmp dyDone
dyPositive:
	ldi sy, 1
dyDone:
	
	; calculate the error

	mov err, dx
	sub err, dy

	mov minusdy, dy
	neg minusdy

lineLoop:

	; check if we are done

	cp x1, x2
	breq lineDone

notDone:
	; output the pixel

	call AVR_Output_Pixel

	; calculate the error

	mov e2, err
	lsl e2

	; check if we need to move in the x direction

	cp e2, minusdy
	brlt moveX

DoneX:
	; check if we need to move in the y direction

	cp dx, e2
	brlt moveY

	rjmp lineLoop

moveX:

	; move in the x direction

	add x1, sx
	sub err, dy
	rjmp DoneX

moveY:

	; move in the y direction

	add y1, sy
	add err, dx
	rjmp lineLoop

lineDone:

	cp y1, y2
	brne notDone

	ret


;***** end of AVR_Draw_Line *****


;***** initUART ***** (NOT WORKING !!!)
;   input:
;	r24, baud prescale low
;	r25, baud prescale high
;
;   Using :
;   r25

.global initUART
initUART:
	ldi r24, 0x67				; mask the lower 4 bits
	sts	_SFR_IO_ADDR(UBRRL),r24	; load baud prescale

	ldi r25, 0
	sts	_SFR_IO_ADDR(UBRRH),r25	; to UBRR0

	ldi	r24, 0b00011000			; enable transmitter
	sts	_SFR_IO_ADDR(UCSRB),r24	; and receiver

	;ldi	r24, 0b00000110						; enable transmitter
	;sts	_SFR_IO_ADDR(_SFR_MEM8 (0xC2)),r24	; and receiver

	ret										; return from subroutine

;***** end of initUART *****


;***** sendUART *****	(NOT WORKING !!!)
;   input:
;	r24, data
;
;   Using :
;   r25

.global sendUART
sendUART:	

	lds	r25, _SFR_IO_ADDR(UCSRA)	; load UCSR0A into r17
	;sbrs r25, 5						; wait for empty transmit buffer

	;rjmp sendUART					; repeat loop

	sts	_SFR_IO_ADDR(UDR), r24		; transmit character
	ret								; return from subroutine

;***** end of sendUART *****
