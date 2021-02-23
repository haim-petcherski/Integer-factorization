//Main header

#ifndef __MAIN_H__
#define __MAIN_H__

//Includes:
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>
#include <windows.h>
#include "HardCodedData.h"
#include "Queue.h"
#include "Lock.h"
#include "Thread.h"


//Typedefs:
//Struct used to pass the thread args to each thread
typedef struct _thread_args {
    Queue* priority_q;
    Lock* lock;
    TCHAR* file_name;
} Thread_args;


//Function Handles:
//Prints errors according to the recieved status
void report_error(Status status);

//Pushing all the priorities of the tasks by their order into the queue
//Recieves pointer to the priorities tasks file and an empty queue
//Retruns a Status enum for debuging
Status update_priority_queue(file_priorities_pointer, queue_priorities);

//Start threading
//Creates the desiered number of threads, passes them the thread arguments and wait for all of them to finish
//Recieves the desiered number of threads and a pointer to the thread arguments
//Retruns a Status enum for debuging
Status start_threading(int num_of_threads, Thread_args thread_args);

#endif // __MAIN_H__