//
//  testClient.c
//  BrickDNS
//
//  Created by Alec Zadikian on 10/4/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "general.h"
#include "ctalk.h"

#include "testClient.h"

int interactiveTest(){
    int sockfd = 0;
    char keyboard[128];
    while (true){
        printf("stdin >> ");
        fgets(keyboard, sizeof(keyboard), stdin);
        struct sockaddr_in serv_addr;
        if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("\n Error : Could not create socket \n");
            return RET_NETWORK_ERROR;
        }
        memset(&serv_addr, '0', sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(9001);
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0){
            printf("\n inet_pton error occured\n");
            return RET_NETWORK_ERROR;
        }
        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            printf("\n Error : Connect Failed \n");
            return RET_NETWORK_ERROR;
        }
        keyboard[strlen(keyboard)-1] = '\0';
        cTalkSend(sockfd, keyboard, strlen(keyboard)+1);
        cTalkRecv(sockfd, keyboard, sizeof(keyboard));
        printf("stdout >> %s\n", keyboard);
    }
}