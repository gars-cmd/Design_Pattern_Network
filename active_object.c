#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "queue.h"
#include "active_object.h"

// initialize pthread_var

pthread_mutex_t mutex;
pthread_cond_t cond_next;

// create a new active object
void newAO(queue* my_queue, void (*start_func)(char* ), void (*end_func)(queue*,void* val) , void* next){

 AO* newone = malloc(sizeof(AO));
 newone->myqueue = create_queue();
 newone->start_func = start_func;
 newone->end_func = end_func;

 pthread_create(&newone->thread_id_t, NULL, &handler, newone);

}


 










//------------------------------UTILS FUNCTION-------------------------


void handler1(AO* ao){

  while (1) {
    while (ao->myqueue->head==NULL) {
      printf("the queue of AO1 is empty then -> wait...\n");
      pthread_cond_wait(&cond_next , &mutex);
    }
    ao->myqueue->head->value = Ccipher(ao->myqueue->head->value);
    void* ans = dequeue(ao->myqueue);
    ao->end_func()

}

}














  while(1){
    while (ao->myqueue->head==NULL){
    printf("thread AO1 wait for entering element in his queue\n");
    pthread_cond_wait(&cond_next , &mutex);
  }
  

  }


}




// function to transform a string according to the ceaser cipher
char* Ccipher(char *string){

    int i = 0;
    int size = strlen(string);
    char* newstring = malloc(sizeof(size));

    while (string[i] != '\0') {
    if (string[i] >= 'A' && string[i] <= 'Z') {
      char c = string[i] - 'A';
      c+=1;  
      c = c%26;
      newstring[i] = c + 'A';
    }
    else if (string[i] >= 'a' && string[i] <='z'){
      char c = string[i] - 'a';
      c+=1;
      c = c%26;
      newstring[i] = c + 'a';
    }
    else {
      char c = string[i];
      newstring[i]=c;
    }
    i++;
  }
 return newstring; 
}


// function to cast up-char to down-char and reverse  
char * up_DWN(char* string){

    int i = 0;
    int size = strlen(string);
    char* newstring = malloc(sizeof(size));
    
    while (string[i] != '\0') {
      char c;
      if (string[i] >= 'A' && string[i] <='Z') {
         c = string[i] + 32;
      }
      else if (string[i] >= 'a' && string[i] <='z'){
         c = string[i] - 32;
      }
      else{
        c = string[i];
      }
      newstring[i] = c;
      printf("for now the string is equal %s\n",newstring);
      i++;
    }
    return newstring;
  }




typedef struct AO{
    queue *myqueue;
    void (*start_func)(queue*);
    void (*end_func)(void);
    pthread_t thread_id_t;
}AO;

