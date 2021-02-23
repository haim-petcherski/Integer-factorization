//Lock header

#ifndef __LOCK_H__
#define __LOCK_H__

//Includes:
#include <stdio.h> 
#include <stdlib.h> 
#include <windows.h>
#include "HardCodedData.h"


//Typedefs:
typedef struct Lock {
    HANDLE read_mutex;
    HANDLE write_mutex;
    int readers_count;
}Lock;


//Function Handles:
//Retuns a pointer to a lock and initializes it with mutexes
Lock* InitializeLock();

//Locks the write_mutex if it is the first read request and increments readers_count
//Returns the error code of the wait function
DWORD read_lock(Lock* lock);

//Decrements readers_count and releases write_mutex if all the reads finished
void read_release(Lock* lock);

//Locks the write_mutex
//Returns the error code of the wait function
DWORD write_lock(Lock* lock);

//Releases the write_mutex
void write_release(Lock* lock);

//Closes the mutex handles, frees allocated memory and updates lock to NULL
void DestroyLock(Lock* lock);

#endif // __LOCK_H__