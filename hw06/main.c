#include <pthread.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h> 
#define PATH_LEN 256 
#define R_FLAGS O_RDONLY 
#define W_FLAGS (O_WRONLY | O_CREAT) 
#define W_PERMS (S_IRUSR | S_IWUSR)

void* producer (void* unused);
typedef struct {   
	int infd;   
	int outfd;   
	char filename[PATH_LEN]; 
} buffer_t;

buffer_t source;
char realSource[PATH_LEN];
char realDest[PATH_LEN];
/* The main program.  */
int main (int argc, char * argv[]) { 
	pthread_t thread_id;
	char currentDir[PATH_LEN];
	char curDir[2][PATH_LEN];	
	int ret;
	
	if (argc != 5){
		fprintf(stderr,"Wrong usage! \n");
		exit(EXIT_FAILURE);
	}
	getcwd(currentDir,sizeof(currentDir));
	//printf("current directory: %s \n \n",currentDir);
	sprintf(curDir[0],"%s/%s",currentDir,argv[3]);
	//printf("last currend directory: %s \n",curDir[0]);
	sprintf(curDir[1],"%s/%s",currentDir,argv[4]);
	
	//printf("last currend directory 2: %s \n",curDir[1]);
	
	strcpy(realSource,curDir[0]);
	strcpy(realDest,curDir[1]);	
	/* Create a new thread.  The new thread will run the producer.  */ 
	pthread_create (&thread_id, NULL, &producer, curDir); 
	/* Print o’s continuously to stderr.  */
  
	pthread_join(thread_id,(void*) &ret);
	//printf("retVal: %d \n",ret);
	  
	printf("\n");
	return 0; // join kullanmazsan burada ne kullandıgın önemli. Eğer join yokken return kullanırsan çalışan bütün threadleri bitirir. 
}

void* producer (void* unused) {
	char curDirectories[2][PATH_LEN];
	char filePathSource[PATH_LEN];
	char filePathDest[PATH_LEN];
	
	strcpy(curDirectories[0],((char(*)[PATH_LEN])unused)[0]); // it is for source directory
	strcpy(curDirectories[1],((char(*)[PATH_LEN])unused)[1]); // it is for destination directory
	
	//printf("temp %s \n",temp);
	DIR *directory=opendir(curDirectories[0]);
	struct dirent *currentFile;
	
	while((currentFile = readdir(directory)) != NULL ){
		  if ( (strcmp(currentFile->d_name, ".") == 0) || (strcmp(currentFile->d_name, "..") == 0) )
        		continue;
		  
		   //chdir(temp);
		  if(DT_DIR==currentFile->d_type){
			  printf("girdim \n");			 
			  strcat(curDirectories[0],"/");
			  strcat(curDirectories[0],currentFile->d_name);
			  //printf("new: %s \n",curDirectories[0]);
			  //printf("There is a directory \n");
			 
			  strcat(curDirectories[1],"/");
			  strcat(curDirectories[1],currentFile->d_name);			 			
			  mkdir(curDirectories[1], S_IRUSR | S_IWUSR | S_IXUSR);
			 
			  
			  producer(curDirectories);
		         
			  strcpy(curDirectories[0],realSource);
			  strcpy(curDirectories[1],realDest);
		  }
		  else{
			   
			  //printf("FILE NAME: %s \n",currentFile->d_name);	
			  sprintf(filePathSource, "%s/%s", curDirectories[0] , currentFile->d_name);
			  sprintf(filePathDest, "%s/%s", curDirectories[1] , currentFile->d_name);
			  printf("filePathSource: %s \n",filePathSource);
			  if((source.infd=open(filePathSource,R_FLAGS))==-1){
				fprintf(stderr,"Source file can not be open for read \n");
				exit(EXIT_FAILURE);
			  }
			  if((source.outfd=open(filePathDest, W_FLAGS, W_PERMS))==-1){
				fprintf(stderr,"Source file can not be open for wright \n");
				exit(EXIT_FAILURE);
			  }
			 
			  printf("%s open for reading and writing \n",currentFile->d_name);
			  strcpy(source.filename,filePathSource);
			  //printf("in fd: %d \n",source.infd);
			  //printf("out fd: %d \n",source.outfd);
			  //printf("filepath: %s \n",source.filename);
		  }
		  
		
              	 
		  
  	}
	
	return NULL; 
}
