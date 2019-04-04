#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>


int ARG_CONTROL=1;
int sizepathfun(char *path);
int postOrderApply(char *path,int pathfun (char *path1));

int main(int argc, char *argv[])
{
   //printf("argc: %d \n",argc);
   if (argc==3 && (strcmp(argv[1],"-z")==0)){
	ARG_CONTROL=2;
	int total_size=postOrderApply(argv[2],sizepathfun);
	if(total_size!=-1){
		total_size=total_size/1024;
		printf("%d \t \t %s \n",total_size,argv[2]);
	}    	
	
    }
    else if(argc==2)
    {
	int total_size=postOrderApply(argv[1],sizepathfun);
    	if(total_size!=-1){
		total_size=total_size/1024;
		printf("%d \t \t %s \n",total_size,argv[1]);
	} 
    }
    else{
    	printf("This usage is wrong. There must be 2 or 3 argument. \n");
	printf("Usage: ./buNeDu -z path_name or ./buNeDu path_name \n");
    }   
    return 0;
}

int sizepathfun(char *path){
    int size=0;
    struct stat st;
    if(lstat(path,&st)<0){
	  return -2;
    }
    if(lstat(path,&st)>=0){
	  size=st.st_size;
    } 
    if (!(S_ISREG(st.st_mode)) && !(S_ISDIR(st.st_mode))){
	  return -1;
    }
    return size;
}

int postOrderApply(char *path,int pathfun (char *path1)){
 
 DIR *directory=opendir(path);
 struct dirent *currentDir;
 char filePath[1024];
 int size=0;
 int total_size=0;
 
  if (directory == NULL){
	printf("%s: This directory can not be open.\n",path);       
	return -1;
  }
 
 
  while((currentDir = readdir(directory)) != NULL ){


    if ( (strcmp(currentDir->d_name, ".") == 0) || (strcmp(currentDir->d_name, "..") == 0) )
        continue;
    
    sprintf(filePath, "%s/%s", path , currentDir->d_name);
    
    size=pathfun(filePath);
    if(size==-1){
	printf("Special file %s \n",currentDir->d_name);
	size=0;
    }
    
    if (DT_DIR==currentDir->d_type && ARG_CONTROL==2)
    {
	
	int directory_size=postOrderApply(filePath,pathfun);
		
	if(directory_size!=-1 || directory_size!=-2){
		printf("%d \t \t %s \n",directory_size/1024,filePath);
		total_size=total_size+directory_size;	
	}		
	
    }
    else if(DT_DIR==currentDir->d_type && ARG_CONTROL==1){ // -z parametresi olmayan çağrı için
  	int directory_size=postOrderApply(filePath,pathfun);
        if(directory_size!=-1 || directory_size!=-2){
		printf("%d \t \t %s \n",directory_size/1024,filePath);	
	}
    }
    else{//txt vs dosyalari için
        total_size=size+total_size;
    }
    
  }
    closedir(directory);
    return total_size;
}


