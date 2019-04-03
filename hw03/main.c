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

#define COMMAND_SIZE 256


int exitCall();
void cd(char *command[]);
void findCommand(char ** split_command,char *command);
void selectCommand(char *command[]);

int main(int argc, char* argv[]){
	int exitStat=1;
	char buffer[COMMAND_SIZE];
	char historyBuff[1024][COMMAND_SIZE];
	
	while(exitStat){
		char **split_command = malloc(10 * sizeof(char*));
		int i = 0;
		for(i = 0; i<10; ++i){
			split_command[i] = malloc(sizeof(char) * COMMAND_SIZE);	
		}
		printf(">$");
		fgets(buffer,sizeof(buffer),stdin);
		
		strcpy(historyBuff[0],buffer);
		printf("history: %s \n",historyBuff[0]);			
		findCommand(split_command,buffer);
		//printf("%s\n",split_command[0]);
		//printf("whole command %s \n",split_command[0]);
		selectCommand(split_command);
		

		for (i = 0; i < 10; i++)
		{
    			char* currentIntPtr = split_command[i];
   			free(currentIntPtr);
		}
		free(split_command);
	
	}
}

void findCommand(char ** split_command,char *command){
	int index=0;
	int tempIndex=0;
	int splitIndex=0;
	int i;
	char aCommand[COMMAND_SIZE];
	char tempCommand[COMMAND_SIZE]={0};
	while(command[index]!='\n'){
		if(command[index]!=' '){
			
			tempCommand[tempIndex]=command[index];
			tempIndex++;
			//printf("temp command: %s \n",tempCommand);
		}
		
		else{
			strcpy(split_command[splitIndex],tempCommand);
			//printf("command %d : %s \n",splitIndex,split_command[splitIndex]);
			splitIndex++;
			for(i=0;i<tempIndex;++i){
				tempCommand[i]='\0';
			}
			tempIndex=0;
		}
		
		if(command[index+1] == '\n'){

			
			strcpy(split_command[splitIndex],tempCommand);
			//printf("command %d : %s \n",splitIndex,split_command[splitIndex]);
			splitIndex++;
			for(i=0;i<tempIndex;++i){
				tempCommand[i]='\0';
			}
			tempIndex=0;

		}					
		index++;						
			
	}

}
void selectCommand(char *command[]){
       pid_t childPid;
       //printf("value %d \n",strcmp(command,"exit\n"));
       if(strcmp(command[0],"exit")==0){
		exitCall();
       }
       
       else if(strcmp(command[0],"cd")==0){
		cd(command);
       }

       else if(strcmp(command[0],"bunedu")==0){
		if(strcmp(command[1],"-z")!=0){
			childPid=fork();
			if(childPid==0){
				char *args[]={"./buNeDu",command[1]};
				args[2]=NULL;
				//printf("args: %s \n",args[2]); 
				execv(args[0],args);
			}
			else{
				wait(NULL);
			}
		}
		else if(strcmp(command[1],"-z")==0){
			childPid=fork();
			if(childPid==0){
				char *args[]={"./buNeDu",command[1]};
				strcpy(args[2],command[2]);
		
				//printf("args: %s \n",args[2]); 
				execv(args[0],args);
			}
			else{
				wait(NULL);
			}

		}
		
       }
       
       else if(strcmp(command[0],"pwd")==0){
		childPid=fork();
		if(childPid==0){
			char *args[]={"./pwd",NULL}; 
			execv(args[0],args);
		}
		else{
			wait(NULL);
		}
       }

       else if(strcmp(command[0],"lsf")==0){
		childPid=fork();
		if(childPid==0){
			char *args[]={"./lsf",NULL}; 
			execv(args[0],args);
		}
		else{
			wait(NULL);
		}
       }
       else if(strcmp(command[0],"cat")==0){
		childPid=fork();
		if(childPid==0){
			char *args[]={"./cat",command[1]}; 
			execv(args[0],args);
		}
		else{
			wait(NULL);
		}
       } 
       else if(strcmp(command[0],"wc")==0){
		childPid=fork();
		if(childPid==0){
			char *args[]={"./wc",command[1]}; 
			execv(args[0],args);
		}
		else{
			wait(NULL);
		}
       }
}

int exitCall(){
    exit(1);		  
}
//cd ile istenilen yere gidiyor ancak gidilen dosya icinde pwd lsf gibi komutları görmüyor çünkü o komutların executableları o dosyanın içinde yoookk
void cd(char *command[]){
    
    if (command[1] == NULL) {
        fprintf(stderr, "CD:Wrong Argument ->\n");
    } else {
        if (chdir(command[1]) != 0) {
            perror("ERROR !!!");
        }
    }
   
}



