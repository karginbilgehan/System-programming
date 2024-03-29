#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h> 
#include    <unistd.h>
#include    <time.h>
#include    "bank_client.h"

#define FILE_NAME "log.txt"

void timer_handler(union sigval sv);
void start_timer(int bankTime);
void process_timer_handler(union sigval sv);
void timer_for_Process();
int money_creator(pid_t pid);

timer_t timerid; // it is for server time 
int process_timer=0; // it is for 1.5 second 
int fileDesc;
int totalTime;
int totalProcess=0;
int forChildProcess[4]={0};
int main(int argc, char *argv[]) {
	int serverFd, dummyFd, clientFd,i,totalChild;
	char clientFifo[CLIENT_FIFO_NAME_LEN];
	struct request req;
	struct response resp;
	int randMoney; /* This is our "service" */
	/* Create well-known FIFO, and open it for reading */
	int bankTime=atoi(argv[1]);
	pid_t p;
	int fd[4][2];
	start_timer(bankTime);
	char forHeader[]="clientPid         process No           para           İslemin Bitis Zamani";
	char forHeader2[]="---------         ----------           ----           --------------------";
 	umask(0); /* So we get the permissions we want */
	remove(FILE_NAME);
	totalTime=atoi(argv[1]);
	fileDesc=open(FILE_NAME, O_RDWR | O_CREAT, 0666);
	
	if (mkfifo(SERVER_FIFO, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
		perror("mkfifo");
	serverFd = open(SERVER_FIFO, O_RDONLY);
	if (serverFd == -1)
		perror("open1");
	/* Open an extra write descriptor, so that we never see EOF */
	dummyFd = open(SERVER_FIFO, O_WRONLY);
	if (dummyFd == -1)
		perror("open2");
	if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		perror("signal");
	
	if(write(fileDesc,forHeader,sizeof(forHeader)-1) < 0)
		perror("write log 1");
	if(write(fileDesc,"\n",1) < 0)
		perror("write log 2");
	if(write(fileDesc,forHeader2,sizeof(forHeader2)-1) < 0)
		perror("write log 3");
	if(write(fileDesc,"\n",1) < 0)
		perror("write log 4");
	for(i=0;i<4;++i)
	{
		if (pipe(fd[i])==-1) 
    			fprintf(stderr, "Pipe Failed" ); 
        }

	for (totalChild = 0; totalChild < 4; ++totalChild) {
		p = fork();
		if (p == 0)
			break;
	}
	
	
	
	if(p==0)
	{
	
		for(;;){
			
			pid_t tempPid;
			timer_for_Process();
			char logData[10]={0};
			int counter=0;
			
			//printf("cpu time used %lf \n",cpu_time_used);
			//close(fd[totalChild][1]); // Close writing end of second pipe 
		  
			// Read string from child, print it and close 
			// reading end. 
			//printf("process timer %d \n",process_timer);
			process_timer=0;
			while(process_timer==0);
			
			while(read(fd[totalChild][0], &tempPid, sizeof(req))<=0);
			//printf("temp pid : %d \n",tempPid);
			//!!! bu kısımdan altı child process içinde olacak.
			/* Open client FIFO (previously created by client) */
			//close(fd[totalChild][0]);
			snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)tempPid);
			//printf("clientFifo: %s \n",clientFifo);
			clientFd = open(clientFifo, O_WRONLY);
			if (clientFd == -1) { /* Open failed, give up on client */
				perror("open3");
				continue;
			}
			/* Send response and close FIFO */
			randMoney=money_creator(tempPid);
			//printf("rand Money: %d \n",ranMoney);
			resp.money = randMoney;
			if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
				fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
			if (close(clientFd) == -1)
				perror("close");
			
			int processNo=getpid();
			clock_t curTime=clock();
			
			sprintf(logData, "%d              %d                 %d              %lf%c%c", tempPid,processNo,randMoney,(double)curTime/CLOCKS_PER_SEC,'\n','\0');
			while(logData[counter]!='\0'){
				counter ++;
			}
			if(write(fileDesc,logData,counter) < 0)
				perror("write log 5");
			
		}	
		

	}
	if(p>0)
	{	
		
		
		for (;;) 
		{ /* Read requests and send responses */
			//printf("buraya girdim. \n");
			if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
				fprintf(stderr, "Error reading request; discarding\n");
				continue; /* Either partial read or error */
			}
			printf("req.pid : %d\n", req.pid);
			int pid=req.pid%4;
			forChildProcess[pid]+=1;
			totalProcess++;
			
			/*if(close(fd[pid][0])==-1)
				perror("close read side in parent process");*/  
			if(write(fd[pid][1], &req.pid, sizeof(req))==-1)
				perror("write parent process"); 
			
			/*if(close(fd[pid][1])==-1)
				perror("close write side in parent process");*/
			
			//seqNum += req.seqLen; /* Update our sequence number */
		}

	}
	
}

