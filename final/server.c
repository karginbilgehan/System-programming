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

#define W_FLAGS (O_WRONLY | O_CREAT) 
#define W_PERMS (S_IRUSR | S_IWUSR)
#define FILE_LEN 128
#define PATH_LEN 256
#define CONTENT_LEN 8192


char clientDirectory[256];//client'ın saklamak istedigi directory icin
char buffer[1024];
char curServerDir[PATH_LEN];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int counter=0;
static char clientDirName[128]={0};

typedef struct {   
	char file_content[CONTENT_LEN];   
	char filename[FILE_LEN]; 
} fileInf;

fileInf fileInformation;//client tarafından gelen file bilgilerini almak için


void parseDirectoryName(char* fullPath);

void * socketThread(void *arg)
{
  
  //printf("girdim \n");
  int newSocket = *((int *)arg);
  int i;
  int outFd,wc;//wc means word count
  DIR *directory;
  char curDirPath[PATH_LEN];//kopyalama yapılacak directoryi tutmak için
  char copiedFile[PATH_LEN];
  struct dirent *currentFile;
  
  recv(newSocket , clientDirectory , sizeof(clientDirectory) , 0);
  // Send message to the client socket 
  pthread_mutex_lock(&lock);
  char *message = malloc(sizeof(clientDirectory)+20);
  strcpy(message,"Client Directory Path: ");
  strcat(message,clientDirectory);
  strcat(message,"\n");
  strcpy(buffer,message);
  free(message);
  printf("%s \n",buffer);
  //printf("len: %d \n",strlen(buffer));
  getcwd(curServerDir,sizeof(curServerDir));
  printf("current: %s \n",curServerDir);
  
  parseDirectoryName(buffer);
  printf("client directorty: %s \n",clientDirName);
  directory=opendir(clientDirName);
  if(directory==NULL){
	if(mkdir(clientDirName, S_IRUSR | S_IWUSR | S_IXUSR)==-1)
		fprintf(stderr,"Directory can not create successfully \n");
		
  }
  sprintf(curDirPath,"%s/%s",curServerDir,clientDirName);//Server Directory içine clientdan gelen directoryi olusturduktan sonra client directorysi içinde işlem yapabilmek için
  printf("kopyalama yapilacak yer: %s \n",curDirPath);   
  recv(newSocket , fileInformation.filename , sizeof(fileInformation.filename) , 0);
  recv(newSocket , fileInformation.file_content , sizeof(fileInformation.file_content) , 0);
  //printf("file name: %s \n",fileInformation.filename);
  //printf("file Content: %s \n",fileInformation.file_content);
  sprintf(copiedFile,"%s/%s",curDirPath,fileInformation.filename);
  if((outFd=open(copiedFile, W_FLAGS, W_PERMS))==-1)
	fprintf(stderr,"File can not open for write");
  printf("out: %d \n",outFd);
  if((wc=write(outFd,fileInformation.file_content,strlen(fileInformation.file_content)))==-1)
		fprintf(stderr,"There is an error for write \n"); 
  //printf("wc: %d \n",wc);
  counter=0; //counter degerini sifirlamak icin. Clientin dosya adini bulmada kullaniyoruz.(parseDirectoryName Fonk.)
  for(i=0;i<sizeof(clientDirectory);++i){
	clientDirectory[i]='\0';
  }
  for(i=0;i<sizeof(clientDirName);++i){
	clientDirName[i]='\0';
  }
  pthread_mutex_unlock(&lock);
  close(newSocket);
  printf("Exit socketThread \n");
  pthread_exit(NULL);
}
int main(int argc, char* argv[]){
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  char serverName[1024];
  sprintf(serverName,"%s/%s","/home/cse312/Desktop",argv[1]); // BU KISIM DEGİSMELİ 
  printf("serverName: %s \n",serverName);
  if(chdir(serverName)<0)
	printf("Current Directory can not change successfully \n"); // server klasörü altında gerekli işlemleri yapmak için
  
  //Create the socket. 
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  // Configure settings of the server address struct
  // Address family = Internet 
  serverAddr.sin_family = AF_INET;
  //Set port number, using htons function to use proper byte order 
  serverAddr.sin_port = htons(8080);
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
    pthread_t tid[60];
    int i = 0;
    //printf("deneme \n");
    while(1)
    {
	//printf("deneme2 \n");
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        //for each client request creates a thread and assign the client request to it to process
       //so the main thread can entertain next request
	//printf("deneme3 \n");
        if( pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0 )
           printf("Failed to create thread\n");
	//printf("deneme4 \n");
        if( i >= 50)
        {
          i = 0;
          while(i < 50)
          {
            pthread_join(tid[i++],NULL);
          }
          i = 0;
        }
    }
  return 0;
}

void parseDirectoryName(char* fullPath){
	int size,i,j=0;

	
	size=strlen(fullPath);
	for(i=size-1;i>=0;--i){
		if(fullPath[i]=='/')
			break;
		counter ++;
	}
	//printf("counter %d \n",counter);
        for(i=size-counter;i<size-1;++i){
		clientDirName[j]=fullPath[i];
		++j;
	}
	//clientDirName[j]='\0';
	
}
