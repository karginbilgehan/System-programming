#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "pwd.h"


int main (int argc, char*argv[]){

	pwd();
	return 0;
}


int pwd(){
	
	char pathName[1024];
	int size=sizeof(pathName);
	if(getcwd(pathName,size)!=NULL){
		fprintf(stdout,"%s \n",pathName);
		return 0;
	}
	else {
		perror("It is an error for pwd. \n");
		return -1;
	}
		
}
