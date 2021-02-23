//Hard coded data header

#ifndef __HARD_CODED_DATA_H__
#define __HARD_CODED_DATA_H__

//Includes:
#include <windows.h>


//Defines:
#define TASK_LEN 11
#define MAX_THREADS MAXIMUM_WAIT_OBJECTS
#define MAX_WAIT_IN_MILISECONDS 10000

//Typedefs:
//Enum to organize the input argv
typedef enum _arg {
    TASKS_FILE = 1,
    PRIORITIES_FILE,
    NUM_TASKS,
    NUM_OF_THREADS,
    ARG_COUNT
} Arg;

//All of the main functions return a status enum for debugging
typedef enum _status {
    INVALID_STATUS_CODE = -1,
    SUCCESS = 0,
    WRONG_ARGUMENT_COUNT,
    INVALID_NUM_OF_THREADS,
    FOPEN_FAIL,
    ALLOC_FAILED,
    REALLOC_FAILED,
    FAILED_TO_OPEN_FILE_FOR_READ,
    FAILED_TO_OPEN_FILE_FOR_WRITE,
    FAILED_TO_SET_FILE_PTR,
    FAILED_TO_SET_EOF,
    FAILED_TO_CREARTE_THREAD,
    FAILED_TO_READ_FILE,
    FAILED_TO_WRITE_FILE,
    INIT_LOCK_FAILED,
    FAILED_ON_WAIT_TIMEOUT = WAIT_TIMEOUT,
    FAILED_ON_WAIT_FAILED = WAIT_FAILED
} Status;

#endif // __HARD_CODED_DATA_H__
