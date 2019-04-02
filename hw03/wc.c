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
	close(fd);
	int NOL=wc(argv[1],size);
	fprintf(stdout,"%d %s\n",NOL,argv[1]);
	return 0;
}	


int wc(char* fileName,int fileSize){
       FILE *fp;
       char text[fileSize];
       int counter=0;
       fp = fopen(fileName,"r");
       while(fgets(text,fileSize,fp)!='\0')
       {
	     counter ++;
       }
             
       fclose(fp);
       return counter;
}

