#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096 /*max text line length*/
#define SERV_PORT 39631 /*port*/

int
main(int argc, char **argv) 
{
 int sockfd, l;
 struct sockaddr_in servaddr;
 char sendline[MAXLINE], recvline[MAXLINE], returnOutput[MAXLINE], endOfProgram[MAXLINE];
 
 // alarm(300);  // to terminate after 300 seconds
	
 //basic check of the arguments
 //additional checks can be inserted
 if (argc !=3) {
  perror("Usage: TCPClient <Server IP> <Server Port>");
  exit(1);
 }
	
 //Create a socket for the client
 //If sockfd<0 there was an error in the creation of the socket
 if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0) {
  perror("Problem in creating the socket");
  exit(2);
 }
 //Set the command to be repeated
 /*while (fgets(sendline, MAXLINE, stdin) == NULL){
	printf("Invalid command, please try again.");
 }*/

 //Creation of the socket
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr= inet_addr(argv[1]);
 servaddr.sin_port =  htons((int)strtol(argv[2], (char **)NULL, 10)); 
 // servaddr.sin_port =  htons(SERV_PORT); //convert to big-endian order
 printf("%s \n", "Creation of servadder complete. Now establishing connection...");

 //Connection of the client to the socket 
 if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0) {
  perror("Problem in connecting to the server");
  exit(3);
 }
 //Create log file
 FILE * logFile = fopen("a3p3Client1.log", "a");
 /*fputs("Command Sent: ", logFile);
 fputs(sendline, logFile);*/

 //Loop through connection run and return 10 times.
 for (l = 1; l < 11; l+=1) {
  printf("%s %d \n", "Sending connection, attempt #", l);
  // Command 1 : Date
  send(sockfd, "date", strlen("date"), 0);
  memset(returnOutput, 0, MAXLINE);
  strcpy(returnOutput, "\nString Received from the server:\n");	
  if (recv(sockfd, recvline, MAXLINE,0) != 0){
	strcat(returnOutput, recvline);
  }
  fputs(returnOutput, stdout);
  fputs(returnOutput, logFile);
  // Command 2: ls -l junk.txt
  send(sockfd, "ls -l junk.txt", strlen("ls -l junk.txt"), 0);
  memset(returnOutput, 0, MAXLINE);
  strcpy(returnOutput, "\nString Received from the server:\n");	
  if (recv(sockfd, recvline, MAXLINE,0) != 0){
	strcat(returnOutput, recvline);
  }
  fputs(returnOutput, stdout);
  fputs(returnOutput, logFile);
  // Command 3: touch junk.txt
  send(sockfd, "touch junk.txt", strlen("touch junk.txt"), 0);
  memset(returnOutput, 0, MAXLINE);
  strcpy(returnOutput, "\nString Received from the server:\n");	
  if (recv(sockfd, recvline, MAXLINE,0) != 0){
	strcat(returnOutput, recvline);
  }
  fputs(returnOutput, stdout);
  fputs(returnOutput, logFile);
  
  sleep(2);
 }
 fclose(logFile);
 system("date");
 sprintf(endOfProgram, "%u", (intmax_t)getpid());
 printf("PID: %s\n", endOfProgram);
 exit(0);
}
