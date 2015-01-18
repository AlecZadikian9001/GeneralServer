//
//  Server.c
//  BrickDNS
//
//  Created by Alec Zadikian on 9/11/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

/*
 ███████╗███████╗██████╗ ██╗   ██╗███████╗██████╗     ██████╗
 ██╔════╝██╔════╝██╔══██╗██║   ██║██╔════╝██╔══██╗   ██╔════╝
 ███████╗█████╗  ██████╔╝██║   ██║█████╗  ██████╔╝   ██║
 ╚════██║██╔══╝  ██╔══██╗╚██╗ ██╔╝██╔══╝  ██╔══██╗   ██║
 ███████║███████╗██║  ██║ ╚████╔╝ ███████╗██║  ██║██╗╚██████╗
 ╚══════╝╚══════╝╚═╝  ╚═╝  ╚═══╝  ╚══════╝╚═╝  ╚═╝╚═╝ ╚═════╝
 */

#include <stdio.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include "general.h"
#include "ctalk.h"
#include "server.h"
#include "database.h"

/* Constants: */
#define DB_URL "sql.db" // TODO set if needed
/* */

struct ServerWorkerThread{
    struct Server* server; // the server options
    unsigned int threadID; // the thread ID for this worker thread
    unsigned int numWords; // number of words in master dictionary
    char* dictionary; // master dictionary, a 2D array of chars (numWords x LONGEST_WORD)
};

// These are macros to the functions used to send/receive data.
// Example provided: Sends and receives using cTalk, which uses prepended variable-length quanitity values to indicate message length in order to separate messages.
/* vvv ** TODO YOUR CODE HERE ** vvv */
#define SERVER_SEND(socket, buffer, len) cTalkSend(socket, buffer, len)
#define SERVER_RECV(socket, buffer, len) cTalkRecv(socket, buffer, len)
/* ^^^ ** TODO YOUR CODE HERE ** ^^^ */

void* workerThreadFunction(void* argVoid){
    struct ServerWorkerThread* arg = (struct ServerWorkerThread*) argVoid;
    int clntSock = 0;
    struct sockaddr_in clientAddress;
    socklen_t clntLen = sizeof(clientAddress);
    int servSock = arg->server->sockfd;
    sqlite3* db = NULL;
    struct timeval tv;
    tv.tv_sec = (int) arg->server->timeout; // timeout seconds
    tv.tv_usec = fmod(arg->server->timeout, 1.0)*1000000.0;  // timeout microseconds
    size_t recvBufLen = arg->server->recvSize;
    uint8_t* receiveBuffer = emalloc(sizeof(uint8_t)*recvBufLen);
    long receiveSize = 0;
    char replyBuffer[arg->server->sendSize];
    //*** Additional thread initialization code goes here ***//
    //vvvv//
    
    //^^^^
    while (true){ // vvv begin client listening loop
        while (!arg->server->isListening && arg->server->isRunning) usleep(1000*200);
        if (!arg->server->isRunning) break;
        if ((clntSock = accept(servSock, (struct sockaddr *) &clientAddress,
                               &clntLen)) >= 0){
            arg->server->threadUsage++;
            
            // Connects to SQLite database once a user connects.
            //databaseConnect(&db, DB_URL); // TODO uncomment this line if you want to use SQLite.
            
            while (!arg->server->isListening && arg->server->isRunning) usleep(1000*200);
            if (!arg->server->isRunning) break;
            setsockopt(clntSock, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv,sizeof(tv));
            if (logLevel >= LOG_ABNORMAL && arg->server->threadUsage >= arg->server->threadLimit) printf("Warning: All worker threads are in use. Consider creating more.\n");
            char* ipAddr = inet_ntoa(clientAddress.sin_addr);
            if (logLevel >= LOG_INFORMATIONAL) printf("Handling client %s on thread %d\n", ipAddr, arg->threadID);
            //*** Additional client connection handling code goes here ***//
            //vvvv//
            
            //^^^^
            do{
                receiveSize = SERVER_RECV(clntSock, receiveBuffer, recvBufLen);
                char check;
                bool isSanitary = true;
                for (long i = 0; i<receiveSize-1; i++){
                    // Check to make sure client input is sanitary
                    // Example provided: limits to lower-case letters, numbers, hyphons, and periods
                    /* vvv ** TODO YOUR CODE HERE ** vvv */
                    check = receiveBuffer[i];
                    if (!((check <= 'z' && check >= 'a') || (check >= '0' && check <= '9') || check == '-' || check == '.' || check == ',' || check == '\0')){
                        isSanitary = false;
                        break;
                    }
                    /* ^^^ ** TODO YOUR CODE HERE ** ^^^ */
                }
                if (!isSanitary){
                    if (logLevel >= LOG_ABNORMAL) printf("Client %s sent unsanitary input.\n", ipAddr);
                    break;
                }
                if (!arg->server->keepAlive) shutdown(clntSock, 0); // stop receiving data
                if (receiveSize < 1 || receiveSize > recvBufLen){
                    if (logLevel >= LOG_ABNORMAL) printf("Client %s sent invalid input.\n", ipAddr);
                    break;
                }
                // Now handle the input from the client
                // Example provided: echoes
                /* vvv ** TODO YOUR CODE HERE ** vvv */
                SERVER_SEND(clntSock, receiveBuffer, strlen(receiveBuffer)+1);
                /* ^^^ ** TODO YOUR CODE HERE ** ^^^ */
            } while (arg->server->keepAlive);
            close(clntSock);
            sqlite3_close(db);
            if (logLevel >= LOG_INFORMATIONAL) printf("Client %s disconnected on thread %d.\n", ipAddr, arg->threadID);
            arg->server->threadUsage--;
            if (arg->server->threadCount > arg->server->threadLimit || !arg->server->isRunning){
                break;
            }
        }
        else{
            if (logLevel>=LOG_ERROR){
                printf("accept() error: %d\n", errno);
            }
        }
    } // ^^^ end client listening loop
    if (logLevel >= LOG_ABNORMAL) printf("Worker thread %d shutting down.\n", arg->threadID);
    arg->server->threadCount--;
    close(clntSock);
    efree(receiveBuffer);
    efree(arg);
    pthread_detach(pthread_self());
    return NULL;
}

