#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h> 
#include <sys/types.h>      

int main (int argc,char*argv[])
{	
	clock_t  start,end;
	pid_t getpid;
	double cpu_time_used;
	int status=1;
  	start = clock();
	int time=atoi(argv[1]);
	sem_t * sem_id;
    	sem_id=sem_open("mysem", O_CREAT, 0600, 0);
	while(status){
		end = clock();	
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;			
		if(cpu_time_used>=time){
			printf("program has done %lf seconds \n",cpu_time_used);
			//kill(0,SIGKILL);
			getpid=fork();
			if(getpid==0){
				printf("Child process has been created \n");
				sem_post(sem_id);
				//for(;;);
			}
			//wait(NULL);
			else{
				sem_wait(sem_id);
				kill(0,SIGKILL);				
				exit(EXIT_FAILURE);
			}			
			
		}
	}
}

