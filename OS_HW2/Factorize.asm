        ; 8080 assembler code
        .hexfile Factorize.hex
        .binfile Factorize.com
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

	; Position for stack pointer
stack   equ 0F000h

	org 000H
	jmp begin

	; Start of our Operating System
GTU_OS:	DI
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

begin:
	LXI SP,stack 	; always initialize the stack pointer
    	MVI A, READ_B
	call GTU_OS
	MOV A,B         ;A=B
	MOV E,A		;E=A
	MOV D,A 	;to protect start value etc 12

;it control that the program finish or not	
control:
	MOV A,E      ;A=E
	CPI 0	     ;A?0
	JZ exit
	MOV A,D	     ;A=D

;it is for substract
loop: 	
	SUB E        ;A=A-E
	JZ print     ;A?0
	JNC loop     ;if A not smaller than zero go to loop
	JMP dec	     ;if A smaller than zero go to dec

;it print the result on screen 	
print:
	MOV B,E		;B=E
	MVI A, PRINT_B 	;store the OS call code to A
	call GTU_OS	;call the OS
	DCR E		;E=E-1
	JMP control

;it decreases the factor
dec:
	MOV A,D 	;A=D
	DCR E		;E=E-1
	JMP control

exit:	
	hlt
