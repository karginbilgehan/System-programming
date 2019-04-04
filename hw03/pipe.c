#include <sys/wait.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <string.h>

int main(int argc,char *argv[]){

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
			
			execlp("ls","ls",(char*) NULL);
			printf("execlp ls");
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
			
			execlp("wc","wc","-l",(char*) NULL);
			printf("execlp wc");
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

	exit(EXIT_SUCCESS);
}
