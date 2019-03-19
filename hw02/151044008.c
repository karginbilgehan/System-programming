#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "151044008.h"

int ARG_CONTROL=1;//it is necessary to control -z argument.

int main(int argc, char *argv[])
{
   pid_t pid;
   if (argc==3 && (strcmp(argv[1],"-z")==0)){//it is for using with -z argument
	ARG_CONTROL=2;
	int total_size=postOrderApply(argv[2],sizepathfun);
	if(total_size!=-1){
		total_size=total_size/1024;
		pid=getpid();		
		printf("%d \t \t %d \t \t %s \n",pid,total_size,argv[2]);
	}    	
	
    }
    else if(argc==2) // it is for using without -z argument. Only two parameters
    {
	int total_size=postOrderApply(argv[1],sizepathfun);
    	if(total_size!=-1){
		total_size=total_size/1024;
		pid=getpid();
		printf("%d \t \t %d \t \t %s \n",pid,total_size,argv[1]);
	} 
    }
    else{// This block is for error situation.
    		printf("This usage is wrong. There must be 2 or 3 argument. \n");
		printf("Usage: ./buNeDu -z path_name or ./buNeDu path_name \n");
    }   
    return 0;
}
// This function return the size of a given file or 
// return -1, if the file is a special like link or pipe or 
// return -2, if file properties not avaliable 
int sizepathfun(char *path){
    int size=0;
    struct stat st;
    if(lstat(path,&st)<0){
	  return -2;
    }
    if(lstat(path,&st)>=0){
	  size=st.st_size;
    } 
    if (!(S_ISREG(st.st_mode)) && !(S_ISDIR(st.st_mode))){ //control for regular file or directory
	  return -1;
    }
    return size;
}
// This function traverses the tree,starting at path. 
//It applies the pathfun function to each file that it encounters in the traversal. 
//The function returns the sum of the positive return values of pathfun, or -1 (if it failed to traverse any subdirectory) 
int postOrderApply(char *path,int pathfun (char *path1)){
 
 DIR *directory=opendir(path);
 struct dirent *currentDir;
 char filePath[1024]; // it is for file path like A/B/C
 int size=0;
 int total_size=0;
 pid_t childPid,id;
  if (directory == NULL){
	printf("%s: This directory can not be open.\n",path);       
	return -1;
  }
 
 
  while((currentDir = readdir(directory)) != NULL ){


    if ( (strcmp(currentDir->d_name, ".") == 0) || (strcmp(currentDir->d_name, "..") == 0) ) // checks the status of being itself or its up folder
        continue;
    
    sprintf(filePath, "%s/%s", path , currentDir->d_name); // it is to specifiy the path name
    size=pathfun(filePath);
    if(size==-1){// it is to control special file.
	id=getpid();
	printf("%d \t \t \t \t Special file %s \n",id,currentDir->d_name);
	size=0;
    }
    
    if (DT_DIR==currentDir->d_type && ARG_CONTROL==2) // it is to control using with 3 argument like ./buNeDu -z A.
    {
	int fds[2];
	pipe (fds);
	childPid=fork();
	if(childPid==0){
		int directory_size=postOrderApply(filePath,pathfun);
		if(directory_size!=-1 || directory_size!=-2){
			
			close (fds[0]); //Okuma kısmı kapatıldı
			write(fds[1],&directory_size,sizeof(directory_size));
			close(fds[1]);
			id=getpid();
			printf("%d \t \t %d \t \t %s \n",id,directory_size/1024,filePath);
			//total_size=total_size+directory_size;	
		}
		_exit(0);	
	}
	else{
		waitpid(childPid,NULL,0);
		int directory_size;	
		
		close(fds[1]);
		read(fds[0],&directory_size,sizeof(directory_size));
		//id=getpid();
		//printf("%d \t \t %d \t \t %s \n",id,directory_size/1024,filePath);
		close(fds[0]);
		total_size=total_size+directory_size;	
		
	}
		
	
    }
    else if(DT_DIR==currentDir->d_type && ARG_CONTROL==1){ // it is to control using with 2 argument like ./buNeDu A.
	childPid=fork();	  	
	if(childPid==0){
		int directory_size=postOrderApply(filePath,pathfun);
        	if(directory_size!=-1 || directory_size!=-2){
			id=getpid();
			printf("%d \t \t %d \t \t %s \n",id,directory_size/1024,filePath);	
		}
		_exit(0);
	}	
	
	else{
		waitpid(childPid,NULL,0);
	} 
		
    }
    else{//it is for total size
        total_size=size+total_size;
    }
    
  }
    closedir(directory);
    return total_size;
}



