#include "Thread.h"


DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
    DWORD status;
    Thread_args* args = (Thread_args*)lpParam;
    Queue* priority_q = args->priority_q;
    Lock* lock = args->lock;
    HANDLE read_file_h = NULL;
    HANDLE write_file_h = NULL;
    DWORD nRead;
    DWORD nWrite;
    char read_buffer[1] = { 0 };
    char* task = NULL;
    char* write_buffer = NULL;
    char* realloc_attempt = NULL;
    int cur_max_task_len = TASK_LEN;
    int char_count = 0;


    //Allocate memory for the task
    task = (char*)malloc(TASK_LEN * sizeof(char));
    if (task == NULL) {
        report_error(ALLOC_FAILED);
    }

    //Create read file handle
    if (INVALID_HANDLE_VALUE == (read_file_h = CreateFile(args->file_name, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
    {
        printf("Terminal failure: Unable to open file for reading: (%d).\n", GetLastError());
        free(task);
        report_error(FAILED_TO_OPEN_FILE_FOR_READ);
    }

    //Create write file handle
    if (INVALID_HANDLE_VALUE == (write_file_h = CreateFile(args->file_name, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)))
    {
        printf("Terminal failure: Unable to open file for writing: (%d).\n", GetLastError());
        free(task);
        CloseHandle(read_file_h);
        report_error(FAILED_TO_OPEN_FILE_FOR_WRITE);
    }


    //Start working on tasks until all done
    while (1)
    {
        //Lock for write on the critical part of the queue access
        status = write_lock(lock);
        if (status == (WAIT_TIMEOUT || WAIT_FAILED)) {
            free(task);
            CloseHandle(read_file_h);
            CloseHandle(write_file_h);
            report_error(status);
        }

        //CRITICAL_SECTION
        //Finished the tasks, return
        if (Empty(priority_q)) {
            write_release(lock);
            break;
        }

        //Set file pointer the highest priority
        if (INVALID_SET_FILE_POINTER == SetFilePointer(read_file_h, Top(priority_q)->value, NULL, FILE_BEGIN)) {
            free(task);
            CloseHandle(read_file_h);
            CloseHandle(write_file_h);
            report_error(FAILED_TO_SET_FILE_PTR);
        }
        Pop(priority_q);
        //END_OF_CRITICAL_SECTION

        //Release write lock after queue access finished
        write_release(lock);


        //Lock for read
        status = read_lock(lock);
        if (status == (WAIT_TIMEOUT || WAIT_FAILED)) {
            free(task);
            CloseHandle(read_file_h);
            CloseHandle(write_file_h);
            report_error(status);
        }

        //Read file until end of line, reallocate more space for task if needed 
        while (1) {
            if (FALSE == ReadFile(read_file_h, read_buffer, 1, &nRead, NULL))
            {
                free(task);
                CloseHandle(read_file_h);
                CloseHandle(write_file_h);
                report_error(FAILED_TO_READ_FILE);
            }
            if (read_buffer[0] == '\r') break; //Reached end of task

            task[char_count] = read_buffer[0];
            char_count++;

            //Reallocate more memory if needed
            if (char_count == cur_max_task_len - 1) {
                cur_max_task_len += TASK_LEN;
                realloc_attempt = (char*)realloc(task, cur_max_task_len * sizeof(char));
                
                //If reallocation fails, free the previously allocated memory
                if (realloc_attempt == NULL) { 
                    free(task);
                    CloseHandle(read_file_h);
                    CloseHandle(write_file_h);
                    report_error(REALLOC_FAILED);
                }
                else task = realloc_attempt;
            }
        }
        task[char_count] = '\0';
        char_count = 0;

        //Release read lock
        read_release(lock);


        //Updates write_buffer with the output
        status = calc_output(&write_buffer, task);
        if (status) {
            free(task);
            free(write_buffer);
            CloseHandle(read_file_h);
            CloseHandle(write_file_h);
            report_error(status);
        }


        //Lock for write
        status = write_lock(lock);
        if (status == (WAIT_TIMEOUT || WAIT_FAILED)) {
            free(task);
            free(write_buffer);
            CloseHandle(read_file_h);
            CloseHandle(write_file_h);
            report_error(status);
        }

        //Set file pointer to the end of the file
        if (INVALID_SET_FILE_POINTER == SetFilePointer(write_file_h, 0, NULL, FILE_END)) {
            free(task);
            free(write_buffer);
            CloseHandle(read_file_h);
            CloseHandle(write_file_h);
            report_error(FAILED_TO_SET_FILE_PTR);
        }

        //Write output line
        if (FALSE == WriteFile(write_file_h, write_buffer, strlen(write_buffer), &nWrite, NULL))
        {
            free(task);
            free(write_buffer);
            CloseHandle(read_file_h);
            CloseHandle(write_file_h);
            report_error(FAILED_TO_WRITE_FILE);
        }

        //Release write lock
        write_release(lock);

        free(write_buffer);
        write_buffer = NULL;
    }

    free(task);
    CloseHandle(read_file_h);
    CloseHandle(write_file_h);

    return SUCCESS;
}


Status calc_output(char** write_buffer, char* task) {
    Queue* primes_q = InitializeQueue();
    int output_len = 0;
    int primes_c = 0;
    int digits_c = 0;

    //Check if InitializeQueue succeed
    if (primes_q == NULL) {
        return ALLOC_FAILED;
    }

    //Get a queue of prime numbers and number of primes and their total digits
    get_prime_numbers(atoi(task), &primes_q, &primes_c, &digits_c);
    
    output_len = strlen("The prime factors of ") + strlen(task) + strlen(" are: ") + (2 * primes_c) + digits_c + 3;

    *write_buffer = (char*)malloc(output_len * sizeof(char));
    if (*write_buffer == NULL) {
        DestroyQueue(primes_q);
        return ALLOC_FAILED;
    }

    //Building the desiered output string
    sprintf_s(*write_buffer, output_len, "The prime factors of %s are: ", task);

    for (int i = 0; i < primes_c; i++)
    {
        sprintf_s(*write_buffer, output_len, (i == 0 ? "%s%d" : "%s, %d"), *write_buffer, Top(primes_q)->value);
        Pop(primes_q);
    }
    sprintf_s(*write_buffer, output_len, "%s\r\n", *write_buffer);
    DestroyQueue(primes_q);

    return SUCCESS;
}

void get_prime_numbers(int num, Queue** primes_q, int *primes_c, int *digits_c) {

    if (num < 2)
        return;
    while (num % 2 == 0)
    {
        (*primes_c)++;
        Push(*primes_q, 2);
        (*digits_c)++;
        num = num / 2; //reduce n by dividing this by 2
    }
    for (int i = 3; i <= sqrt(num); i = i + 2) { //i will increase by 2, to get only odd numbers
        while (num % i == 0)
        {
            Push(*primes_q, i);
            (*primes_c)++;
            *digits_c += count_digits(i);
            num = num / i;
        }
    }
    if (num > 2)
    {
        (*primes_c)++;
        Push(*primes_q, num);
        *digits_c += count_digits(num);
    }
}


//counting length per factor
int count_digits(int number)
{
    int num;
    int count = 0;
    num = number;
    /* Run loop till num is greater than 0 */
    while (num != 0)
    {
        /* Increment digit count */
        count++;

        /* Remove last digit of 'num' */
        num /= 10;
    }
    return count;
}
