#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int ARG_CONTROL=1;

int sizepathfun(char *path){
    int size=0;
    struct stat st;
    if(lstat(path,&st)>=0){
		size=st.st_size;
    } 
    if (!(S_ISREG(st.st_mode)) && !(S_ISDIR(st.st_mode))){
	    return -1;
    }
    return size;
}

int depthFirstApply(char *path,int pathfun (char *path1)){
 
 DIR *directory=opendir(path);
 struct dirent *dit;
 char filePath[1024];
 int size=0;
 int total_size=0;
 
  if (directory == NULL){
	printf("%s: This directory can not be open.\n",path);       
	return -1;
  }
 
 
  while((dit = readdir(directory)) != NULL ){


    if ( (strcmp(dit->d_name, ".") == 0) || (strcmp(dit->d_name, "..") == 0) )
        continue;
    
    sprintf(filePath, "%s/%s", path , dit->d_name);
    
    size=pathfun(filePath);
    if(size==-1){
	printf("Special File %s \n",dit->d_name);
	size=0;
    }
    
    /*if(DT_DIR!=dit->d_type){
	printf("%s",path);
    	printf("-->");
    	printf("%s\t\t",dit->d_name);
    	printf("size: %d \n",size);
    }*/
    
    if (DT_DIR==dit->d_type && ARG_CONTROL==2)
    {
	
	int directory_size=depthFirstApply(filePath,pathfun);
		
	if(directory_size!=-1){
		printf("%d \t \t %s \n",directory_size/1024,filePath);
		total_size=total_size+directory_size;	
	}		
	
    }
    else if(DT_DIR==dit->d_type && ARG_CONTROL==1){ // -z parametresi olmayan çağrı için
  	int directory_size=depthFirstApply(filePath,pathfun);
        if(directory_size!=-1){
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

int main(int argc, char *argv[])
{
   
   if (argc==3 && (strcmp(argv[1],"[-z]")==0)){
	ARG_CONTROL=2;
	int total_size=depthFirstApply(argv[2],sizepathfun);
	if(total_size!=-1){
		total_size=total_size/1024;
		printf("%d \t \t %s \n",total_size,argv[2]);
	}    	
	
    }
    else if(argc==2)
    {
	int total_size=depthFirstApply(argv[1],sizepathfun);
    	if(total_size!=-1){
		total_size=total_size/1024;
		printf("%d \t \t %s \n",total_size,argv[1]);
	} 
    }   
    return 0;
}
