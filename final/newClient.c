#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#define FILE_LEN 128
#define PATH_LEN 256
#define CONTENT_LEN 100000
#define R_FLAGS O_RDONLY 

typedef struct {   
	char file_content[CONTENT_LEN];   
	char filename[FILE_LEN];
	//int file_counter;
	int mode;//gonderilen yapının bir klasör mü yoksa dosya mı olduğunu anlamak için 0 for file 1 for directory
	int rec_mode;
} fileInf;
typedef struct {
	char modifiedTime[PATH_LEN];
	char filePath[PATH_LEN];
} modifiedOp;



void socketOperation(char* firstDirName);
void fileOperation(char* dirName,int socket);
int modified(char *dirName);
void findModifiedFile();

int clientSocket;
fileInf fileInformation;
modifiedOp modifiedOperation[512];
modifiedOp modifiedOperationForFunction[512];
struct stat file;
int structSize=0;
int indexF=0;


int main(int argc, char* argv[]){
  //fileInformation.file_counter=0;
  int i=0;
  
  socketOperation(argv[1]);
  indexF=0;
  
  while(i<10000000){
	modified(argv[1]);
  	indexF=0;
	//i++;
	//sleep(15);
	findModifiedFile();
  	indexF=0;
	i++;
  }
  
  
 
 
  /*for(i=0;i<structSize;++i){
	printf("file path for modified file for func:  %s \n",modifiedOperationForFunction[i].filePath);
  }

  for(i=0;i<structSize;++i){
	printf("file path for modified:  %s \n",modifiedOperation[i].filePath);
  }*/
 
    

 
  return 0;
}
// client çalştırıldığında gelen directory'e ait işlemleri yapmak için gerekli başlangıç fonksiyonu
// socket e clientdan gelen ilk directory ismini gonderir.
void socketOperation(char* firstDirName){

	//printf("In thread\n");
  //printf("client deneme2 \n");
  char dirName[256];//gönderilecek olan directorynin ismi
  char buffer[1024];
  char synch[5]="synch";
  struct sockaddr_in serverAddr;
  socklen_t addr_size;
  // Create the socket. 
  
    clientSocket = socket(PF_INET, SOCK_STREAM, 0);
    
  //Configure settings of the server address
 // Address family is Internet 
    serverAddr.sin_family = AF_INET;
  //Set port number, using htons function 
    serverAddr.sin_port = htons(8080);
 //Set IP address to localhost
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    //Connect the socket to the server using the address
    addr_size = sizeof serverAddr;
   
    if(connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size)==0)
		printf("connected \n");
    //printf("client deneme \n");
    sprintf(dirName,"%s",firstDirName);//firstDirName argumandan girilen full pathi belirtir
    //printf("client deneme 7 \n");
    if( send(clientSocket , dirName , sizeof(dirName) , 0) < 0)
    {
            printf("Send failed\n");
    }
    recv(clientSocket,synch,sizeof(synch),0);
    fileOperation(dirName,clientSocket);
    //printf("client deneme 8 \n");
    //Read the message from the server into the buffer
    /*if(recv(clientSocket, buffer, 1024, 0) < 0)
    {
       printf("Receive failed\n");
    }*/
    
    //Print the received message
    //printf("Data received: %s\n",buffer);
    close(clientSocket);
}
//directory içindeki filelara ait contenti ve filenami struct yardımıyla servera gonderir
void fileOperation(char* dirName,int socket){
        DIR *directory=opendir(dirName);
	struct dirent *currentFile;
	char clientFilePath[PATH_LEN];
	char innerDirectoryPath[PATH_LEN];
	char synch[5]="synch";
        int i=0;
	
	FILE *filePointer;
    	char ch;
	//printf("girdim 3 \n");
	if(directory==NULL){
		printf("directory is null \n");
	}
        while((currentFile = readdir(directory)) != NULL ){
		fileInformation.rec_mode=0;
		if ( (strcmp(currentFile->d_name, ".") == 0) || (strcmp(currentFile->d_name, "..") == 0) )
        		continue;
                 
                //dizin için
		if(DT_DIR==currentFile->d_type){
			fileInformation.mode=1;
			sprintf(innerDirectoryPath,"%s/%s",dirName,currentFile->d_name);
			printf("directory path: %s \n",innerDirectoryPath);
			sprintf(fileInformation.filename,"%s",currentFile->d_name);
			sprintf(fileInformation.file_content,"%s","redundant");
			//printf("client file content: %s \n",fileInformation.file_content);
			send(socket,&fileInformation.rec_mode,sizeof(fileInformation.rec_mode),0);
			recv(socket , synch , sizeof(synch) , 0);
			send(socket,fileInformation.filename,sizeof(fileInformation.filename),0);
			recv(socket , synch , sizeof(synch) , 0);
			send(socket,fileInformation.file_content,sizeof(fileInformation.file_content),0);
			recv(socket , synch , sizeof(synch) , 0);
			send(socket,&fileInformation.mode,sizeof(fileInformation.mode),0);
			recv(socket , synch , sizeof(synch) , 0);			
			fileOperation(innerDirectoryPath,clientSocket);
			fileInformation.rec_mode=1;
  			send(socket,&fileInformation.rec_mode,sizeof(fileInformation.rec_mode),0);
			recv(socket , synch , sizeof(synch) , 0);
		}
     		else{
			printf("dosya icin geldim \n");
			sprintf(clientFilePath, "%s/%s", dirName , currentFile->d_name);
		        printf("ClientfilePath: %s \n",clientFilePath);
			
			stat(clientFilePath,&file);
   			sprintf(modifiedOperation[indexF].filePath,"%s",clientFilePath);
			sprintf(modifiedOperation[indexF].modifiedTime,"%s",ctime(&file.st_mtime));
			
			filePointer=fopen(clientFilePath,"r");
			if(filePointer==NULL){
				fprintf(stderr,"File can not be open \n");
			}
		       
			while((ch=getc(filePointer))!=EOF){
				fileInformation.file_content[i]=ch;
				++i;
				if(i==CONTENT_LEN-1){
					break;
				}
			}
			fileInformation.file_content[i]='\0';
	     		i=0;
			sprintf(fileInformation.filename,"%s",currentFile->d_name);
			
			printf("file name: %s \n",fileInformation.filename);
			//printf("file Content: %s \n",fileInformation.file_content);
			fileInformation.mode=0;
			
			send(socket,&fileInformation.rec_mode,sizeof(fileInformation.rec_mode),0);
			recv(socket , synch , sizeof(synch) , 0);
			send(socket,fileInformation.filename,sizeof(fileInformation.filename),0);
			recv(socket , synch , sizeof(synch) , 0);
			send(socket,fileInformation.file_content,sizeof(fileInformation.file_content),0);
			recv(socket , synch , sizeof(synch) , 0);
			send(socket,&fileInformation.mode,sizeof(fileInformation.mode),0);
			recv(socket , synch , sizeof(synch) , 0);
			structSize++;
			indexF++;
		
		}
                
     		
		
	
        }
	//fileInformation.file_counter=1;
	//send(socket,&fileInformation.file_counter,sizeof(fileInformation.file_counter),0);
	
}
int modified(char *dirName){
	//printf("dirName %s \n",dirName);
	DIR *directory=opendir(dirName);
	struct dirent *currentFile;
	char clientFilePath[PATH_LEN];
	char innerDirectoryPath[PATH_LEN];
	
	
	if(directory==NULL){
		printf("directory is null2 \n");
	}
        while((currentFile = readdir(directory)) != NULL ){
		
		if ( (strcmp(currentFile->d_name, ".") == 0) || (strcmp(currentFile->d_name, "..") == 0) )
        		continue;
                 
                //dizin için
		if(DT_DIR==currentFile->d_type){
			
			sprintf(innerDirectoryPath,"%s/%s",dirName,currentFile->d_name);
			//printf("modified directory path: %s \n ",innerDirectoryPath);
			sprintf(fileInformation.filename,"%s",currentFile->d_name);
			sprintf(fileInformation.file_content,"%s","redundant");
			//printf("client file content: %s \n",fileInformation.file_content);
			modified(innerDirectoryPath);
			
			
		}
     		else{
			//printf("dosya icin geldim \n");
			//printf("girdim \n");
			sprintf(clientFilePath, "%s/%s", dirName , currentFile->d_name);
		        //printf("modified ClientfilePath: %s \n",clientFilePath);
			
			stat(clientFilePath,&file);
			//printf("modified ClientfilePath: %s \n",clientFilePath);
			//printf("INDEX F :%d \n \n",indexF);
   			sprintf(modifiedOperationForFunction[indexF].filePath,"%s",clientFilePath);
			sprintf(modifiedOperationForFunction[indexF].modifiedTime,"%s",ctime(&file.st_mtime));
			indexF++;	
			
			//printf("file Content: %s \n",fileInformation.file_content);
			
		}
		
        }
	
	return 0;	
}

void findModifiedFile(){

	int i=0;
	int k=0;
	//printf("GELDIM \n");
	//printf("struct size %d \n",structSize);
	while(k<structSize){
			//printf("girdim2 \n");
			while(i<structSize){
				
				if(strcmp(modifiedOperation[k].filePath,modifiedOperationForFunction[i].filePath)==0){
					//printf("\n IKI ESIT DOSYA BULDUM \n ");
					if(strcmp(modifiedOperation[k].modifiedTime,modifiedOperationForFunction[i].modifiedTime)!=0){
						printf("DOSYA DEGISTI \n");					
					}
					
				}
				++i;
			}
		i=0;
		++k;
	}
}
