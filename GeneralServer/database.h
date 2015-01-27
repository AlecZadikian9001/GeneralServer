//
//  database.h
//  UPServer2
//
//  Created by Alec Zadikian on 6/22/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

#include "general.h"
#include <stdio.h>
#include "sqlite3.h"

#ifndef UPServer2_database_h
#define UPServer2_database_h

#define stringColumn(row, column) \
((char*) ( ((uintptr_t*) row)[column] )) // Returns the string from the specified column (an integer) of the specified row (a value of a LinkedList returned by databaseSelect)

#define intColumn(row, column) \
*((int*) (((uintptr_t*)(row))[column])) // Returns the integer from the specified column (an integer) of the specified row (a value of a LinkedList returned by databaseSelect)

int databaseConnect(sqlite3 **db, char* dbURL); // Connects to a database, leaving db pointing to sqlite3 variable.
int databaseCommand(char* command, sqlite3* db); // Sends a command to the specified database.
struct LinkedList* databaseSelect(char* command, sqlite3* db, int count); // Selects data from a database and returns the results as a LinkedList of maximum length count (set count to -1 for "infinite"). Resulting LinkedList's values are to be parsed with the stringColumn and intColumn macros (above). Example: intColumn(linkedList->value, 4)

void freeRows(struct LinkedList* head); // Frees a linked list and all associated memory returned by databaseSelect

#endif
