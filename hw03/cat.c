#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "cat.h"


int main (int argc, char*argv[]){
	int fd,size;
	fd=open(argv[1],O_RDONLY);
	size=lseek(fd,(off_t) 0, SEEK_END);
	//printf("%d %s \n",(int)size,argv[1]);
	close(fd);
	cat(argv[1],size);
}


void cat(char* fileName,int fileSize){
       FILE *fp;
       char text[fileSize+1];
       fp = fopen(fileName,"r");
       while(fscanf(fp,"%[^\n]\n",text)!=EOF)
       {
	     fprintf(stdout,"%s\n",text);
       }
             
       fclose(fp);
}

