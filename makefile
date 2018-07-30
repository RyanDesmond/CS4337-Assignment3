all: TCPClient TCPServer TCPClient2_1 TCPClient2_2 TCPClient3_1 TCPClient3_2 TCPClient3_3

TCPClient: a3p1Client1.c
	gcc -o TCPClient a3p1Client1.c

TCPServer: a3p1Server1.c
	gcc -o TCPServer a3p1Server1.c -lnsl -lpthread

TCPClient2_1: a3p2Client1.c
	gcc -o TCPClient2_1 a3p2Client1.c

TCPClient2_2: a3p2Client2.c
	gcc -o TCPClient2_2 a3p2Client2.c

TCPClient3_1: a3p3Client1.c
	gcc -o TCPClient3_1 a3p3Client1.c

TCPClient3_2: a3p3Client2.c
	gcc -o TCPClient3_2 a3p3Client2.c

TCPClient3_3: a3p3Client3.c
	gcc -o TCPClient3_3 a3p3Client3.c
