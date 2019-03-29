        ; 8080 assembler code
        .hexfile ShowPrimes.hex
        .binfile ShowPrimes.com
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
	
string: dw '',00AH,00H ;
prime: dw ':prime',00AH,00H ;	
start: dw '------Show Primes is called-----',00AH,00H ;
begin:
	
	LXI SP,stack	 ; always initialize the stack pointer
	
	LXI B,start	
	MVI A,PRINT_STR
	call GTU_OS
	
	MVI A,255
	MVI E,254	
	MOV D,A 		;to protect start value etc 12
	MVI C,0

control:
	MOV A,E 
	CPI 2 			; to realize the value of factor
	JZ findPrime
	MOV A,D 
	CPI 2 			; to realize the end of the program
	JZ exit			; If the register D is equal to 2 that means the program must halt.

loop: 
	SUB E			;A=A-E
	JZ findNotPrime		;That means find a factor. So this number is not prime 
	JNC loop		
	JMP dec			;this is to control this situation--> A<0
	
findNotPrime:
	MVI C,1			; C=1
	DCR E			; E--
	JMP control		

dec:
	MOV A,D			;A=D
	DCR E			;E--
	JMP control

findPrime:	
	MOV A,C			;A=C
	CPI 1			;A?1, to realize that the number prime or not
	JNZ controlForPrime
	MVI C,0    		;C=0
	MOV B,D
	MVI A,PRINT_B
	call GTU_OS
	LXI B, string
	MVI A, PRINT_STR
	call GTU_OS   	
	DCR D			;D--
	MOV H,D			;H=D
	DCR H			;H--
	MOV E,H			;E=H
	JMP Control

;to realize the value of Prime Number
;If the number is 4,there will not print 
controlForPrime:
	MOV B,D    		;B=D
	MOV A,B			;A=B
	CPI 4			;A?4
	JZ notPrint	
	JNZ printPrime

printPrime:
	MVI A, PRINT_B		
	call GTU_OS
	LXI B, prime
	MVI A, PRINT_STR
	call GTU_OS
	MVI C,0			;C=0
	DCR D			;D--
	MOV H,D			;H=D
	DCR H			;H--
	MOV E,H			;E=H
	JMP Control

notPrint:		
	MVI C,0			;C=0
	DCR D			;D--
	MOV H,D			;H=D
	DCR H			;H--
	MOV E,H 		;E=H
	JMP Control
exit:
	MVI B,2                 ; To print number 2 as a prime number
	MVI A, PRINT_B
	call GTU_OS
	LXI B, prime
	MVI A, PRINT_STR
	call GTU_OS
	hlt
