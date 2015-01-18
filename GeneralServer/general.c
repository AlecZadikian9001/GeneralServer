//
//  general.c
//  AgreementFinderServer
//
//  Created by Alec Zadikian on 3/25/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

#include <stdio.h>
#include <strings.h>
#include "general.h"

void * emalloc(size_t amt){
    void *v = malloc(amt);
    if(!v){
        fprintf(stderr, "Out of memory! Halting server NOW!!!\n");
        exit(RET_MEMORY_ERROR);
    }
    return v;
}
void * ecalloc(size_t amt){
    void *v = emalloc(amt);
    bzero(v, amt);
    return v;
}
void * erealloc(void* input, size_t amt){
    void *v = realloc(input, amt);
    if(!v){
        fprintf(stderr, "Out of memory! Halting server NOW!!!\n");
        exit(RET_MEMORY_ERROR);
    }
    return v;
}

void freeLinkedList(struct LinkedList* head, void freeFunction(void*)){ //Some pretty legit recursion up in this crib.
    if (!head) return;
    if (head->value) freeFunction(head->value);
    if (head->next) freeLinkedList(head->next, freeFunction);
    efree(head);
}

int max(int numArgs, ...){
    va_list args;
    va_start(args, numArgs);
    int ret = 0;
    int n = 0;
    for(unsigned int i = 0; i < numArgs; ++i){
        n = va_arg(args, int);
        if (n>ret) ret = n;
    }
    va_end(args);
    return ret;
}





