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


int main (int argc, char*argv[]){
	lsf();
}


int lsf(){
	
    DIR *directory;
    struct dirent *currentFile;
    struct stat st;
    char buffer[1024];
    char currentDir[1024];

    if (getcwd(currentDir, sizeof(currentDir)) != NULL){
                    directory = opendir(currentDir);
    }
    else{
        perror("There is an error for lsf. \n");
        exit(EXIT_FAILURE);
    }   
    
    while((currentFile = readdir(directory)) != NULL) 
    {   
        
	if ( (strcmp(currentFile->d_name, ".") == 0) || (strcmp(currentFile->d_name, "..") == 0) )
        continue;
	 
        sprintf(buffer, "%s/%s",currentDir, currentFile->d_name);
        stat(buffer, &st);

      
	if(S_ISREG(st.st_mode)){
		printf("R");
	}        
	else{
		printf("S");
	}
	
	printf( (st.st_mode & S_IRUSR) ? "r" : "-");
        printf( (st.st_mode & S_IWUSR) ? "w" : "-");
        printf( (st.st_mode & S_IXUSR) ? "x" : "-");
        printf( (st.st_mode & S_IRGRP) ? "r" : "-");
        printf( (st.st_mode & S_IWGRP) ? "w" : "-");
        printf( (st.st_mode & S_IXGRP) ? "x" : "-");
        printf( (st.st_mode & S_IROTH) ? "r" : "-");
        printf( (st.st_mode & S_IWOTH) ? "w" : "-");
        printf( (st.st_mode & S_IXOTH) ? "x     " : "-     ");
        
	  
        printf("%d      ",(int)st.st_size);
	printf("%s\n", currentFile->d_name);
    }
    closedir(directory);

    return 1;

}

