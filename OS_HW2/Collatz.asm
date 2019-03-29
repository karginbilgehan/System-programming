        ; 8080 assembler code
        .hexfile Collatz.hex
        .binfile Collatz.com
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
LOAD_EXEC	equ 5
SET_QUANTUM     equ 6
PROCESS_EXIT    equ 9

	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS: DI	
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
	;EI
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        

	;This program adds numbers from 0 to 10. The result is stored at variable
	; sum. The results is also printed on the screen.

even: dw ': is the even number',00AH,00H ;
odd: dw ': is the odd number',00AH,00H ;

begin:
	LXI SP, stack
	MVI A, READ_B
	call GTU_OS
	MOV A,B
	MVI D,2
	MOV E,B ;to store the actual value
	MOV H,B ;to store the actual value
	MVI C,1 ;counter	

subtract:
	
	SUB D
	JZ divide
	JNC subtract
	JMP oddNumber

evenNumber:
	
	MOV B,C
	MVI A,PRINT_B
	call GTU_OS
	;MOV B,E
	;MVI A,PRINT_B
	;call GTU_OS
	;LXI B,even
	;MVI A,PRINT_STR
	;call GTU_OS
	MVI A,PROCESS_EXIT
	call GTU_OS

oddNumber:
	
	MOV A,E
        ADD E
	ADD E
	INR A
	MOV B,A
	MVI A,PRINT_B
	call GTU_OS
	JMP 0637H

divide:
       
       MOV A,E
       SUB D
       JZ evenNumber
       MOV E,A
       INR C
       JMP divide
       

	
