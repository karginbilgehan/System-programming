        ; 8080 assembler code
        .hexfile Palindrome.hex
        .binfile Palindrome.com
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
GTU_OS:	PUSH D
	push D
	push H
	push psw
	nop	; This is where we run our OS in C++, see the CPU8080::isSystemCall()
		; function for the detail.
	pop psw
	pop h
	pop d
	pop D
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE        

	;This program prints a null terminated string to the screen


notPalindrome: dw 'Not Palindrome',00AH,00H ;
palindrome: dw 'Palindrome',00AH,00H ;
string: dw '',00AH,00H ;

begin:
	LXI SP,stack	 ; always initialize the stack pointer
	LXI B, string	
	MVI A, READ_STR  ; store the OS call code to A
	call GTU_OS	 ; call the OS

;This label fill the stack with giving string 
fillTheStack:
	LDAX B		;A=(BC)		
	CPI 0 		;to realize end of the string
	JZ loadString
	MOV D,A
	PUSH D		;push the element in stack
	INX B
	JMP fillTheStack

;This label load the giving string in register BC
loadString:
	LXI B,string

;This label pop the element in stack and compare the giving string by one by
popTheStack:
	LDAX B   ;A=(BC)
	CPI 0    ;to realize end of the string
	JZ isPal
	POP D
	CMP D 	; A?D
	JNZ notPal ;it control that letters are not same.
	INX B      ;BC=BC+1
	JMP popTheStack

notPal:
	LXI B, notPalindrome	
	MVI A, PRINT_STR  ; store the OS call code to A
	call GTU_OS
	hlt	

isPal:	
	LXI B, palindrome	
	MVI A, PRINT_STR  ; store the OS call code to A
	call GTU_OS
	hlt
