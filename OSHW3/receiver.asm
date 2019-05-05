				; 8080 assembler code
        .hexfile receiver.hex
        .binfile receiver.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump

	; OS call list
PRINT_B		equ 4
PRINT_MEM	equ 3
READ_B		equ 7
READ_MEM	equ 2
PRINT_STR	equ 1
READ_STR	equ 8
LOAD_EXEC 	equ 5
PROCESS_EXIT	equ 9
SET_QUANTUM 	equ 6
RAND_INT 	equ 12	
ALL_INFORMATION equ 11
WAIT		equ 13
SIGNAL		equ 14	
	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:
	DI
	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	EI
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        

	;This program adds numbers from 0 to 10. The result is stored at variable
	; sum. The results is also printed on the screen.
nl:	dw 'RECEIVER',00H	

begin:
	LXI SP,stack 	; always initialize the stack pointer
	;DI
	MVI D,60 ; total number of loop 
	MVI E,0 ; counter
	JMP main



main:
	;LXI B,nl
	;MVI A,PRINT_STR
	;call GTU_OS
	MOV A,E
	CMP D		;loop control for 400
	JZ exit
	
	; WAIT KISMI
	MVI C,2d	;full semaphore
	MVI A,WAIT	
	call GTU_OS
	MVI C,0d	;mutex
	MVI A,WAIT
	call GTU_OS
	
	;CRITICAL SECTION
	;LXI B, 1491D
	;LDAX B
	MVI A,PRINT_MEM		;memory adresinden degeri okumak icin
	call GTU_OS
	;MVI A,11d	;memory icerigini bastirmak icin
	;scall GTU_OS
	;MOV B,A
	;MVI A,PRINT_B
	;call GTU_OS
	

	;SIGNAL KISMI
	MVI C,0d	;mutex
	MVI A,SIGNAL
	call GTU_OS
	MVI C,1d	;empty semaphore
	MVI A,SIGNAL
	call GTU_OS
	
	INR E 		; counter
	JMP main	; while counter < 50

exit:
	MVI A,PROCESS_EXIT
	call GTU_OS
	
