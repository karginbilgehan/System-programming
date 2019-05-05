#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"
#include <string.h>
#include <fstream> 
#include <unistd.h>
using namespace std;

#define CYCLE 10

GTUOS::GTUOS(){
  //string inputFile = "input.txt";
  //string outputFile = "output.txt";  

  /*open files*/
  //cin.open(inputFile);
  //cout.open(outputFile);  
}

GTUOS::~GTUOS(){
  /*close files*/
  //cin.close();
  //cout.close();  
}

/*
  Handle the operating system calls
*/
uint64_t GTUOS::handleCall(CPU8080& cpu){
  //cout << "system" << endl;
  uint16_t cycles;

  int c = 0;
  if(1 || (cin && cout)){
    switch(cpu.state->a){
    case PRINT_B:
      cycles = sysPrintB(cpu);
      break;
    case PRINT_MEM:
      cycles = sysPrintMem(cpu);    
      break;
    case READ_B:
      cycles = sysReadB(cpu);
      break;
    case READ_MEM:
      cycles = sysReadMem(cpu);
      break;
    case PRINT_STR:
      cycles = sysPrintStr(cpu);
      break;
    case READ_STR:
      cycles = sysReadStr(cpu);
      break;
    case LOAD_EXEC:
      cycles = loadExec(cpu);
      break;
    case PROCESS_EXIT:
      cycles = processExit(cpu);
      break;
    case SET_QUANTUM:
	break;
    case PRINT_WHOLE:
      cycles = printWhole(cpu);
      break;
    case 11:      
       
       for(int i = 0 ; i < 0x10000 ; ++i){
      // for(int i = 53248 ; i < 53280 ; ++i){
	if(i % 32 == 0){
	  printf("\n");
	  printf("%d - ",c++);
	}

	printf("%3d ",cpu.memory->physicalAt(i));
      }
      printf("\n");
      break;
    case RAND_INT:
	cycles = randInt(cpu);
	break;
    case WAIT:
	cycles = wait(cpu);
	break;
    case SIGNAL:
	cycles = signal(cpu);
	break;
    default:
	break;
    }
  }else{
    cout << "Not found input.txt or output.txt" << endl;
  }
  return cycles;
}
/*
 * Called when scheduling happened
 * Prints PID,PC,MEM_BASE Content and Address,SP
 **/
uint64_t GTUOS::printWhole(CPU8080& cpu){
  uint16_t lastProcess = 0x00cffe;
  uint16_t ptableStart = 0x0d000;
  uint16_t ptableLen = 0x00100;

  uint16_t ptableAddr = ptableStart;
  uint8_t pid = cpu.memory->at(lastProcess);

  uint8_t tempPid = pid;
  while(tempPid > 0){
    ptableAddr += ptableLen;
    tempPid--;
  }
  uint16_t pc = (cpu.memory->at(ptableAddr + 10) << 8) | cpu.memory->at(ptableAddr + 9);
  uint16_t sp = (cpu.memory->at(ptableAddr + 8) << 8) | cpu.memory->at(ptableAddr + 7);
  uint16_t base = (cpu.memory->at(ptableAddr + 12) << 8) | cpu.memory->at(ptableAddr + 11);
  uint16_t baseContent = base + pc;
  uint16_t processNameAddr = (cpu.memory->at(ptableAddr + 17) << 8) | cpu.memory->at(ptableAddr + 16);

  cout << "\n----Context Scheduling----" << endl;
  printf("PID   :\t%4d | ",  pid);
  printf("PNAME    : ");

  while(cpu.memory->at(processNameAddr) != (uint8_t) 0){
    cout << cpu.memory->at(processNameAddr);
    processNameAddr++;
  }
  printf("\t| ");

  printf("PC    :\t%4x | ",  pc);
  printf("SP    :\t%4x | ",  sp);
  printf("BASE  :\t%4x | ",  base);
  printf("BASE_CONTENT  :\t%4x\n",  baseContent);
  cout << endl;
  
  std::ofstream myfile;//it is to write file 
   std::ofstream myfile2;//it is to write file 
  int i;
  myfile.open("output.txt", std::ios_base::app);//it is to write file
  myfile2.open("localList.txt", std::ios_base::app);//it is to write file
  myfile<<"------------------------CONTEXT SWITCH HAS OCCURED---------------------------"<<endl;
  for(i=2000;i<2053;i++){
	if(i==2000){
		myfile<<i<<":"<<(int)cpu.memory->physicalAt(i)<<"(mutex)"<<endl;
	}
	else if(i==2001){
		myfile<<i<<":"<<(int)cpu.memory->physicalAt(i)<<"(empty)"<<endl;
	}
	else if(i==2002){
		myfile<<i<<":"<<(int)cpu.memory->physicalAt(i)<<"(full)"<<endl;
	}
	else
		myfile<<i<<":"<<(int)cpu.memory->physicalAt(i)<<endl;
  }
  myfile2<<"------------------------CONTEXT SWITCH HAS OCCURED---------------------------"<<endl;
  for(i=50000;i<50053;i++){
	
		myfile2<<i<<":"<<(int)cpu.memory->physicalAt(i)<<endl;
  }
  return CYCLE;
}

