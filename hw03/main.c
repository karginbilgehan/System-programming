#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "lsf.h"
#include "cat.h"
#include "pwd.h"
#include "lsf.h"

int exitCall();
void findCommand(char* command);

int main(int argc, char* argv[]){
	int exitStat=1;
	char buffer[1024];
	while(exitStat){
		printf(">$");
		fgets(buffer,sizeof(buffer),stdin);	
		findCommand(buffer);
	}
}

int exitCall(){
    exit(1);		  
}

void findCommand(char *command){
       pid_t childPid;
       //printf("value %d \n",strcmp(command,"exit\n"));
       if(strcmp(command,"exit\n")==0){
		exitCall();
       }
       else if(strcmp(command,"pwd\n")==0){
		childPid=fork();
		if(childPid==0){
			char *args[]={"./pwd",NULL}; 
			execv(args[0],args);
		}
		else{
			wait(NULL);
		}
       }

       else if(strcmp(command,"lsf\n")==0){
		childPid=fork();
		if(childPid==0){
			char *args[]={"./lsf",NULL}; 
			execv(args[0],args);
		}
		else{
			wait(NULL);
		}
       }
}


