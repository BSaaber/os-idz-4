#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define ECHOMAX 255

void DieWithError(char *errorMessage);  /* Error handling function */
void HandleTCPClient(int clntSocket, int* free_space);   /* TCP client handling function */

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    unsigned int cliAddrLen;         /* Length of incoming message */

    if (argc != 3)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port> <Rooms amount>\n", argv[0]);
        exit(1);
    }

    echoServPort = atoi(argv[1]);  /* First arg:  local port */

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    printf("Server IP address = %s. Wait...\n", inet_ntoa(echoClntAddr.sin_addr));


    int free_space = atoi(argv[2]);

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        cliAddrLen = sizeof(echoClntAddr);


        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        //////////////////// HandleTCPClient  //////////////////////


        char echoBuffer[ECHOMAX];        /* Buffer for echo string */
        int recvMsgSize;                    /* Size of received message */
        char* success_response = "okay\0";
        char* error_response =   "fail\0";
        char* rent_request = "rent\0";
        char* free_request = "free\0";
        char* response;

        /* Receive message from client */
        if ((recvMsgSize = recvfrom(servSock, echoBuffer, ECHOMAX, 0,
                                    (struct sockaddr *) &echoClntAddr, &cliAddrLen)) < 0)
            DieWithError("recvfrom() failed");
        printf("log: free space - %d\n", free_space);

        /* Send received string and receive again until end of transmission */
            if (!strcmp(free_request, echoBuffer)) {
                free_space += 1;
                response = success_response;
                printf("log: freeing room. space now - %d\n", free_space);
            } else if (!strcmp(rent_request, echoBuffer)) {
                if (free_space > 0) {
                    free_space -= 1;
                    response = success_response;
                    printf("log: allocating room. space now - %d\n", free_space);
                } else {
                    response = error_response;
                    printf("log: all rooms are allocated. free space - %d\n", free_space);
                }
            }
//            /* Echo message back to client */
//            if (send(clntSocket, response, strlen(response), 0) != strlen(response))
//                DieWithError("send() failed");

        /* Send received datagram back to the client */
        if (sendto(servSock, response, strlen(response), 0,
                   (struct sockaddr *) &echoClntAddr, sizeof(echoClntAddr)) != strlen(response))
            DieWithError("sendto() sent a different number of bytes than expected");

        //close(servSock);    /* Close client socket */
        printf("log: connection closed\n");
        ////////////////////////////////////////////////////////////

    }
    /* NOT REACHED */
}

