#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

int ARG_CONTROL=1;

int sizepathfun(char *path){
    int size=0;
    struct stat st;
    if(stat(path,&st)>=0){
		size=st.st_size;
	} 
    return size;
}

int depthFirstApply(char *path,int pathfun (char *path1)){
 
 DIR *directory=opendir(path);
 struct dirent *dit;
 char filePath[1024];
 int size=0;
 int total_size=0;
 
 if (directory == 0){
       return 0;
 }
 
 
  while((dit = readdir(directory)) != NULL ){


    if ( (strcmp(dit->d_name, ".") == 0) || (strcmp(dit->d_name, "..") == 0) )
        continue;
    
    sprintf(filePath, "%s/%s", path , dit->d_name);
    
    size=pathfun(filePath);
    
    /*if(DT_DIR!=dit->d_type){
	printf("%s",path);
    	printf("-->");
    	printf("%s\t\t",dit->d_name);
    	printf("size: %d \n",size);
    }*/
    
    if (DT_DIR==dit->d_type && ARG_CONTROL==1)
    {
        int directory_size=depthFirstApply(filePath,pathfun);
        total_size=total_size+directory_size;
        printf("%d \t \t %s \n",directory_size/1024,filePath);
    }
    else if(DT_DIR==dit->d_type && ARG_CONTROL==2){
  	int directory_size=depthFirstApply(filePath,pathfun);
        //total_size=total_size+directory_size;
        printf("%d \t \t %s \n",directory_size/1024,filePath);
    }
    else{
        total_size=size+total_size;
    }
    
  }
   
    return total_size;
}

int main(int argc, char *argv[])
{
   
    if (argc==3 && (strcmp(argv[2],"[-z]")==0)){
	ARG_CONTROL=2;
	int total_size=depthFirstApply(argv[1],sizepathfun);
    	total_size=total_size/1024;
   	printf("%d \t \t %s \n",total_size,argv[1]);
    }
    else 
    {
	int total_size=depthFirstApply(argv[1],sizepathfun);
    	total_size=total_size/1024;
   	printf("%d \t \t %s \n",total_size,argv[1]);
    }   
    return 0;
}