// Changes how many threads the server uses for its thread pool.
// You might want to make some console command or something that calls this.
// If this is more than before, it spawns new ones immediately.
// If this is less than before, unneeded ones die off once a user connects to them (i.e. not immediately).
// Developer comment: I should later implement the ability to immediately kill off threads.
int changeThreadLimit(int num, struct Server* server){
    if (logLevel >= LOG_INFORMATIONAL) printf("ChangeNumWorkerThreads called, attempting to change to %d threads.\n", num);
    if (num<0){
        if (logLevel >= LOG_ERROR) printf("Invalid thread limit %d specified in changeNumWorkerThreads. Defaulting to 64.\n", num);
        num = 64;
    }
    bool fail = false;
    int maxThreads = (int) sysconf(_SC_THREAD_THREADS_MAX);
    if (maxThreads > 0 && num > maxThreads - 4){
        server->threadLimit = maxThreads-4;
        if (logLevel >= LOG_ERROR) printf("The thread limit specified is less than 4 fewer than the maximum supported by the OS and is therefore likely unsafe. Setting it to 4 fewer than maximum, but this could theoretically still be unsafe.\n");
        fail = true;
    }
    server->threadLimit = num;
    if (num > server->threadCount){
        int created = 0;
        for (; created < server->threadLimit - server->threadCount; created++){
            struct ServerWorkerThread* arg = emalloc(sizeof(struct ServerWorkerThread));
            arg->server = server;
            arg->threadID = server->threadCount+created;
            pthread_t clientThread;
            if (pthread_create(&clientThread, NULL, workerThreadFunction, (void*) arg) != 0){
                if (logLevel >= LOG_ERROR) printf("Unable to create the number of worker threads specified.\n");
                efree(arg);
                fail = true; break;
            }
        }
        server->threadCount = server->threadCount + created;
        if (logLevel >= LOG_ABNORMAL) printf("Created %d threads.\n", created);
    }
    else{
        if (logLevel >= LOG_ABNORMAL) printf("%d threads will be shut down as new clients connect.\n", server->threadCount - server->threadLimit);
    }
    if (logLevel >= LOG_INFORMATIONAL) printf("Set thread limit to %d.\n", server->threadLimit);
    return fail?RET_THREAD_ERROR:RET_NO_ERROR;
}

// Starts the server.
int startServer(struct Server* mainArg){
    pthread_t mainThread;
    mainArg->mainThread = &mainThread;
    mainArg->threadCount = 0;
    mainArg->threadUsage = 0;
    mainArg->isRunning = true;
    
    // TODO Uncomment the following chunk of code if you want to use the SQLite database.
    // Checks to make sure you can connect to database.
    /*
    sqlite3* db = NULL;
    databaseConnect(&db, DB_URL);
    sqlite3_close(db);
    */
     
    return changeThreadLimit(mainArg->threadLimit, mainArg);
}

// Tells all client threads to shut down ASAP.
// Sets the server isRunning flag to false, which can have whatever consequences you intend.
int stopServer(struct Server* server){
    changeThreadLimit(0, server);
    server->isRunning = false;
    return RET_NO_ERROR;
}
int startListening(struct Server* server){
    server->isListening = true;
    return RET_NO_ERROR;
}
int stopListening(struct Server* server){
    server->isListening = false;
    return RET_NO_ERROR;
}