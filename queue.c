// #include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include "queue.h"

pthread_mutex_t mutex_q;
pthread_cond_t cond_q;
#define QUEUE_EMPTY INT_MIN

/* The function to create the queue */
queue* create_queue()
{
      queue* myqueue = malloc(sizeof(queue));
      myqueue->queue_size = 0;	
      myqueue->head = NULL;
      myqueue->tail = NULL;
      return myqueue;
}


/* the function to add an element to the queue */ 
void enqueue(queue *myqueue , void* value) 
{
      pthread_mutex_lock(&mutex_q);
      //check for empty queue
      if (myqueue->head == NULL) {
              myqueue->head = malloc(sizeof(node));
              myqueue->head->value = value;
              myqueue->tail = myqueue->head;
      }
      else {
      myqueue->tail->next = malloc(sizeof(node));
      myqueue->tail = myqueue->tail->next;
      myqueue->tail->value = value;
      }

      myqueue->tail->next = NULL;
      myqueue->queue_size++;

      pthread_mutex_unlock(&mutex_q);
      pthread_cond_signal(&cond_q);
      printf("a val was enqueued\n");
}

/* return the element at the top of the queue */
void* queue_pick(queue *myqueue)
{
      if (myqueue->head == NULL)
      {
              printf("DEBUG:the queue is empty \n");
              return NULL;
      }

      return myqueue->head->value;
}

/* delete the first element of the queue and return it */
void* dequeue (queue *myqueue)
{
      pthread_mutex_lock(&mutex_q);
      /* check for empty queue */
              while (myqueue->head == NULL) {
                      printf("waiting for insertion ...");
                      pthread_cond_wait(&cond_q, &mutex_q);	
              }

      node *tmp = myqueue->head;
      void* res = tmp->value;
      myqueue->head = myqueue->head->next;

      /* if there is one element on the queue , the queue is now empty */
      if (myqueue->head == NULL) {
              myqueue->tail = NULL;
              printf("DEBUG: the queue is now empty \n");
      }
      free(tmp);
      myqueue->queue_size--;
      pthread_mutex_unlock(&mutex_q);
  return res;
}

void clear_queue(queue *myqueue)
{
      node *tmp = myqueue->head;
      while (myqueue->head != NULL) {
              dequeue(myqueue);
      }
      myqueue->queue_size = 0;
      myqueue->head = myqueue->tail = NULL;
}




