#include    <signal.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    "bank_client.h"

int main(int argc, char *argv[]) {
	int serverFd, dummyFd, clientFd;
	char clientFifo[CLIENT_FIFO_NAME_LEN];
	struct request req;
	struct response resp;
	int randMoney; /* This is our "service" */

	/* Create well-known FIFO, and open it for reading */
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
	for (;;) { /* Read requests and send responses */
		if (read(serverFd, &req, sizeof(struct request)) != sizeof(struct request)) {
			fprintf(stderr, "Error reading request; discarding\n");
			continue; /* Either partial read or error */
		}
		printf("req.pid : %d\n", req.pid);
		/* Open client FIFO (previously created by client) */
		snprintf(clientFifo, CLIENT_FIFO_NAME_LEN, CLIENT_FIFO_TEMPLATE, (long) req.pid);
		clientFd = open(clientFifo, O_WRONLY);
		if (clientFd == -1) { /* Open failed, give up on client */
			perror("open3");
			continue;
		}
		/* Send response and close FIFO */
		randMoney=rand()%100;
		resp.money = randMoney;
		if (write(clientFd, &resp, sizeof(struct response)) != sizeof(struct response))
			fprintf(stderr, "Error writing to FIFO %s\n", clientFifo);
		if (close(clientFd) == -1)
			perror("close");
		//seqNum += req.seqLen; /* Update our sequence number */
	}
}
