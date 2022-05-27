#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef QUEUE
#define QUEUE




#define QUEUE_EMPTY INT_MIN


typedef struct node {
	void* value;
	struct node *next; 
}node;

typedef struct queue{
	int queue_size;
	node *head;
	node *tail;
}queue;

queue* create_queue();
void enqueue(queue *myqueue , void* value);
void* queue_pick(queue *myqueue);
void* dequeue (queue *myqueue);
void clear_queue(queue *myqueue);



#endif // !QUEUE
