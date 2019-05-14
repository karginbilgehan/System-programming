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
#define FILE_LEN 128
#define PATH_LEN 256
#define CONTENT_LEN 8192
#define R_FLAGS O_RDONLY 

typedef struct {   
	char file_content[CONTENT_LEN];   
	char filename[FILE_LEN]; 
} fileInf;

void socketOperation(char* firstDirName);
void fileOperation(char* dirName,int socket);

fileInf fileInformation;

int main(int argc, char* argv[]){

  socketOperation(argv[1]);  
   // !feof(fp)
 
  return 0;
}
// client çalştırıldığında gelen directory'e ait işlemleri yapmak için gerekli başlangıç fonksiyonu
// socket e clientdan gelen ilk directory ismini gonderir.
void socketOperation(char* firstDirName){

	//printf("In thread\n");
  //printf("client deneme2 \n");
  char dirName[256];//gönderilecek olan directorynin ismi
  char buffer[1024];
  int clientSocket;
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
        int i=0;
	FILE *filePointer;
    	char ch;
        while((currentFile = readdir(directory)) != NULL ){
		printf("girdim \n");
		if ( (strcmp(currentFile->d_name, ".") == 0) || (strcmp(currentFile->d_name, "..") == 0) )
        		continue;
                 
                //dosya için
     
                sprintf(clientFilePath, "%s/%s", dirName , currentFile->d_name);
                //printf("fileName: %s \n",clientFilePath);
		filePointer=fopen(clientFilePath,"r");
		if(filePointer==NULL){
			fprintf(stderr,"File can not be open \n");
		}
               
		while((ch=getc(filePointer))!=EOF){
			fileInformation.file_content[i]=ch;
			++i;
		}
		sprintf(fileInformation.filename,"%s",currentFile->d_name);
		send(socket,fileInformation.filename,sizeof(fileInformation.filename),0);
		send(socket,fileInformation.file_content,sizeof(fileInformation.file_content),0);

		
		//printf("file name: %s \n",fileInformation.filename);
		//printf("file Content: %s \n",fileInformation.file_content);
		
        }
}