/*
 * Called to set the quantum time of scheduling
 */
uint64_t GTUOS::setQuantum(CPU8080& cpu){
  uint8_t quantum = cpu.state->b;
  cpu.setQuantum(quantum);
  return 0;
}

/*
 * Caleld when a process wants to exit
 * Removes process from the memory,
 * process table and reduces number of processes
 * by one
 */
uint64_t GTUOS::processExit(CPU8080& cpu){

  uint16_t baseProcessAddr = 0x00500;
  uint16_t processLen = 512;

  uint16_t ptableStart = 0x0d000;
  uint16_t ptableLen = 256;

  uint16_t ptableAddr = ptableStart;
  
  uint16_t currProcessAddr = ((Memory*)(cpu.memory))->getBaseRegister();
  uint16_t tempProcessAddr = currProcessAddr;

  uint16_t schedulerAddr = 0x00040;
  
  int i = 0;
  while(tempProcessAddr != baseProcessAddr){
    tempProcessAddr -=processLen;
    i++;
  }

  while(i > 0){
    ptableAddr += ptableLen;
    i--;
  }

  uint16_t lastProcess = 0x0cffe;
  uint16_t processCount = 0x0cfff;
  
  ((Memory*)(cpu.memory))->setBaseRegister(0);
  //memset(&cpu.memory->at(currProcessAddr),0,processLen);
  memset(&cpu.memory->at(ptableAddr),0,ptableLen);

  //cpu.memory->at(lastProcess) = cpu.memory->at(lastProcess) - 1;
  cpu.memory->at(processCount) = cpu.memory->at(processCount) - 1;
  cpu.state->pc = schedulerAddr; //go back to scheduler
  return (CYCLE * 8);
}

/*
 * Loads a new process to the given address 
 **/
uint64_t GTUOS::loadExec(CPU8080& cpu){
  uint16_t fileNameAddr;
  uint16_t processStartAddr;
  
  char* fileName = (char* ) malloc(sizeof(uint8_t) * 64);

  fileNameAddr = ((uint16_t)cpu.state->b << 8) | cpu.state->c;

  int i = 0;
  while(cpu.memory->at(fileNameAddr) != (uint8_t) 0){
    fileName[i] = cpu.memory->at(fileNameAddr);
    fileNameAddr++;
    i++;
  }
  fileName[i] = 0;

  processStartAddr = ((uint16_t)cpu.state->h << 8) | cpu.state->l;
  
  cpu.ReadFileIntoMemoryAt(fileName, processStartAddr);
  return (CYCLE * 10);
}

