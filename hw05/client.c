#include    <stdio.h>
#include    <stdlib.h>

#include    "bank_client.h"

static char clientFifo[CLIENT_FIFO_NAME_LEN];
static void /* Invoked on exit to delete client FIFO */

removeFifo(void) {
	unlink(clientFifo);
}

int main(int argc, char *argv[]) {
	int serverFd, clientFd;
	int i;
	pid_t p;
	struct request req;
	struct response resp;
	if (argc > 1 && strcmp(argv[1], "--help") == 0) 
		perror("[seq-len...]\n");
	/* Create our FIFO (before sending request, to avoid a race) */
	//umask(0); /* So we get the permissions we want */
	/*snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
	if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
		perror("mkfifo");
	if (atexit(removeFifo) != 0)
		perror("atexit");
		
	/* Construct request message, open server FIFO, and send request */
	int totalProcess=atoi(argv[1]);
	for (i = 0; i < totalProcess; ++i) {
		p = fork();
		if (p == 0)
			break;
	}
	if(p==0){//child process
		snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
		if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
			perror("mkfifo");
		if (atexit(removeFifo) != 0)
			perror("atexit");
		req.pid = getpid();
		//printf("pid child: %d \n",req.pid);
		//req.seqLen = atoi(argv[1]);
		serverFd = open(SERVER_FIFO, O_WRONLY);
		if (serverFd == -1)
			perror("open");
		if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
			perror("Can't write to server");

		/* Open our FIFO, read and display response */
		clientFd = open(clientFifo, O_RDONLY);
		if (clientFd == -1)
			perror("open");
		if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
			perror("Can't read response from server");
		printf("Musteri %d %d lira aldi.\n",req.pid, resp.money);
		exit(EXIT_SUCCESS);	
	}	
	for(i=0;i<totalProcess;i++){
		wait(NULL);
	}
		
	/*snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) getpid());
	if (mkfifo(clientFifo, S_IRUSR | S_IWUSR | S_IWGRP) == -1 && errno != EEXIST)
		perror("mkfifo");
	if (atexit(removeFifo) != 0)
		perror("atexit");
	req.pid = getpid();
	printf("pid parent: %d \n",req.pid);
	//req.seqLen = atoi(argv[1]);*/
	/*serverFd = open(SERVER_FIFO, O_WRONLY);
	if (serverFd == -1)
		perror("open");
	if (write(serverFd, &req, sizeof(struct request)) != sizeof(struct request))
		perror("Can't write to server");
	/* Open our FIFO, read and display response */
	/*clientFd = open(clientFifo, O_RDONLY);
	if (clientFd == -1)
		perror("open");
	if (read(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
		perror("Can't read response from server");
	printf("%d\n", resp.money);*/
	exit(EXIT_SUCCESS);

	
}
