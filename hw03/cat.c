#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

void cat(char* fileName,int fileSize);

int main (int argc, char*argv[]){
		char letter;
		printf("argc: %d \n",argc);	
		if(argc==1){
			printf("argc %d \n",argc);	
			while(scanf("%c",&letter)!=EOF){
				printf("%c",letter);
			}
			
		}
		else{
		       FILE *fp;
		       char text[1024];
		       fp = fopen(argv[1],"r");
		       while(fscanf(fp,"%[^\n]\n",text)!=EOF)
		       {
			     fprintf(stdout,"%s\n",text);
		       }
			     
		       fclose(fp);
	
		}
}


