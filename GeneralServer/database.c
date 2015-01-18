//
//  database.c
//  UPServer2
//
//  Created by Alec Zadikian on 6/22/14.
//  Copyright (c) 2014 AlecZ. All rights reserved.
//

#include "database.h"
#include "general.h"
#include <stdio.h>
#include "sqlite3.h"
#include <strings.h>
#include <string.h>

int databaseConnect(sqlite3 **db, char* url){
    int rc = sqlite3_open_v2(url, db, SQLITE_OPEN_READWRITE, NULL);
    if(rc!=SQLITE_OK){
        if (logLevel >= LOG_ERROR) printf("Can't open database! Error: %s\n", sqlite3_errmsg(*db));
        sqlite3_close_v2(*db);
        return(RET_SQL_ERROR);
    }
    return RET_NO_ERROR;
}

int databaseCommand(char* command, sqlite3* db){
    char* error = NULL;
    int ret = sqlite3_exec(db, command, NULL, 0, &error);
	if (ret!=SQLITE_OK){
		//if (logLevel >= LOG_ERROR) printf("SQL command %s aborted. Error: %s\n", command, error);
		return RET_SQL_ERROR;
	}
    if (error) sqlite3_free(error);
    return RET_NO_ERROR;
}

struct LinkedList* databaseSelect(char* command, sqlite3* db, int count){ //returns null upon error
    sqlite3_stmt* stmt = NULL;
    int retval = retval = sqlite3_prepare_v2(db,command,(int) (strlen(command))*sizeof(char),&stmt,NULL);
    if(retval!=SQLITE_OK){
        if (logLevel >= LOG_ERROR) printf("Selecting data from database failed! Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return NULL;
    }
    struct LinkedList* ret = emalloc(sizeof(struct LinkedList)); bzero(ret, sizeof(struct LinkedList));
    struct LinkedList* index = ret;
    int result = sqlite3_step (stmt);
    if (result != SQLITE_ROW){
        efree(ret);
        sqlite3_finalize(stmt);
        return NULL;
    }
    int i = 0;
    while ((count==-1 || i<count) && result != SQLITE_DONE && result != SQLITE_ERROR){
        if (result == SQLITE_ROW) { //begin row loop
            const int numCols = sqlite3_column_count(stmt);
            uintptr_t* row = emalloc(sizeof(uintptr_t)*numCols); //make the array representing a row of columns
            
            for (int colIndex = 0; colIndex<numCols; colIndex++){ //begin column loop
                switch (sqlite3_column_type(stmt, colIndex)){
                    case SQLITE_INTEGER:{
                        const int col = sqlite3_column_int(stmt, colIndex);
                        int* colPointer = emalloc(sizeof(int));
                        *colPointer = col;
                        row[colIndex] = (uintptr_t) colPointer;
                        break;
                    }
                    case SQLITE_TEXT:{
                        const char* text = strdup((const char*) sqlite3_column_text(stmt, colIndex));
                        row[colIndex] = (uintptr_t) text;
                        /*TEMPORARY*/ //printf("At column %d, DB returned: %s\n",colIndex,(char*)row[colIndex]);
                        break;
                    }
                    default:{
                        //if (logLevel >= LOG_ERROR) printf("Strange error selecting data from SQLite database. Found an invalid value.\n");
                        break;
                    }
                }
            } //end column loop
            
            index->value = row;
            index->size = sizeof(uintptr_t)*numCols;
            index->next = emalloc(sizeof(struct LinkedList)); bzero(index->next, sizeof(struct LinkedList));
            index = index->next;
        } //end row loop
        result = sqlite3_step (stmt);
    }
    sqlite3_finalize(stmt);
    return ret;
}

void freeRows(struct LinkedList* head){ //to free linked lists returned by the above function
    freeLinkedList(head, &free);
}

