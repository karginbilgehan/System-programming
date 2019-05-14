#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include<pthread.h>
char client_message[2000];
char buffer[1024];
char temp[100];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int counter=0;
static char clientDirName[128]={0};
char* parseDirectoryName(char* fullPath);

void * socketThread(void *arg)
{
  
  //printf("girdim \n");
  int newSocket = *((int *)arg);
  int i;
  recv(newSocket , client_message , 2000 , 0);
  // Send message to the client socket 
  pthread_mutex_lock(&lock);
  char *message = malloc(sizeof(client_message)+20);
  strcpy(message,"Client Directory Path: ");
  strcat(message,client_message);
  strcat(message,"\n");
  strcpy(buffer,message);
  free(message);
  printf("%s \n",buffer);
  //printf("len: %d \n",strlen(buffer));
  getcwd(temp,sizeof(temp));
  printf("current: %s \n",temp);
  parseDirectoryName(buffer);
  printf("Exit socketThread \n");
  pthread_mutex_unlock(&lock);
  close(newSocket); 
  counter=0;
 
  for(i=0;i<sizeof(client_message);++i){
	client_message[i]='\0';
  }
  for(i=0;i<sizeof(clientDirName);++i){
	clientDirName[i]='\0';
  }
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
	printf("Current Directory has not changed successfully \n"); // server klasörü altında gerekli işlemleri yapmak için
  
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

char* parseDirectoryName(char* fullPath){
	int size,i,j=0;

	
	size=strlen(fullPath);
	for(i=size-1;i>=0;--i){
		if(fullPath[i]=='/')
			break;
		counter ++;
	}
	//printf("counter %d \n",counter);
        for(i=size-counter;i<size;++i){
		clientDirName[j]=fullPath[i];
		++j;
	}
	printf("client directorty:%s \n",clientDirName);
	return clientDirName;
}
