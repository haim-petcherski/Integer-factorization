//Queue header

#ifndef __QUEUE_H__
#define __QUEUE_H__

//Includes:
#include <stdio.h> 
#include <stdlib.h> 
#include <stdbool.h>


//Typedefs:
// node in the  queue 
typedef struct QNode {
    int value;
    struct QNode* next;
}QNode;

// front stores the front node of the queue  and rear stores the last node
// by this method the queue will be implemented as a linked list
typedef struct Queue {
    QNode* front;
    QNode* rear;
}Queue;


// Creating new node as the rest nodes in the queue
// Receives a number to enter the node
// Returns a pointer to a queue node with the number
QNode* NewNode(int k);

//Initialize queue
//Receives nothing
//Returns a pointer to an empty queue
Queue* InitializeQueue();

//Top of the queue
//Receives a pointer to a queue
// Returns the a pointer to the first node in the queue
QNode* Top(Queue* q);

// Pushing new node the last place in the node
// Receives a pointer to the queue and a number to the new node that will be created
void Push(Queue* q, int value);

// Cheking if the queue is empty
// Receives a pointer to the queue
// Returns true if it is empty and false if it's not
bool Empty(Queue* q);

// Removing the front node in the queue
// Receives a pointer to the queue
void Pop(Queue* q);

// Empting the queue from all the nodes
// Receives a pointer to the queue
void DestroyQueue(Queue* q);

#endif // __QUEUE_H__