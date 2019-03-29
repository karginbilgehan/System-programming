	; 8080 assembler code
        .hexfile MicroKernel.hex
        .binfile MicroKernel.com
        ; try "hex" for downloading in hex format
        .download bin  
        .objcopy gobjcopy
        .postbuild echo "OK!"
        ;.nodump


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

	org 0000H
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
	;EI
	ret
	; ---------------------------------------------------------------
	; YOU SHOULD NOT CHANGE ANYTHING ABOVE THIS LINE 
  

	org 0028H
	jmp interruptHandler

	
	org 05DCH

first_process: dw  'Collatz.com',00H		
second_process: dw  'sum.com',00H	
third_process: dw  'ShowPrimes.com',00H	
begin:	
	DI
	LXI SP,stack
	LXI B, 05DCH	; dosyanin isminin saklandigi bellek blogunun baslangic adresi		1010.adres
	LXI H, 1770H	; dosyanin nereden itibaren RAM'e yazilacaginin baslangic adresi	5000.adres
	MVI A, LOAD_EXEC
	call GTU_OS

	LXI B, 05E9H	; dosyanin isminin saklandigi bellek blogunun baslangic adresi		1010.adres
	LXI H, 1F40H	; dosyanin nereden itibaren RAM'e yazilacaginin baslangic adresi	5000.adres
	MVI A, LOAD_EXEC
	call GTU_OS
	
	LXI B, 05F2H	; dosyanin isminin saklandigi bellek blogunun baslangic adresi		1010.adres
	LXI H, 2710H	; dosyanin nereden itibaren RAM'e yazilacaginin baslangic adresi	5000.adres
	MVI A, LOAD_EXEC
	call GTU_OS
	
	
	
	;EI
	LXI H,0000H		; yeni prosesin kendi adres space'indeki ilk adresi
	LXI D,1770H		; yeni prosesin kendi adres space'i, 5000. adres
	PCHL
	
			
	
	hlt


interruptHandler:
    DI
    ;256dan başlayan değerleri process table a atmak için kullandığımız alan
    LXI B, 0100H
    LDAX B
    LXI B,3000H
    STAX B
    
    LXI B, 0101H
    LDAX B
    LXI B,3001H
    STAX B 
    
    LXI B, 0102H
    LDAX B
    LXI B,3002H
    STAX B    
    
    LXI B, 0103H
    LDAX B
    LXI B,3003H
    STAX B 

    LXI B, 0104H
    LDAX B
    LXI B,3004H
    STAX B 

    LXI B, 0105H
    LDAX B
    LXI B,3005H
    STAX B 

    LXI B, 0106H
    LDAX B
    LXI B,3006H
    STAX B 
    
    LXI B, 0107H
    LDAX B
    LXI B,3007H
    STAX B 
    
    LXI B, 0108H
    LDAX B
    LXI B,3008H
    STAX B 

    LXI B, 0109H
    LDAX B
    LXI B,3009H
    STAX B 

    LXI B, 010AH
    LDAX B
    LXI B,300AH
    STAX B 

    LXI B, 010BH
    LDAX B
    LXI B,300BH
    STAX B 

    LXI B, 010CH
    LDAX B
    LXI B,300CH
    STAX B 

    LXI B, 010DH
    LDAX B
    LXI B,300DH
    STAX B
	 
    ;process tabledan gerekli değerleri çekmemiz gereken alan 
 
  
    

    

    

   ;stack pointer ı atmak için kullandık.Hlden once stack pointerı atmak gerekiyor. Çünkü stack pointer için hl yi kullanıyoruz.
    LXI B,3007H
    LDAX B
    MOV L,A ; stack pointer low
    LXI B,3008H
    LDAX B
    MOV H,A ; stack pointer high
    SPHL
    
    LXI B,3003H
    LDAX B
    MOV D,A

    LXI B,3004H
    LDAX B
    MOV E,A
    PUSH D

    
    LXI B,3005H
    LDAX B
    MOV H,A

    LXI B,3006H
    LDAX B
    MOV L,A
    PUSH H
   
    
      
   
    ;PC ve base registırları atma işlemini gerçekleştirdik.
    LXI B,300BH
    LDAX B
    MOV E,A  ; base register low
    LXI B,300CH
    LDAX B
    MOV D,A ; base register high
    LXI B,3009H
    LDAX B
    MOV L,A ; program counter low
    LXI B,300AH
    LDAX B
    MOV H,A  ; program counter high
   
   
    
   

    LXI B,3002H
    LDAX B
    PUSH psw	; NEW
    LXI B,3001H
    LDAX B
    MOV B,A
    POP psw	; NEW
    MOV C,A	; NEW

    PUSH B
    
 
;cc degerini ve a registırını push eder. Ama a nın içinde yanlış deger olacagı için a yı tekrar almamız gerekecek
    LXI B,300DH
    LDAX B
    PUSH PSW
    POP PSW
    
       ;a registerı surekli kullandigimiz icin bu blogu en sonda yapmamiz gerekir cunku a surekli degisiyor
    LXI B,3000H
    LDAX B
    MOV A,A
    
    POP B
    EI
    PCHL
    hlt


jumpSum:
	org 7530H
	LXI H,0000H		; yeni prosesin kendi adres space'indeki ilk adresi
	LXI D,1F40H		; yeni prosesin kendi adres space'i, 5000. adres
	PCHL
jumpShowPrimes:
	
	org 9000H
	LXI H,0000H		; yeni prosesin kendi adres space'indeki ilk adresi
	LXI D,2710H		; yeni prosesin kendi adres space'i, 5000. adres
	PCHL
