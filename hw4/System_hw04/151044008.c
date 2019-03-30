#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include "151044008.h"

int ARG_CONTROL=1;//it is necessary to control -z argument.

int main(int argc, char *argv[])
{
   pid_t pid,childPid;
   int fd;	
   char toFile[1024];
   //struct flock lock;

   if(argc==3){
	pathControl(argv[2]);//control for file that cannot be open or can be open.
   }
   else if(argc==2){
   	pathControl(argv[1]);
   }
   
   remove("151044008sizes.txt");//to clear size file
   childPid=fork();
   if(childPid<0){
	printf("Process can not created. \n");
   }
   else if(childPid==0){//child process
		if (argc==3 && (strcmp(argv[1],"-z")==0)){//it is for using with -z argument
			ARG_CONTROL=2;
			int total_size=postOrderApply(argv[2],sizepathfun);
			if(total_size!=-1){
				total_size=total_size/1024;
				childPid=getpid();		
				sprintf(toFile,"%d \t \t %d \t \t %s \n",childPid,total_size,argv[2]);
				int temp_size=strlen(toFile);
				fd=open("151044008sizes.txt",O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
				//memset (&lock, 0, sizeof(lock));//start file locking 
		 		//lock.l_type = F_WRLCK;
		 		//fcntl (fd, F_SETLKW, &lock);
				write(fd,toFile,temp_size);
				//lock.l_type = F_UNLCK;
		 		//fcntl (fd, F_SETLKW, &lock);//end file locking 
				close(fd);
			}
			   	
		}
    		else if(argc==2) // it is for using without -z argument. Only two parameters
    		{
			int total_size=postOrderApply(argv[1],sizepathfun);
			
		    	if(total_size!=-1){
				total_size=total_size/1024;
				childPid=getpid();
				sprintf(toFile,"%d \t \t %d \t \t %s \n",childPid,total_size,argv[1]);
				int temp_size=strlen(toFile);
				fd=open("151044008sizes.txt",O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
				//memset (&lock, 0, sizeof(lock));//start file locking 
		 		//lock.l_type = F_WRLCK;
		 		//fcntl (fd, F_SETLKW, &lock);
				write(fd,toFile,temp_size);
				//lock.l_type = F_UNLCK;
		 		//fcntl (fd, F_SETLKW, &lock);//end file locking 
				close(fd);
		
			} 
			
    		}	
   		_exit(0);
		
   }
   
   else{//parent process
		waitpid(childPid,NULL,0);
		if(argc==3 && (strcmp(argv[1],"-z")==0)){
			pid=getpid();
			//This block read the file and print the output on the screen
			char *buffer;
			int fileSize=findTotalSize();
			buffer = (char *)malloc(fileSize*sizeof(char));
			fd=open("151044008sizes.txt",O_CREAT | O_APPEND | O_RDONLY, S_IRWXU);
			read(fd,buffer,fileSize);
			close(fd);
			int tp=totalProcess(buffer);
			printf("%s",buffer);
			printf("%d child processes created. Main process is %d. \n",tp,pid);
			
			
		}
		else if(argc==2){
			pid=getpid();
			//This block read the file and print the output on the screen
			char *buffer;
			int fileSize=findTotalSize();
			buffer = (char *)malloc(fileSize*sizeof(char));
			fd=open("151044008sizes.txt",O_CREAT | O_APPEND | O_RDONLY, S_IRWXU);
			read(fd,buffer,fileSize);
			close(fd);
			int tp=totalProcess(buffer);
			printf("%s",buffer);
			printf("%d child processes created. Main process is %d. \n",tp,pid);
			
			
		}
		else{// This block is for error situation.
	    		printf("This usage is wrong. There must be 2 or 3 argument. \n");
			printf("Usage: ./buNeDu -z path_name or ./buNeDu path_name \n");
    		}
   	
		
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
 //FILE *fp;
 pid_t childPid,id;
 int fds[2];
 char toFile[1024]; 
 
 //struct flock lock;
  
  while((currentDir = readdir(directory)) != NULL ){


    if ( (strcmp(currentDir->d_name, ".") == 0) || (strcmp(currentDir->d_name, "..") == 0) ) // checks the status of being itself or its up folder
        continue;
    
    sprintf(filePath, "%s/%s", path , currentDir->d_name); // it is to specifiy the path name
    size=pathfun(filePath);
    if(size==-1){// it is to control special file.
	int fd;	
	char toFile[1024];
	id=getpid();
	sprintf(toFile,"%d \t \t %s \t \t Special file %s \n",id,"-1",currentDir->d_name);
	int temp_size=strlen(toFile);
		
	fd=open("151044008sizes.txt",O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
	//memset (&lock, 0, sizeof(lock));//start file locking 
 	//lock.l_type = F_WRLCK;
 	//fcntl (fd, F_SETLKW, &lock);	
	write(fd,toFile,temp_size);
	//lock.l_type = F_UNLCK;
 	//fcntl (fd, F_SETLKW, &lock);//end file locking 
	close(fd);	
	size=0;
    }
    
    if (DT_DIR==currentDir->d_type && ARG_CONTROL==2) // it is to control using with 3 argument like ./buNeDu -z A.
    {
	int fd;
	//FILE *fp;
	
	pipe (fds);
	childPid=fork();
	if(childPid==0){//child
		int directory_size=postOrderApply(filePath,pathfun);
		if(directory_size!=-1 || directory_size!=-2){
			
			/*fp=fopen("directorySize.txt", "a+");//it is to find total size for every directory
			if (fp == NULL)
  			{
       			        printf( "Could not open file. \n" ) ;
        		}
			
			fprintf(fp,"%d",directory_size);//print the size of directory to communicate with other process
			
			fclose(fp);*/
			close (fds[0]); //Okuma kısmı kapatıldı
			write(fds[1],&directory_size,sizeof(directory_size));
			close(fds[1]);
			id=getpid();
			sprintf(toFile,"%d \t \t %d \t \t %s \n",id,directory_size/1024,filePath);
			int size=strlen(toFile);	
			fd=open("151044008sizes.txt",O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
			//memset (&lock, 0, sizeof(lock));//start file locking 
 			//lock.l_type = F_WRLCK;
 			//fcntl (fd, F_SETLKW, &lock);
			write(fd,toFile,size);
			//lock.l_type = F_UNLCK;
 			//fcntl (fd, F_SETLKW, &lock);//end file locking
			close(fd);
				
		}
		_exit(0);	
	}
	else{// parent
		/*waitpid(childPid,NULL,0);
		int dir_size;
		fp=fopen("directorySize.txt", "a+");//it is to find total size for every directory
			if ( fp == NULL )
  			{
       			        printf( "Could not open file." ) ;
        			
    			}
		fscanf(fp,"%d",&dir_size);//specify the directory size 
		fclose(fp);
		remove("directorySize.txt");*/
		int dir_size;
		close(fds[1]);
		read(fds[0],&dir_size,sizeof(dir_size));
		close(fds[0]);
		total_size=total_size+dir_size;//total size for directory
		
		
	}
		
	
    }
    else if(DT_DIR==currentDir->d_type && ARG_CONTROL==1){ // it is to control using with 2 argument like ./buNeDu A.
	int fd;
	//char toFile[1024];
	
	childPid=fork();	  	
	if(childPid==0){
		int directory_size=postOrderApply(filePath,pathfun);
        	if(directory_size!=-1 || directory_size!=-2){
			id=getpid();
			sprintf(toFile,"%d \t \t %d \t \t %s \n",id,directory_size/1024,filePath);
			int size=strlen(toFile);
			fd=open("151044008sizes.txt",O_CREAT | O_APPEND | O_WRONLY, S_IRWXU);
			//memset (&lock, 0, sizeof(lock));//start file locking
 			//lock.l_type = F_WRLCK;
 			//fcntl (fd, F_SETLKW, &lock);
			write(fd,toFile,size);
			//lock.l_type = F_UNLCK;
 			//fcntl (fd, F_SETLKW, &lock);//end file locking
			close(fd);	
		}
		_exit(0);
	}	
	
	else{
		//waitpid(childPid,NULL,0);
		total_size=total_size;
	} 
		
    }
    
    else{//it is for total size
        total_size=size+total_size;
    }
    
  }
    while(wait(NULL)>0){
	    
	    //waitpid(childPid,NULL,0);
	    
	    /*fp=fopen("directorySize.txt", "a+");//it is to find total size for every directory
	    if ( fp == NULL ){
	       	 printf( "Could not open file." ) ;
					
	    }
	    fscanf(fp,"%d",&dir_size);//specify the directory size 
            close(fp);
	    remove("directorySize.txt");*/
	    	
	    	
    }
    closedir(directory);
    return total_size;
}

//This fuction find the total processes and return this value.
//parse the file to find total proces number. 
int totalProcess(char * buff){
	
	int i,j;
	i = j = 0;
	int pid,pid2;
	int totalP,control;
	totalP=control=0;
	while(buff[i]!='\0'){
		if(i==0){
			sscanf(&buff[i],"%d",&pid);
			totalP++;		
		}
		if(buff[i-1]=='\n'){
			sscanf(&buff[i],"%d",&pid);
			while(j<i-1){
				if(j==0){
					sscanf(&buff[j],"%d",&pid2);
					if(pid==pid2){//compare two pid, if two pid are the same that means this process predefined.
						control=1;					
					}
				}
				if(buff[j-1]=='\n'){
					sscanf(&buff[j],"%d",&pid2);
					
					if(pid==pid2){//compare two pid, if two pid are the same that means this process predefined.
						control=1;					
					}
				}
				j++;
			}
			j=0;
			if(control==0){
				totalP++;
			}
			else{
				control=0;
			}
				
		}
		++i;	
	}
	
	return totalP;

}

//This fuction find total character in file and return this value.
//This size is necessary to read file.
int findTotalSize(){

	int totalChar=0;
	FILE *file;
	file = fopen("151044008sizes.txt", "r");
	while(fgetc(file)!= EOF){
		totalChar++;		
	}
	
	fclose(file);
	return totalChar;

}
//This function check that file cannot be open or can be open. 
void pathControl(char* pathName){
	
	DIR *directory=opendir(pathName);
	if (directory == NULL){
		printf("%s: This directory can not be open.\n",pathName);       
		exit(1);
  	}
	closedir(directory);
	
}	
