//
//  general.h
//  AgreementFinderServer
//
//  Created by Alec Zadikian on 3/25/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

#ifndef AgreementFinderServer_general_h
#define AgreementFinderServer_general_h

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

// Constants:

#define numDigits(var) (log10(var)+1)

// Typedefs:
typedef enum{
    LOG_NONE, // Don't print anything.
    LOG_ERROR, // Print errors.
    LOG_ABNORMAL, // LOG_ERROR plus abnormal activity.
    LOG_INFORMATIONAL, // LOG_ABNORMAL plus normal activity.
    LOG_FULL, // LOG_INFORMATIONAL plus all network I/O!
} LogLevel;
typedef enum{
	RET_NO_ERROR, //0
	RET_INPUT_ERROR = -1,
    RET_SQL_ERROR = -2,
    RET_NETWORK_ERROR = -3,
    RET_THREAD_ERROR = -4,
    RET_MEMORY_ERROR = -5
} ReturnCode;

// Memory management:
#define efree(var) \
do{ \
free(var); \
var = NULL; \
} while (0) \

void * emalloc(size_t amt);
void * ecalloc(size_t amt);
void * erealloc(void* input, size_t amt);

// Structs and related functions:
struct LinkedList{
    struct LinkedList* next;
    size_t size;
    void* value;
};
void freeLinkedList(struct LinkedList* head, void freeFunction(void*));
bool strArrayContainsStr(char** arr, size_t arrLen, char* str);
void freeStrArray(char** arr, size_t arrLen);

// Global constants:
LogLevel logLevel;

// Other functions:
int max(int, ...);
#define streq(str1, str2) (strcmp(str1, str2)==0)

#endif