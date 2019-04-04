#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <string.h>
#include <fcntl.h>


#define COMMAND_SIZE 256


int exitCall();
void cd(char *command[]);
void findCommand(char ** split_command,char *command,int* pipe);
void selectCommand(char *command[],int* pipe);
void selectCommandForPipe(char* parseCommand[]);
void help();

int beforePipeCounter=0;
int afterPipeCounter=0;
char currentDir[1024];

int main(int argc, char* argv[]){
	int exitStat=1;
	char buffer[COMMAND_SIZE];
	char historyBuff[1024][COMMAND_SIZE];//it is for !n command.
	int historyIndex=0;
	int forPipe=0;
	getcwd(currentDir,sizeof(currentDir));
	while(exitStat){
		char **split_command = malloc(10 * sizeof(char*));
		int i = 0;
		for(i = 0; i<10; ++i){
			split_command[i] = malloc(sizeof(char) * COMMAND_SIZE);	
		}
		printf(">$");
		fgets(buffer,sizeof(buffer),stdin);
		
		
		if(buffer[0]=='!'){//it is for !n command.
			int index=(int)buffer[1]-48;
			//printf("index: %d \n",index);
			findCommand(split_command,historyBuff[index],&forPipe);
			
		}			
		
		else{//all commands except !n.
			strcpy(historyBuff[historyIndex],buffer);//it is to keep al command in an array.
			//printf("history: %s \n",historyBuff[historyIndex]);
			historyIndex++;
			findCommand(split_command,buffer,&forPipe);
		}
		
		/*for(i=0;i<historyIndex;++i){
			printf("history[%d]: %s \n",i,historyBuff[i]);
		}*/
		//printf("%s\n",split_command[0]);
		//printf("whole command %s \n",split_command[0]);
		
		selectCommand(split_command,&forPipe);
		
		beforePipeCounter=0;
		afterPipeCounter=0;
		forPipe=0;
		
		for (i = 0; i < 10; i++)
		{
    			char* currentIntPtr = split_command[i];
   			free(currentIntPtr);
		}
		free(split_command);
	
	}
}