/*
  Print the contents of register B
*/
uint64_t GTUOS::sysPrintB( CPU8080& cpu){
  //cout << "-";
  cout << (int)cpu.state->b << endl;
  //printf("%d",(int)cpu.state->b);
  return CYCLE;
}
/*
  Print the content of memory pointed by B and C
  Calculate start address first then get the block
*/
uint64_t GTUOS::sysPrintMem( CPU8080& cpu){
  //uint16_t fileNameAddr;
  //fileNameAddr = ((uint16_t)cpu.state->b << 8) | cpu.state->c;
  //cout << (int)cpu.memory->physicalAt(index-1);
  int val=(int)cpu.memory->physicalAt(index-1);
  cpu.memory->physicalAt(indexForLocalList)=val;
  indexForLocalList++;
  cpu.memory->physicalAt(index-1)=0;
  index--;
  //printf("%02x\n",(int)cpu.memory->at(fileNameAddr));
  return CYCLE;
}

/*
  Read an integer and put it to B
*/
uint64_t GTUOS::sysReadB( CPU8080& cpu){
  uint16_t num;
  cin >> num;
  cpu.state->b = num;
    
  return CYCLE;
}

/*
  Read an integer and put it to memory address pointed
  by B and C
*/
uint64_t GTUOS::sysReadMem( CPU8080& cpu){
  	uint8_t num;
        num=cpu.state->b;
	//cout<<"NUM:"<<(int)num<<endl;
        if (num>=0 && num<=255){
            cpu.memory->physicalAt(index) = (uint8_t)num;
        }
        else{
            cout<<"You entered wrong value."<<endl;
        }
        index++;
    return CYCLE;
}

/*
  Print the null terminated string
  whose start address is stored cin B and C
*/
uint64_t GTUOS::sysPrintStr( CPU8080& cpu){
  uint16_t fileNameAddr;
  fileNameAddr = ((uint16_t)cpu.state->b << 8) | cpu.state->c;
  
  while(cpu.memory->at(fileNameAddr) != (uint8_t) 0){
    cout << cpu.memory->at(fileNameAddr);
    fileNameAddr++;
  }

  return CYCLE;
}
/*
  Read string and put it to memory address
  pointed by B and C
*/
uint64_t GTUOS::sysReadStr( CPU8080& cpu){
  uint16_t fileNameAddr;
  string str;
  cin >> str;
  fileNameAddr = (((uint16_t)cpu.state->b) << 8) | cpu.state->c;
  for(int i = 0 ; i < (signed) str.length() ; ++i){
    cpu.memory->at(fileNameAddr) = str[i];
    fileNameAddr++;
  }
  cpu.memory->at(fileNameAddr) = (uint8_t)'\0'; 
  return CYCLE;
}

uint64_t GTUOS::randInt( CPU8080& cpu){
	uint8_t num;	
	srand(timer);
	num=rand()%256;
	cpu.state->b = num;
	//std::cout << intOfStateB << std::endl
	srand(timer);
	return (CYCLE*6);
}

