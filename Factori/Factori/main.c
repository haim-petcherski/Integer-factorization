#include "main.h"

Status main(int argc, char* argv[])
{
    errno_t error_fopen;
    FILE* file_priorities_pointer = NULL;
    Queue* queue_priorities = NULL;
    char* tasks_path = NULL;
    char* priorities_path;
    int num_of_threads;
    Thread_args thread_args;
    Status status = INVALID_STATUS_CODE;


    //Check for correct input argument count
    if (argc != ARG_COUNT) {
        printf("Error - got %d arguments, expecting %d", argc, ARG_COUNT);
        return WRONG_ARGUMENT_COUNT;
    }

    //Get input arguments
    tasks_path = argv[TASKS_FILE];
    num_of_threads = atoi(argv[NUM_OF_THREADS]);
    priorities_path = argv[PRIORITIES_FILE];

    //Check number of threads validity
    if (num_of_threads <= 0 || num_of_threads > MAX_THREADS) {
        printf("Error - Invalid number of threads %d", num_of_threads);
        return INVALID_NUM_OF_THREADS;
    }

    if ((error_fopen = fopen_s(&file_priorities_pointer, priorities_path, "r")) != 0)
        report_error(FOPEN_FAIL);
    //building queue q of priorities

    queue_priorities = InitializeQueue();
    if (queue_priorities == NULL)
    {
        fclose(file_priorities_pointer);
        report_error(ALLOC_FAILED);
    }

    if ((status = update_priority_queue(file_priorities_pointer, queue_priorities)) != SUCCESS)
    {
        fclose(file_priorities_pointer);
        DestroyQueue(queue_priorities);
        report_error(status);
    }

    //finish building queue q of priorities
    fclose(file_priorities_pointer);

    thread_args.file_name = tasks_path; 
    thread_args.priority_q = queue_priorities;
    thread_args.lock = InitializeLock();
    if (thread_args.lock == NULL) {
        DestroyQueue(queue_priorities);
        report_error(INIT_LOCK_FAILED);
    }
    //create thread func

    status = start_threading(num_of_threads, thread_args);
    if (status) {
        DestroyQueue(queue_priorities);
        report_error(status);
    }

    DestroyQueue(queue_priorities);
    DestroyLock(thread_args.lock);

    return SUCCESS;
}

Status update_priority_queue(FILE* file_priorities_pointer, Queue* queue_priorities)
{
    char digit_priority;
    int j = -1;
    int size = TASK_LEN;
    char* offset_priority = NULL;
    offset_priority = (char*)calloc(size, sizeof(char));
    char* realloc_attempt = NULL;

    if (offset_priority == NULL)
    {
        return ALLOC_FAILED;
    }

    while ((digit_priority = getc(file_priorities_pointer)) != EOF)
    {
        if (j >= size - 1)
        {
            /* time to make it bigger */
            size += TASK_LEN;
            realloc_attempt = (char*)realloc(offset_priority, size * sizeof(char));
            //If reallocation fails, free the previously allocated memory
            if (realloc_attempt == NULL) {
                free(offset_priority);
                return REALLOC_FAILED;
            }
            else offset_priority = realloc_attempt;
        }
        if (digit_priority == '\n') {
            j++;
            offset_priority[j] = '\0';
            Push(queue_priorities, atoi(offset_priority));
            if (queue_priorities == NULL)
            {
                return ALLOC_FAILED;
            }
            j = -1;
        }
        else
        {
            j++;
            offset_priority[j] = digit_priority;
        }
    }
    free(offset_priority);
    return SUCCESS;
}
Status start_threading(int num_of_threads, Thread_args thread_args)
{
    DWORD dwEvent;
    HANDLE  threads_h[MAX_THREADS];
    for (int thread = 0; thread < num_of_threads; thread++) {

        threads_h[thread] = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size  
            MyThreadFunction,       // thread function name
            &thread_args,   // argument to thread function 
            0,                      // use default creation flags 
            NULL);
        if (threads_h[thread] == NULL)
            return FAILED_TO_CREARTE_THREAD;
    }

    //Wait for all threads to finish
    dwEvent = WaitForMultipleObjects(num_of_threads, threads_h, TRUE, MAX_WAIT_IN_MILISECONDS);
    switch (dwEvent)
    {
    case WAIT_TIMEOUT:
        report_error(WAIT_TIMEOUT);

        // Return value is invalid.
    case WAIT_FAILED:
        printf("Wait error: %d\n", GetLastError());
        report_error(WAIT_FAILED);
    }

    //Close thread handles and exit
    for (int thread = 0; thread < num_of_threads; thread++) {
        CloseHandle(threads_h[thread]);
    }

    return SUCCESS;

}

void report_error(Status status) {
    switch (status) {
    case FOPEN_FAIL:                    printf("Error - Failed Opening the input file"); break;
    case ALLOC_FAILED:                  printf("Error - Allocation failed"); break;
    case REALLOC_FAILED:                printf("Error - Reallocation failed"); break;
    case FAILED_TO_OPEN_FILE_FOR_READ:  printf("Error - Failed at CreateFile() for read file"); break;
    case FAILED_TO_OPEN_FILE_FOR_WRITE: printf("Error - Failed at CreateFile() for write file"); break;
    case FAILED_TO_SET_FILE_PTR:        printf("Error - Failed at SetFilePointer()"); break;
    case FAILED_TO_CREARTE_THREAD:      printf("Error - Failed at CreateThread()"); break;
    case FAILED_TO_READ_FILE:           printf("Error - Failed at ReadFile()"); break;
    case FAILED_TO_WRITE_FILE:          printf("Error - Failed at WriteFile()"); break;
    case FAILED_ON_WAIT_TIMEOUT:        printf("Error - Failed because of timeout"); break;
    case FAILED_ON_WAIT_FAILED:         printf("Error - Failed at wait_function"); break;
    }
    exit(status);
}