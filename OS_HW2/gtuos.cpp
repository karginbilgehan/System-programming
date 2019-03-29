#include <iostream>
#include <fstream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
//these are for System Calls functions
const uint8_t READ_B=7;
const uint8_t PRINT_B = 4;
const uint8_t PRINT_MEM=3;
const uint8_t READ_MEM=2;
const uint8_t PRINT_STR=1;
const uint8_t READ_STR=8;
const uint8_t LOAD_EXEC=5;
const uint8_t SET_QUANTUM=6;
const uint8_t PROCESS_EXIT=9;

const uint64_t CYCLE=10;
const uint64_t CYCLE_HUNDRED=100;
const uint64_t CYCLE_SEVEN=7;
const uint64_t CYCLE_EIGHTY=80;
uint64_t GTUOS::handleCall( CPU8080 & cpu){


    std::ofstream myfile;//it is to write file 
    std::ifstream myfile2;//it is to read file
    
    myfile.open("output.txt", std::ios_base::app);//it is to write file
    myfile2.open("input.txt");//it is to read file

    if( !myfile ) { // file couldn't be opened to write
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }
    if( !myfile2 ) { // file couldn't be opened to read
        std::cerr << "Error: file could not be opened" << std::endl;
        exit(1);
    }

    uint16_t address,hl_address;
    address= ((uint16_t) cpu.state->b << 8) | cpu.state->c;//the address of reg BC
    hl_address = (((uint16_t)cpu.state->h) << 8) | cpu.state->l;
    
    //PRINT_B system call	
    if(cpu.state->a == PRINT_B){
        myfile << (int)cpu.state->b<<" ";
        return CYCLE;
    }
    //PRINT_MEM system call
    else if(cpu.state->a==PRINT_MEM){
        myfile << "Value of Memory Address B and C:" << (int)cpu.memory->at(address)<<std::endl;
        return CYCLE;
    }
    //READ_B system call
    else if(cpu.state->a==READ_B){
        int number;
        myfile2 >> number;
        cpu.state->b=(uint8_t)number;
        //myfile << "Value of Register B: "<< (int)cpu.state->b << std::endl;
        return  CYCLE;
    }
    
    //READ_MEM system call
    else if(cpu.state->a == READ_MEM){
        uint8_t num;
        myfile2 >> num;
        if (num>=0 && num<=255){
            cpu.memory->at(address) = (uint8_t)num;
        }
        else{
            myfile<<"You entered wrong value.";
        }
        return CYCLE;
    }

    //PRINT_STR system call
    else if (cpu.state->a == PRINT_STR){
        uint64_t cycle=0;
        for(uint16_t i=address;i<=address+(uint16_t)65535;++i)//this value is equal to 2^16.
        {
            if(cpu.memory->at(i) != (uint8_t)0){
                myfile << (char)cpu.memory->at(i);
                cycle = cycle + (uint64_t)10;
	    }
	     else
                break;
        }
        return cycle;
    }
    
    //READ_STR system call
    else if (cpu.state->a==READ_STR){
        uint64_t cycle=0;
        std::string str = "";
        myfile2 >> str;

        uint16_t real_address;
        real_address=address;
        int lengthOfString=str.length();
        for (int j = 0; j < lengthOfString; j++)
        {
            cpu.memory->at(real_address) = (uint8_t)str[j];
            real_address++;
            cycle= cycle + (uint64_t) 10;
        }
        cpu.memory->at(real_address) =(uint8_t)'\0';
        //myfile<<"The string read from input.txt:";
        for(uint16_t i=address;i<address+lengthOfString;++i)//it is for print the string that read from input.txt
        {
            myfile<< (char) cpu.memory->at(i);
        }

	    myfile<<":";
        return cycle;


    }
   
    else if (cpu.state->a==LOAD_EXEC){
      	
	
	char file_name[1024] = { 0 };
	int i;
	    for (i = 0; (int)cpu.memory->at(address + i) != 0; ++i) 
		file_name[i] = (char) cpu.memory->at(address + i);
	    file_name[i] = '\0';
	    //printf("file name : %s, address hl : %x\n", file_name, address_hl);
	    cpu.ReadFileIntoMemoryAt(file_name, hl_address);    
	    return CYCLE_HUNDRED;


    }
    else if (cpu.state->a==SET_QUANTUM){
      	
	uint8_t quant=cpu.state->b;
	//printf("quant: %d \n",quant);
	cpu.setQuantum(quant);
	return CYCLE_SEVEN;
    }
    else if (cpu.state->a==PROCESS_EXIT){
	memset(&cpu.memory->at(12288),0,14*sizeof(uint8_t));
	(( Memory *)(cpu.memory))->setBaseRegister(0);
	if (counter==1){
		cpu.state->pc=0X7530;
	}	
	else if(counter==2){
		cpu.state->pc=0X9000;
	}
	counter++;			
	return CYCLE_EIGHTY;	
    }
    
    myfile.close();
    myfile2.close();
    std::cout <<  "Unimplemented OS call";
    throw -1;
    return 0;
}
