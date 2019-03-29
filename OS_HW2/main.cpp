#include <iostream>
#include "8080emuCPP.h"
#include "gtuos.h"
#include "memory.h"

	// This is just a sample main function, you should rewrite this file to handle problems 
	// with new multitasking and virtual memory additions.
int main (int argc, char**argv)
{
	if (argc != 3){
		std::cerr << "Usage: prog exeFile debugOption\n";
		exit(1); 
	}
	int DEBUG = atoi(argv[2]);
	
	Memory mem(100000);
	CPU8080 theCPU(&mem);
	GTUOS	theOS;

	theCPU.ReadFileIntoMemoryAt(argv[1], 0x0000);	
 
	do	
	{
		theCPU.Emulate8080p(DEBUG);
		
		if(DEBUG==2){
			getchar();		
		}
		if(theCPU.isSystemCall()){
						
			theOS.handleCall(theCPU);
		}
			
		
	}	while (!theCPU.isHalted()
		
			);
		printf("256 deki value %x \n",mem.physicalAt(0x2710));
		printf("257 deki value %x \n",mem.physicalAt(0x2711));
		printf("258 deki value %x \n",mem.physicalAt(0x2712));
		printf("259 deki value %x \n",mem.physicalAt(0x2713));
		printf("260 deki value %x \n",mem.physicalAt(0x2714));
		printf("261 deki value %x \n",mem.physicalAt(0x2715));
		printf("262 deki value %x \n",mem.physicalAt(0x2716));
		printf("263 deki value %x \n",mem.physicalAt(0x2717));
		printf("264 deki value %x \n",mem.physicalAt(0x2718));
		printf("265 deki value %x \n",mem.physicalAt(0x2719));
		printf("266 deki value %x \n",mem.physicalAt(0x271A));
		printf("267 deki value %x \n",mem.physicalAt(0x271B));
		printf("268 deki value %x \n",mem.physicalAt(0x271C));	
		printf("268 deki value %x \n",mem.physicalAt(0x271D));		
		return 0;
}