void start_timer(int bankTime){
	
	
	struct sigevent sev;
	struct itimerspec trigger;

	/* Set all `sev` and `trigger` memory to 0 */
	memset(&sev, 0, sizeof(struct sigevent));
	memset(&trigger, 0, sizeof(struct itimerspec));

	/* 
	 * Set the notification method as SIGEV_THREAD:
	 *
	 * Upon timer expiration, `sigev_notify_function` (thread_handler()),
	 * will be invoked as if it were the start function of a new thread.
	 *
	 */
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = &timer_handler;
	
	/* Create the timer. In this example, CLOCK_REALTIME is used as the
	 * clock, meaning that we're using a system-wide real-time clock for
	 * this timer.
	 */
	timer_create(CLOCK_REALTIME, &sev, &timerid);

	/* Timer expiration will occur withing 5 seconds after being armed
	 * by timer_settime().
	 */
	trigger.it_value.tv_sec = bankTime;

	/* Arm the timer. No flags are set and no old_value will be retrieved.
	 */
	timer_settime(timerid, 0, &trigger, NULL);
	
	/* Wait 10 seconds under the main thread. In 5 seconds (when the
	 * timer expires), a message will be printed to the standard output
	 * by the newly created notification thread.
	 */
	
	/* Delete (destroy) the timer */
	//timer_delete(timerid);
}

void timer_for_Process(){

	struct sigevent sev;
	struct itimerspec trigger;

	/* Set all `sev` and `trigger` memory to 0 */
	memset(&sev, 0, sizeof(struct sigevent));
	memset(&trigger, 0, sizeof(struct itimerspec));

	/* 
	 * Set the notification method as SIGEV_THREAD:
	 *
	 * Upon timer expiration, `sigev_notify_function` (thread_handler()),
	 * will be invoked as if it were the start function of a new thread.
	 *
	 */
	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_notify_function = &process_timer_handler;
	
	/* Create the timer. In this example, CLOCK_REALTIME is used as the
	 * clock, meaning that we're using a system-wide real-time clock for
	 * this timer.
	 */
	timer_create(CLOCK_REALTIME, &sev, &timerid);

	/* Timer expiration will occur withing 5 seconds after being armed
	 * by timer_settime().
	 */
	trigger.it_value.tv_sec = 1.5;

	/* Arm the timer. No flags are set and no old_value will be retrieved.
	 */
	timer_settime(timerid, 0, &trigger, NULL);
	
	/* Wait 10 seconds under the main thread. In 5 seconds (when the
	 * timer expires), a message will be printed to the standard output
	 * by the newly created notification thread.
	 */
	
	/* Delete (destroy) the timer */
	//timer_delete(timerid);



}
//child oldugunda direk exit yaparsan childlar orphan durumuna duser. Bunu kontrol et.
void timer_handler(union sigval sv) {
	//printf("Bank has been ");
        /* Will print "5 seconds elapsed." */
	char endPoint[100];
	char endPoint2[100];
	char endPoint3[100];
	char endPoint4[100];
	char endPoint5[100];
	int counter=0;
	int counter2=0;
	int counter3=0;
	int counter4=0;
	int counter5=0;
	if(timer_delete(timerid)!=0){
		perror("Timer can not destroy successfully");
	}
	
	sprintf(endPoint,"%d saniye dolmustur. %d musteriye hizmet verdik.%c%c",totalTime,totalProcess,'\n','\0');
	while(endPoint[counter]!='\0'){
		counter ++;
	}
	
	if(write(fileDesc,endPoint,counter) < 0)
		perror("write log 6");
	
	/////////////////////////////////////////////////////
	sprintf(endPoint2,"process %d %d musteriye hizmet sundu.\n%c",1,forChildProcess[0],'\0');
	
	while(endPoint2[counter2]!='\0'){
		counter2 ++;
	}
	if(write(fileDesc,endPoint2,counter2) < 0)
		perror("write log 7");
	
	
	/////////////////////////////////////////////////////
	sprintf(endPoint3,"process %d %d musteriye hizmet sundu.\n%c",2,forChildProcess[1],'\0');
	
	while(endPoint3[counter3]!='\0'){
		counter3 ++;
	}
	if(write(fileDesc,endPoint3,counter3) < 0)
		perror("write log 8");
		
	
	////////////////////////////////////////////////////	

	sprintf(endPoint4,"process %d %d musteriye hizmet sundu.\n%c",3,forChildProcess[2],'\0');
	while(endPoint4[counter4]!='\0'){
		counter4 ++;
	}
	if(write(fileDesc,endPoint4,counter4) < 0)
		perror("write log 9");	

	////////////////////////////////////////////////////

	sprintf(endPoint5,"process %d %d musteriye hizmet sundu.\n%c",4,forChildProcess[3],'\0');
	
	while(endPoint5[counter5]!='\0'){
		counter5 ++;
	}
	if(write(fileDesc,endPoint5,counter5) < 0)
		perror("write log 10");
	
	close(fileDesc);
	
	kill(0,SIGTERM);
	exit(EXIT_SUCCESS);
}

void process_timer_handler(union sigval sv) {
	process_timer=1;
}

int money_creator(pid_t pid){
	int money=rand();
	money*=54464;
	money%=pid;
	money%=100;	
	money+=100;
	money%=100;
	return money;
}