uint64_t GTUOS::wait( CPU8080& cpu){
	int semaphore;
	int semaphoreAdress;
	
	//cout<<"base register"<<((Memory *)cpu.memory)->getBaseRegister()<<endl;
	//cout<<"c value: "<<(int)cpu.state->c<<endl;
	semaphoreAdress=(int)cpu.state->c + 2000;
	
	//cout<<"Adress: "<<semaphoreAdress<<endl;
	semaphore=(int)(cpu.memory->physicalAt(semaphoreAdress));
	
	    //cout<<endl<<endl<<"WAIT"<<endl;
	    //cout<<"mutex: "<<(int)cpu.memory->physicalAt(2000)<<" empty: "<<(int)cpu.memory->physicalAt(2001)<<" full: "<<(int)cpu.memory->physicalAt(2002)<<endl;
	    //cout<<" semp_addr: "<<semaphoreAdress<<" semo_val: "<<semaphore<<endl<<endl<<endl;
	//cout<<"mutex: "<<semaphore<<endl;
	//printf("address : %d, val : %d\n", semaphoreAdress, semaphore);
	//sleep(1);
		
	
	
	if(semaphore==0){//context switch olmalı. Interrupt geldiğinde 8080emu.cpp içindeki rst 5 kodunun ayınısı yapmak gerekmez mi ?
		
		//cpu.dispatchScheduler();
		cpu.onInterrupt();	
		//cout<<"base register: "<<(int)((Memory *)cpu.memory)->getBaseRegister();	
		((Memory *)cpu.memory)->setBaseRegister(0);		
		
		
	        cpu.state->pc = 0x29;
	
		/*uint16_t ret = cpu.state->pc + 2;
		WriteMem(cpu.memory, cpu.state->sp - 1, (ret >> 8) & 0xff);
	        WriteMem(cpu.memory, cpu.state->sp - 2, (ret & 0xff));
		cpu.state->sp = cpu.state->sp - 2;
	        cpu.state->pc = 0x28;*/
		//cout<<"value:"<<(int)cpu.memory->at(1491)<<endl;
	}
	else{
		semaphore--;
		cpu.memory->physicalAt(semaphoreAdress)=semaphore;
	}	
		
	
	//cout<<"counter: "<<counter<<endl;
	
	return (CYCLE*20);
}

/*uint64_t GTUOS::signal( CPU8080& cpu){
	/*int semaphore;
	int semaphoreAdress;
	

	semaphoreAdress=(int)cpu.state->c + 2000;
	semaphore=(int)(cpu.memory->at(semaphoreAdress));
	
	if(semaphore==50 )){
		//cout<<"Context switch must be happen"<<endl;
		cpu.onInterrupt();
	        cpu.state->pc = 0x29;
		//cout<<"pc:"<<cpu.state->pc<<endl;
	}
	else{
		semaphore++;
		cpu.memory->at(semaphoreAdress)=semaphore;
	}

	
	return (CYCLE*20);

}*/

uint64_t GTUOS::signal(CPU8080 & cpu) {
    uint16_t semp_addr = 2000 + cpu.state->c; // Semaphore no is kept in register C
    //printf("semp_addr : %d\n", semp_addr);
    int semp_val = (int)cpu.memory->physicalAt(semp_addr);
    //printf("semp_val : %d\n", semp_val);
    //cout<<endl<<endl<<"SIGNAL"<<endl;
    //cout<<"mutex: "<<(int)cpu.memory->physicalAt(2000)<<" empty: "<<(int)cpu.memory->physicalAt(2001)<<" full: "<<(int)cpu.memory->physicalAt(2002)<<endl;
    //cout<<" semp_addr: "<<semp_addr<<" semo_val: "<<semp_val<<endl<<endl<<endl;

    
    if (cpu.state->c == 0) { // if the semaphore is mutex
        if (!semp_val) { 
            ++semp_val;
            //printf("MUTEX | val : %d is assigned to address : %d\n", semp_val, semp_addr);
            cpu.memory->physicalAt(semp_addr) = semp_val;
        }
        else { 
            // do nothing
        }
        
    }
    else if (cpu.state->c == 2 || cpu.state->c == 1) { // if the semaphore is not mutex
        if (semp_val < 50) {
            ++semp_val;
            //printf("SEMAPHORE | val : %d is assigned to address : %d\n", semp_val, semp_addr);
            cpu.memory->physicalAt(semp_addr) = semp_val;
        }
        else { // needs to be blocked without busy waiting : context switching
	     cpu.onInterrupt();		
	     ((Memory *)cpu.memory)->setBaseRegister(0);		
		
		
	     cpu.state->pc = 0x29;
             //printf("SEMAPHORE | context switch must be occurred\n");
        }
    }
    else { // error
        fprintf(stderr, "Wrong semaphore index\n");
    }
}

void GTUOS::WriteMem(MemoryBase *memory, uint16_t address, uint8_t value) {
      memory->at(address) = value;
}
