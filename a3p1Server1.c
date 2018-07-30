#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/*
    CONCURRENT SERVER: THREAD EXAMPLE
    Must be linked with the "pthread" library also, e.g.:
       cc -o example example.c -lnsl -lsocket -lpthread 

    This program creates a connection socket, binds a name to it, then
    listens for connections to the sockect.  When a connection is made,
    it accepts messages from the socket until eof, and then waits for
    another connection...

    This is an example of a CONCURRENT server -- by creating threads several
    clients can be served at the same time...

    This program has to be killed to terminate, or alternately it will abort in
    120 seconds on an alarm...
*/

#define PORTNUMBER 39631
#define BUFSIZE 1025

struct serverParm {
           int connectionDesc;
       };

void *serverThread(void *parmPtr) {

#define PARMPTR ((struct serverParm *) parmPtr)
    int recievedMsgLen;
    char messageBuf[BUFSIZE], returnBuf[BUFSIZE], endOutput[BUFSIZE];
    char headerString[32];
    FILE * logFile = fopen("a3p1Server.log", "a");
    FILE *fp;

    //Append Header to log File
    sprintf(headerString, "%u", (intmax_t)getpid());
    strcat(headerString, ": PID\n");
    fputs(headerString, logFile);
    /* Server thread code to deal with message processing */
    printf("DEBUG: connection made, connectionDesc=%d\n",
            PARMPTR->connectionDesc);
    if (PARMPTR->connectionDesc < 0) {
        printf("Accept failed\n");
        return(0);    /* Exit thread */
    }
    
    /* Receive messages from sender... */
    while ((recievedMsgLen=
            read(PARMPTR->connectionDesc,messageBuf,sizeof(messageBuf)-1)) > 0) 
    {
	memset(endOutput, 0, BUFSIZE);
	recievedMsgLen[messageBuf] = '\0';	
	printf("Message: %s\n",messageBuf);
	/* Create child process to run command. Pipe output to fp FILE pointer */
	if ((fp = popen(messageBuf, "r")) == NULL) {
		perror("Server: Error opening pipe");
		exit(1);
	}
	/* Reads fp file into return buffer*/
	printf("Command Output:\n");
	while (fgets(returnBuf, BUFSIZE, fp)){
		printf("%s\n", returnBuf);
		strcat(endOutput, returnBuf);
	}
	/*else {
		perror("Server: Error putting file output into output buffer");
		exit(2);
	}*/

	/* Write returnbuffer to connection */
        if (write(PARMPTR->connectionDesc,endOutput,1025) < 0) {
        	perror("Server: write error");
        	return(0);
        }
	fputs(messageBuf, logFile);
	fputs(endOutput, logFile);
    }
    close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
    free(PARMPTR);                   /* And memory leaks */
    fclose(fp);
    fclose(logFile);
    return(0);                       /* Exit thread */
}

main(int argc, char **argv[]) 
{	
    int listenDesc;
    struct sockaddr_in myAddr;
    struct serverParm *parmPtr;
    int connectionDesc;
    pthread_t threadID;

	if (argc != 2){
		//Error in arguments.
		perror("Usage: TCPServer <Server Port>");
		exit(5);
	}
    /* For testing purposes, make sure process will terminate eventually */
    alarm(120);  /* Terminate in 120 seconds */

    /* Create socket from which to read */
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("open error on socket");
        exit(3);
    }
	
    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons((int) strtol(argv[1], (char **)NULL, 10));
        
    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0) {
        perror("bind error");
        exit(2);
    }

    /* Start accepting connections.... */
    /* Up to 5 requests for connections can be queued... */
    listen(listenDesc,5);

    while (1) /* Do forever */ {
        /* Wait for a client connection */
        connectionDesc = accept(listenDesc, NULL, NULL);

        /* Create a thread to actually handle this client */
        parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
        parmPtr->connectionDesc = connectionDesc;
        if (pthread_create(&threadID, NULL, serverThread, (void *)parmPtr) 
              != 0) {
            perror("Thread create error");
            close(connectionDesc);
            close(listenDesc);
            exit(1);
        }

        printf("Parent ready for another connection\n");
    }

}
