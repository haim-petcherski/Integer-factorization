//Queue module
//Containing the functions that implement a linked list queue

#include "Queue.h" 


// Creating new node as the rest nodes in the queue
QNode* NewNode(int num)
{
	QNode* temp = (QNode*)malloc(sizeof(QNode));
	if (temp == NULL)
		return NULL;
	temp->value = num;
	temp->next = NULL;
	return temp;
}



// Initialize queue 
Queue* InitializeQueue()
{
	Queue* q = (Queue*)malloc(sizeof(Queue));
	if (q == NULL)
		return NULL;
	q->rear = NULL;
	q->front = q->rear;
	return q;
}

// Top of the queue
QNode* Top(Queue* q)
{
	return q->front;
}

// Pushing new node the last place in the node
void Push(Queue* q, int value)
{
	// Create a new LL node 
	QNode* temp = NewNode(value);
	if (temp == NULL)
	{
		DestroyQueue(q);
		return;
	}
	// If queue is empty, then new node is front and rear both 
	if (q->rear == NULL) {
		q->front = temp;
		q->rear = temp;
		return;
	}

	// Add the new node at the end of queue and change rear 
	q->rear->next = temp;
	q->rear = temp;
}

// Cheking if the queue is empty
bool Empty(Queue* q)
{
	if (q->front == NULL)
		return true;
	return false;
}

// Removing the front node in the queue
void Pop(Queue* q)
{
	// If queue is empty, return NULL. 
	if (q->front == NULL)
		return;
	// Store previous front and move front one node ahead 
	QNode* temp = q->front;

	q->front = q->front->next;

	// If front becomes NULL, then change rear also as NULL 
	if (q->front == NULL)
		q->rear = NULL;

	free(temp);
}

// Empting the queue from all the nodes
void DestroyQueue(Queue* q)
{
	while (!Empty(q))
	{
		Pop(q);
	}
	q = NULL;
}