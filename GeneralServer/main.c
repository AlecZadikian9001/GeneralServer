//
//  main.c
//  BrickDNS
//
//  Created by Alec Zadikian on 9/11/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

#include <stdio.h>
#include <sys/uio.h>
#include <sys/types.h>  /* for Socket data types */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <netinet/in.h> /* for IP Socket data types */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <math.h>
#include <stdint.h>
#include <pthread.h>
#include <time.h>
#include <uuid/uuid.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <sys/ioctl.h>
#include "testClient.h"
#include "server.h"
#include "general.h"

#define PORT 9001 // TODO change
#define MAX_PENDING 9000 // TODO change

int main(int argc, const char * argv[]){
    if (argc!=5){
        printf("Invalid usage. Start with: GeneralServer [logLevel] [threadLimit] [timeout] [keepAlive]\n");
        return 0;
    }
    logLevel = (atoi(argv[1])>LOG_FULL)?LOG_FULL:atoi(argv[1]);
    if (logLevel > LOG_NONE){
        printf("Starting GeneralServer server on port %d...\n", PORT);
    }
    
    // Create listening socket:
    int servSock;                    /* Socket descriptor for server */
    struct sockaddr_in serverAddress; /* Local address */
    signal(SIGPIPE, SIG_IGN); // ignore that shit error that would stop my server
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){ if (logLevel >= LOG_ERROR) printf("Socket creation failed!\n"); return RET_NETWORK_ERROR; }
    bzero(&serverAddress, sizeof(serverAddress));   /* Zero out structure */
    serverAddress.sin_family = AF_INET;                /* Internet address family */
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    serverAddress.sin_port = htons(PORT);      /* Local port */
    int yes=1; //char yes='1'; // use this under Solaris
    if (setsockopt(servSock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    if (bind(servSock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){ if (logLevel >= LOG_ERROR) printf("Socket binding failed!\n"); return RET_NETWORK_ERROR; }
    if (listen(servSock, MAX_PENDING) < 0){ if (logLevel >= LOG_ERROR) printf("Socket listening failed!\n"); return RET_NETWORK_ERROR; }
    
    struct Server server;
    server.sockfd = servSock;
    server.threadLimit = atoi(argv[2]); //16;
    server.timeout = atof(argv[3]); //100;
    server.keepAlive = atoi(argv[4]); // true;
    server.isListening = true;
    server.isRunning = false;
    server.recvSize = 1024;
    if (logLevel>=LOG_ABNORMAL) printf("logLevel: %d\nthreadLimit: %d\ntimeout: %f\nkeepAlive: %d\n",logLevel,server.threadLimit, server.timeout, server.keepAlive);
    
    startServer(&server);
    interactiveTest(); // Runs the test defined in testClient.h; you may comment this line out.
    
    // Now wait as the worker threads run.
    // Stops once the thread count hits 0.
    // TODO You might want to change this behavior. Maybe stop immediately if server.isRunning is false?
    // pause(); // If you want to make it just wait forever.
    while (server.threadCount > 0) sleep(1); // If you want it to stop once the thread count hits 0.
    if (logLevel>=LOG_ABNORMAL) printf("\n*** SERVER STOPPING ***\n");
    
}

