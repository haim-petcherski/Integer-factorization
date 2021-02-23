//Thread header

#ifndef __THREAD_H__
#define __THREAD_H__

//Includes:
#include <math.h>
#include "HardCodedData.h"
#include "main.h"
#include "Queue.h"
#include "Lock.h"


//Function Handles:
//My thread function
//Working on tasks until all done
//Geting the highest priority from the queue in an exclusive access managed by the lock
//Reading each task, calculating it's primes and writing to the end of the file the output
//Read and write access to the file is managed by the lock
//Recieves a pointer to the thread arguments containing the shared queue and lock and the file name
//Retruns a DWORD for debuging
DWORD WINAPI MyThreadFunction(LPVOID lpParam);

//Calculate the output
//Gets the task in a string format, decomposes it to the prime elements and updates the write_buffer in the expected format
//Returns status enum for debuging
Status calc_output(char** write_buffer, char* task);

//Get prime numbers
//Decomposes the task number to its prime factors and saves them to a queue
//Also updates counters for number of prime factors and the their total digits
void get_prime_numbers(int num, Queue** primes_q, int* primes_c, int* digits_c);

//Count digis
//Gets a number and returns the number of digits it contains
int count_digits(int number);

#endif // __THREAD_H__
