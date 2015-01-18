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

#define stringColumn(array, index) \
((char*) ( ((uintptr_t*) array)[index] ))

#define intColumn(array, index) \
*((int*) (((uintptr_t*)(array))[index]))



int databaseConnect(sqlite3 **db, char* dbURL); // Connects to a database, leaving db pointing to sqlite3 variable.
int databaseCommand(char* command, sqlite3* db); // Sends a command to the specified database.
struct LinkedList* databaseSelect(char* command, sqlite3* db, int count); // Selects data from a database. Resulting LinkedList's values are to be parsed with COLUMN macros (above).

void freeRows(struct LinkedList* head);

#endif
