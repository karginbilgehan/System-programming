#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <string.h> 
#include    <unistd.h>
#include    <time.h>
#include    "bank_client.h"


void timer_handler(union sigval sv);
void start_timer(int bankTime);

timer_t timerid;
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
	umask(0); /* So we get the permissions we want */
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
			close(fd[totalChild][1]); // Close writing end of second pipe 
		  
			// Read string from child, print it and close 
			// reading end. 
			
			while(read(fd[totalChild][0], &tempPid, sizeof(req))<=0);
			printf("temp pid : %d \n",tempPid);
			//!!! bu kısımdan altı child process içinde olacak.
			/* Open client FIFO (previously created by client) */
			close(fd[totalChild][0]);
			snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long)tempPid);
			printf("clientFifo: %s \n",clientFifo);
			clientFd = open(clientFifo, O_WRONLY);
			if (clientFd == -1) { /* Open failed, give up on client */
				perror("open3");
				continue;
			}
			/* Send response and close FIFO */
			randMoney=rand();
			randMoney=randMoney * 55251;
			randMoney=randMoney % tempPid;
			randMoney=randMoney % 100;
			randMoney+=100;
			resp.money = randMoney;
			if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
				fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
			if (close(clientFd) == -1)
				perror("close");
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
			
			
			if(close(fd[pid][0])==-1)
				perror("close read side");  
			if(write(fd[pid][1], &req.pid, sizeof(req))==-1)
				perror("write"); 
			if(close(fd[pid][1])==-1)
				perror("close write side");
			
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

//child oldugunda direk exit yaparsan childlar orphan durumuna duser. Bunu kontrol et.
void timer_handler(union sigval sv) {
	printf("Bank has been closed. \n");
        /* Will print "5 seconds elapsed." */
	if(timer_delete(timerid)!=0){
		perror("Timer can not destroy successfully");
	}
	kill(0,SIGKILL);
	exit(EXIT_SUCCESS);
}
