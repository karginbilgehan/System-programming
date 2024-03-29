#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
#include <dirent.h>
#include <sys/stat.h>

#define W_FLAGS (O_WRONLY | O_CREAT) 
#define W_PERMS (S_IRUSR | S_IWUSR)
#define FILE_LEN 128
#define PATH_LEN 256
#define CONTENT_LEN 100000


char clientDirectory[256];
char buffer[1024];
char curServerDir[PATH_LEN];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int counter=0;
static char clientDirName[128]={0};
//char serverName[1024];

typedef struct {   
	char file_content[CONTENT_LEN];   
	char filename[FILE_LEN];
	//int  file_counter;
	int mode; 
	int rec_mode;
} fileInf;
typedef struct {
	int newSocket;
	char serverName[1024];

}mainSocket;

fileInf fileInformation;//client tarafından gelen file bilgilerini almak için
void parseDirectoryName(char* fullPath);
void receive();
void * socketThread(void *arg);

int main(int argc, char* argv[]){
  int serverSocket;
  mainSocket sock;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  sprintf(sock.serverName,"%s",argv[1]); 
  printf("serverName: %s \n",sock.serverName);
  printf("Server IP Address: 127.0.0.1 \n");
  //if(chdir(sock.serverName)<0)
	//printf("Current Directory can not change successfully \n"); 
  
  //Create the socket. 
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  // Configure settings of the server address struct
  // Address family = Internet 
  serverAddr.sin_family = AF_INET;
  //Set port number, using htons function to use proper byte order 
  serverAddr.sin_port = htons(atoi(argv[3]));
  //Set IP address to localhost 
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //Set all bits of the padding field to 0 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  //Bind the address struct to the socket 
  if(bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr))==0)
	printf("binded \n");
  //Listen on the socket, with 40 max connection requests queued 
  if(listen(serverSocket,50)==0)
    printf("Listening\n");
  else
    printf("Error\n");
    int threadSize=atoi(argv[2]);
    pthread_t tid[threadSize];
    int i = 0;
 
    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        sock.newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
	sprintf(sock.serverName,"%s",argv[1]);
        //for each client request creates a thread and assign the client request to it to process
       //so the main thread can entertain next request
	//printf("deneme3 \n");
        if( pthread_create(&tid[i], NULL, socketThread, &sock) != 0 )
           printf("Failed to create thread\n");
	//printf("deneme4 \n");
        if( i >= threadSize)
        {
          i = 0;
          while(i<threadSize)
          {
            pthread_join(tid[i++],NULL);
          }
          i = 0;
        }
    }
  return 0;
}

void * socketThread(void *arg){
  mainSocket new = *((mainSocket *)arg);
  char synch[5]="synch";
  int i,j=0;
  DIR *directory;
  char curDirPath[PATH_LEN];//kopyalama yapılacak directoryi tutmak için
  char realclientDirName[PATH_LEN];
  struct dirent *currentFile;
  pthread_mutex_lock(&lock);
  recv(new.newSocket , clientDirectory , sizeof(clientDirectory) , 0);
  //send(new.newSocket,synch,sizeof(synch),0);
  // Send message to the client socket 
  char *message = malloc(sizeof(clientDirectory)+20);
  strcpy(message,"Client Directory Path: ");
  strcat(message,clientDirectory);
  strcat(message,"\n");
  strcpy(buffer,message);
  free(message);
  printf("%s \n",buffer);
  //getcwd(curServerDir,sizeof(curServerDir));
  parseDirectoryName(buffer);
  sprintf(realclientDirName,"%s/%s",new.serverName,clientDirName);
  printf("client directory: %s \n",realclientDirName);
  directory=opendir(realclientDirName);
  if(directory==NULL){
	if(mkdir(realclientDirName, S_IRUSR | S_IWUSR | S_IXUSR)==-1)
		fprintf(stderr,"Directory can not create successfully \n");
		
  }
  sprintf(curDirPath,"%s/%s",curServerDir,clientDirName);
  receive(new,realclientDirName);
  for(i=0;i<sizeof(clientDirectory);++i){
	clientDirectory[i]='\0';
  }
  for(i=0;i<sizeof(clientDirName);++i){
	clientDirName[i]='\0';
  }
  pthread_mutex_unlock(&lock);
  close(new.newSocket);
  printf("End of the thread \n");
  pthread_exit(NULL);
}
void parseDirectoryName(char* fullPath){
	int size,i,j=0;
	size=strlen(fullPath);
	for(i=size-1;i>=0;--i){
		if(fullPath[i]=='/')
			break;
		counter ++;
	}
        for(i=size-counter;i<size-1;++i){
		clientDirName[j]=fullPath[i];
		++j;
	}
	//clientDirName[j]='\0';
}
void receive(mainSocket new,char *realPath){
          int outFd;
	  char copiedFile[PATH_LEN];
	  char copiedFilePath[PATH_LEN];
	  int wc;//wc means word count
	  int j=0;
	  char synch[5]="synch";
 	  DIR *directory;
	  while(j<20){
		recv(new.newSocket,&fileInformation.rec_mode,sizeof(fileInformation.rec_mode),0);
		//send(new.newSocket,synch,sizeof(synch),0);
		if(fileInformation.rec_mode==1){
			break;
		}
	  	recv(new.newSocket , fileInformation.filename , sizeof(fileInformation.filename) , 0);
		//send(new.newSocket,synch,sizeof(synch),0);
	  	recv(new.newSocket , fileInformation.file_content , sizeof(fileInformation.file_content) , 0);
		//send(new.newSocket,synch,sizeof(synch),0);
	  	recv(new.newSocket , &fileInformation.mode , sizeof(fileInformation.mode) , 0);
		//send(new.newSocket,synch,sizeof(synch),0);
  		sprintf(copiedFilePath,"%s/%s",realPath,fileInformation.filename);
		//printf("file name: %s \n",fileInformation.filename);
		//printf("file Content: %s \n",fileInformation.file_content);
		//printf("mode: %d \n",fileInformation.mode);
		//recv(newSocket , &fileInformation.file_counter , sizeof(fileInformation.file_counter) , 0);
		if(fileInformation.mode==0){
			  //printf("file name: %s \n",fileInformation.filename);
		  	  //printf("file Content: %s \n",fileInformation.file_content);
		 	  //printf("file counter: %d \n",fileInformation.file_counter);
		  	  sprintf(copiedFile,"%s/%s",new.serverName,fileInformation.filename);
		  	  printf("copiedFile: %s \n",copiedFilePath);
		  	  if((outFd=open(copiedFilePath,W_FLAGS , W_PERMS))==-1){
				fprintf(stderr,"File can not open for write \n");
			
		  	  }
			  //printf("file Content: %s \n",fileInformation.file_content);	
			  //if((wc=fwrite(fileInformation.file_content,1,strlen(fileInformation.file_content),outFd))==-1)
					//fprintf(stderr,"There is an error for write \n");
			  //fprintf(outFd,"%s",fileInformation.file_content);	
			  //printf("out: %d \n",outFd);
			  if((wc=write(outFd,fileInformation.file_content,strlen(fileInformation.file_content)))==-1)
					fprintf(stderr,"There is an error for write \n");
		}
	  	else {
			//printf("directory name: %s \n ",copiedFilePath);
			directory=opendir(copiedFilePath);
			if(directory==NULL){
				if(mkdir(copiedFilePath, S_IRUSR | S_IWUSR | S_IXUSR)==-1)
					fprintf(stderr,"Directory can not create successfully \n");
					receive(new,copiedFilePath);
	  		}
	     }
		j++;
	
	  }
	  
}
