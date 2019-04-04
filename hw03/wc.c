#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
//#include "wc.h"


int main (int argc, char*argv[]){
	int counter=0;
	char letter;
		printf("argc: %d \n",argc);	
		if(argc==1){
			printf("argc %d \n",argc);	
			while(scanf("%c",&letter)!=EOF){
				if(letter == '\n'){
					counter++;
				}
			}
			printf("%d \n",counter);
		}
		else{
			FILE *fp;
			char text[1024];
			fp = fopen(argv[1],"r");
			while(fgets(text,1024,fp)!='\0')
			{
				counter++;
			}
					     
			fclose(fp);
			printf("%d \n",counter);	
	
		}
		
	return 0;
}	