void findCommand(char ** split_command,char *command,int* pipe){
	int index=0;
	int tempIndex=0;
	int splitIndex=0;
	
	int i;
	char aCommand[COMMAND_SIZE];
	char tempCommand[COMMAND_SIZE]={0};
	
	
	while(command[index]!='\n'){
		if(command[index]=='|'){
			*pipe=1;
		}
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
			if(*pipe==0){
				beforePipeCounter++;
			}
			if(*pipe==1){
				afterPipeCounter++;
			}
		}
		
		if(command[index+1] == '\n'){

			
			strcpy(split_command[splitIndex],tempCommand);
			//printf("command %d : %s \n",splitIndex,split_command[splitIndex]);
			splitIndex++;
			for(i=0;i<tempIndex;++i){
				tempCommand[i]='\0';
			}
			tempIndex=0;
			//*afterPipeCounter+=1;
		}					
		index++;						
			
	}	
		
}	
void selectCommand(char *command[],int* pipe){
       pid_t childPid;
       //printf("value %d \n",strcmp(command,"exit\n"));
       char curDir[COMMAND_SIZE];

       if(strcmp(command[0],"exit")==0){
		exitCall();
       }
       else if(strcmp(command[0],"help")==0){
		help();
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
		if(*pipe==0){
			childPid=fork();
			if(childPid==0){
				char *args[]={"./pwd",NULL}; 
				execv(args[0],args);
			}
			else{
				wait(NULL);
			}
		}
		if(*pipe ==1){
			selectCommandForPipe(command);
		}		
		
       }

       else if(strcmp(command[0],"lsf")==0){
		if(*pipe==0){
			childPid=fork();
			if(childPid==0){
				char *args[]={"./lsf",NULL}; 
				execv(args[0],args);
			}
			else{
				wait(NULL);
			}
		}
		if(*pipe ==1){
			selectCommandForPipe(command);
		}		
		
	}
       else if(strcmp(command[0],"cat")==0){
		if(*pipe ==0){
			childPid=fork();
			if(childPid==0){
				char *args[]={"./cat",command[1]}; 
				execv(args[0],args);
			}
			else{
				wait(NULL);
			}
		}
		if(*pipe ==1){
			selectCommandForPipe(command);
		}		
		
       } 
       else if(strcmp(command[0],"wc")==0){
		if(*pipe==0){
			//sprintf(curDir,"%s%s",currentDir,"/wc");
			//printf("current: %s \n",currentDir);
			//printf("current dir: %s \n",curDir);
			childPid=fork();
			if(childPid==0){
				
				//printf("cuurent:%s \n",curDir);
				char *args[]={"./wc",command[1]}; 
				execv(args[0],args);
			}
			
			
			else{
				wait(NULL);
			}	
		}		
		if(*pipe ==1){
			selectCommandForPipe(command);
		}
       }
}
void selectCommandForPipe(char* parseCommand[]){
	//printf("afterPipeCounter %d \n",afterPipeCounter);
	//printf("beforePipeCounter %d \n",beforePipeCounter);
	char command[COMMAND_SIZE][COMMAND_SIZE];	
	int pfd[2];
	if(pipe(pfd)==-1)
		printf("pipe");

	switch(fork()){
		case -1:
			printf("fork");
		case 0:
			if(close(pfd[0])==-1)
				printf("close 1");
			if(pfd[1] != STDOUT_FILENO){
				if(dup2(pfd[1], STDOUT_FILENO)==-1)
					printf("dup2 1");
				if(close(pfd[1]) == -1)	
					printf("close 2");		
			}
			if (beforePipeCounter==1){
				sprintf(command[0],"%s%s","./",parseCommand[0]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[0],(char*) NULL);
				//printf("execlp ls");
			}
			else if (beforePipeCounter==2){
				//printf("girdim \n");
				sprintf(command[0],"%s%s","./",parseCommand[0]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[0],parseCommand[1],(char*) NULL);
				printf("execlp ls");
			}
			else if (beforePipeCounter==3){
				sprintf(command[0],"%s%s","./",parseCommand[0]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[0],parseCommand[1],parseCommand[2],(char*) NULL);
				//printf("execlp ls");
			
			}
			
		default:
			break;
	}

	switch(fork()){
		case -1:
			printf("fork");
		case 0:
			if(close(pfd[1])==-1)
				printf("close 3");
			if(pfd[0] != STDIN_FILENO){
				if(dup2(pfd[0], STDIN_FILENO)==-1)
					printf("dup2 2");
				if(close(pfd[0]) == -1)	
					printf("close 4");		
			}
			//printf("parse Command: %s \n",parseCommand[2]);
			if(beforePipeCounter==1 && afterPipeCounter==1){
				sprintf(command[0],"%s%s","./",parseCommand[2]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[2],(char*) NULL);

			}
			else if(beforePipeCounter==1 && afterPipeCounter==2){
				sprintf(command[0],"%s%s","./",parseCommand[2]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[2],parseCommand[3],(char*) NULL);

			}
			else if(beforePipeCounter==1 && afterPipeCounter==3){
				sprintf(command[0],"%s%s","./",parseCommand[2]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[2],parseCommand[3],parseCommand[3],(char*) NULL);

			}
			else if(beforePipeCounter==2 && afterPipeCounter==1){
				sprintf(command[0],"%s%s","./",parseCommand[3]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[3],(char*) NULL);
	
			}
			else if(beforePipeCounter==2 && afterPipeCounter==2){
				sprintf(command[0],"%s%s","./",parseCommand[3]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[3],parseCommand[4],(char*) NULL);
	
			}

			else if(beforePipeCounter==2 && afterPipeCounter==3){
				sprintf(command[0],"%s%s","./",parseCommand[3]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[3],parseCommand[4],parseCommand[5],(char*) NULL);
	
			}
			else if(beforePipeCounter==3 && afterPipeCounter==1){
				sprintf(command[0],"%s%s","./",parseCommand[4]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[4],(char*) NULL);
	
			}
			else if(beforePipeCounter==3 && afterPipeCounter==2){
				sprintf(command[0],"%s%s","./",parseCommand[4]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[4],parseCommand[5],(char*) NULL);
	
			}
			else if(beforePipeCounter==3 && afterPipeCounter==3){
				sprintf(command[0],"%s%s","./",parseCommand[4]);
				//printf("command: %s \n",command[0]);
				execlp(command[0],parseCommand[4],parseCommand[5],parseCommand[6],(char*) NULL);
	
			}			
		
		default:
			break;
	}

	if(close(pfd[0])==-1)
		printf("close 5");
	if(close(pfd[1])==-1)
		printf("close 6");
	if(wait(NULL)==-1)
		printf("wait 1");
	if(wait(NULL)==-1)
		printf("wait 2");

	

}

int exitCall(){
    exit(1);		  
}
//cd ile istenilen yere gidiyor ancak gidilen dosya icinde pwd lsf gibi komutları görmüyor çünkü o komutların executableları o dosyanın içinde yoookk
void cd(char *command[]){
    
    if (command[1] == NULL) {
        fprintf(stderr, "CD:Wrong Argument ->\n");
    } 
    else {
        if (chdir(command[1]) != 0) {
            perror("ERROR !!!");
        }
    }
   
}

void help(){

	printf("Supported command are: pwd/ lsf/ cat/ wc/ bunedu/ cd/ exit/ \n");
	printf("\n");
	printf("Description of pwd: Print the full name of the current working directory. \n");
	printf("Usage: >$ pwd\n");
	printf("\n");
	printf("Description of lsf: List the information about the files. Like file type, access right, file size and file name. \n");
	printf("Usage: >$ lsf\n");
	printf("\n");
	printf("Description of cat: Concatenate FILE(s), or standart input, to standart output. \n");
	printf("Usage: >$ cat myfile.txt\n");
	printf("\n");
	printf("Description of wc: Print on standard output the number of lines in the file provided to it as argument or the string coming from standard input until EOF character. \n");
	printf("Usage: >$ wc example.c\n");
	printf("\n");
	printf("Description of bunedu: Summmarize disk of usage each file, recursively for directories. \n");
	printf("Usage: >$ bunedu -z A or >$ bunedu A\n");
	printf("\n");
	printf("Description of cd: Change the present working directory to the location provided as argument. \n");
	printf("Usage: >$ cd example or >$ cd .. \n");
	printf("\n");
	printf("Description of exit: Causes normal process termination. \n");
	printf("Usage: >$ exit\n");
	printf("\n");
}
