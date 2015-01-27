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

#define numDigits(var) (log10(var)+1) // Returns the number of digits of an integer when represented in base 10 notation

// Typedefs:
typedef enum{
    LOG_NONE, // Don't print anything.
    LOG_ERROR, // Print errors.
    LOG_ABNORMAL, // LOG_ERROR plus abnormal activity.
    LOG_INFORMATIONAL, // LOG_ABNORMAL plus normal activity.
    LOG_FULL, // Everything that can possible be logged. By default, this will be LOG_INFORMATIONAL plus all network I/O.
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
// Frees the memory a variable is pointing to and sets the variable's value to NULL
#define efree(var) \
do{ \
free(var); \
var = NULL; \
} while (0) \

void * emalloc(size_t amt); // malloc with error detection
void * ecalloc(size_t amt); // calloc with error detection
void * erealloc(void* input, size_t amt); // realloc with error detection

// Structs and related functions:
struct LinkedList{
    struct LinkedList* next;
    size_t size;
    void* value;
};
void freeLinkedList(struct LinkedList* head, void freeFunction(void*)); // Calls freeFunction() on all of a LinkedList's values then frees it
bool strArrayContainsStr(char** arr, size_t arrLen, char* str); // Returns true IFF a string identical to str is inside arr
void freeStrArray(char** arr, size_t arrLen); // Frees a string array

// Global constants:
LogLevel logLevel;

// Other functions:
int max(int, ...); // MAX with flexible paramters, first parameter is number of parameters
#define streq(str1, str2) (strcmp(str1, str2)==0) // Returns true IFF str1 and str2 are identical strings

#endif