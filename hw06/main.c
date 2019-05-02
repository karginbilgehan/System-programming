#include <pthread.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#define PATH_LEN 256 
void* producer (void* unused);
typedef struct {   
	int infd;   
	//int outfd;   
	char filename[PATH_LEN]; 
} buffer_t;

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
	char filePath[PATH_LEN];
	
	strcpy(curDirectories[0],((char(*)[PATH_LEN])unused)[0]); // it is for source directory
	strcpy(curDirectories[1],((char(*)[PATH_LEN])unused)[1]); // it is for destination directory
	
	DIR *directory=opendir(curDirectories[0]);
 	struct dirent *currentFile;
	
	while((currentFile = readdir(directory)) != NULL ){
		  if ( (strcmp(currentFile->d_name, ".") == 0) || (strcmp(currentFile->d_name, "..") == 0) )
        		continue;			
		  sprintf(filePath, "%s/%s", curDirectories[0] , currentFile->d_name);
		  printf("filePath: %s \n",filePath);
	}
	//printf("first :%s \n",curDirectories[0]);
	//printf("second :%s \n",curDirectories[1]);
  	//printf("\n");
		  
	return (void*) 10; 
}
