//
//  Server.h
//  BrickDNS
//
//  Created by Alec Zadikian on 9/11/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

#ifndef BrickDNS_Server_h
#define BrickDNS_Server_h

#include <pthread.h>
#include "sqlite3.h"
#include "general.h"

struct Server{
    int sockfd; // server socket
    bool isRunning; // set to false to stop the server ASAP
    bool isListening; // whether the server is taking connections
    pthread_t* mainThread; // main thread descriptor
    double timeout; // timeout before disconnecting clients
    bool keepAlive; // whether client connections should be kept alive
    size_t sendSize; // size of send buffer in bytes
    size_t recvSize; // size of receive buffer in bytes
    int threadCount; // the total number of worker threads alive
    int threadUsage; // the number of worker threads being used
    int threadLimit; // the max number of worker threads (may be less than total)
};

// These are macros to the functions used to send/receive data.
// Example provided: Sends and receives using cTalk, which uses prepended variable-length quanitity values to indicate message length in order to separate messages.
/* vvv ** TODO YOUR CODE HERE ** vvv */
#define SERVER_SEND(socket, buffer, len) cTalkSend(socket, buffer, len)
#define SERVER_RECV(socket, buffer, len) cTalkRecv(socket, buffer, len)
/* ^^^ ** TODO YOUR CODE HERE ** ^^^ */

int startServer(struct Server* server);
int startListening(struct Server* server);
int stopListening(struct Server* server);
int stopServer(struct Server* server);

#endif
