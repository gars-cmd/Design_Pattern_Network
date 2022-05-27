#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "queue.h"
#include <unistd.h>                                     
#include <errno.h>                                      
#include <string.h>                                     
#include <sys/types.h>                                  
#include <sys/socket.h>                                 
#include <netinet/in.h>                                 
#include <netdb.h>                                      
#include <arpa/inet.h>  
#include <signal.h>
// initialize var

pthread_mutex_t mutex;
pthread_cond_t cond_next1;
pthread_cond_t cond_next2;
pthread_cond_t cond_next3;


queue* q1;
queue* q2;
queue* q3;

#define PORT "3490" 
#define BACKLOG 10 
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

typedef struct AO {
  queue* myqueue;
  char*(*start_func)(char*);
  void(*end_func)(queue* , void*);
  pthread_t th_t;
}AO;


typedef struct packet {
  char* string;
  int* socket;
}packet;


AO* newAO(queue* myqueue , char* (*start_func) (char*), void (*end_func) (queue*,void*)  ){

  AO* ao = malloc(sizeof(AO));
  ao->myqueue = myqueue;
  ao->start_func = start_func;
  ao->end_func = end_func;

  return ao;

}


void destroyAO(AO* ao){
  clear_queue(ao->myqueue);
  pthread_cancel(ao->th_t);
  free(ao);
}


// function to transform a string according to the ceaser cipher
char* Ccipher(char *string){
  printf("cipher was called\n");

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
  printf("after cipher is %s\n",newstring);
  return newstring; 
}


// function to cast up-char to down-char and reverse  
char * up_DWN(char* string){

  printf("upDwn was called\n");
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






void handler_connect(int *client){
  // pthread_mutex_lock(&mutex);
  printf("new connection to handle\n");

  char buff[1024];
  memset(buff, 0, 1024);
  int socket = *client;
  int recept = recv(socket , buff , 1024 , 0);
  printf("recept = %d\n",recept);
  if (recept == -1) {
    perror("error occur when recv\n");
    EXIT_FAILURE;
  }
  buff[recept] = '\0';
  printf("the data to enqueue = %s \n",buff);
  packet* pkt = malloc(sizeof(packet));
  pkt->string = calloc(sizeof(buff) , 1);
  pkt->string = buff;
  pkt->socket = &socket;
  enqueue(q1, pkt);
  printf("the data %s was enqueue in q1\n",buff);
  pthread_cond_signal(&cond_next1);
  // pthread_mutex_unlock(&mutex);
}


typedef struct pair{
  int index;
  AO* ao;
  queue* next_queue;
}pair;



void handler (pair * pair_n){
  while (1) {
    while (pair_n->index == 1 && q1->head == NULL) {
      printf("the queue q%d is locked\n",pair_n->index);
      pthread_cond_wait(&cond_next1 , &mutex);
    }
    while (pair_n->index == 2 && q2->head == NULL) {
      printf("the queue q%d is locked\n",pair_n->index);
      pthread_cond_wait(&cond_next2 , &mutex);
    } 
    while (pair_n->index == 3 && q3->head == NULL) {
      printf("the queue q%d is locked\n",pair_n->index);
      pthread_cond_wait(&cond_next3 , &mutex);
    }
    printf("the AO that running is %d\n",pair_n->index);
    if (pair_n->index == 3){
      printf("case of last AO\n");
      char* ans = ((packet*)pair_n->ao->myqueue->head->value)->string;
      int client = *((packet*)pair_n->ao->myqueue->head->value)->socket; 

      int tmp = send(client , ans , sizeof(ans) ,0) ;
      if (tmp == -1) {
        perror("error occur on send ");
      }
      else{
        free(pair_n->ao->myqueue->head->value);
        printf("the send succeed \n");
        dequeue(pair_n->ao->myqueue);
      }
    }
    else{

      printf("the active object %d is running\n",pair_n->index);
      // extract the  string from the packet in the node of the queue   
      char * string = ((packet*)pair_n->ao->myqueue->head->value)->string; 
      printf("the data in head is %s \n",string);
      char * ans = pair_n->ao->start_func(string);

      // prepare the next packet to forward to q_n+1
      packet* for_pakt = malloc(sizeof(packet));
      for_pakt->string = ans;
      for_pakt->socket = ((packet*)pair_n->ao->myqueue->head->value)->socket;

      // enqueue the new packet to the next queue 
      pair_n->ao->end_func(pair_n->next_queue , for_pakt);
      char* check = ((packet*)pair_n->next_queue->head->value)->string; 
      printf("the head of the next queue is now %s \n",check);

      free(pair_n->ao->myqueue->head->value);
      dequeue(pair_n->ao->myqueue);
      if (pair_n->index == 1) {
        pthread_cond_signal(&cond_next2);
        printf("free2\n");
      }else {
        pthread_cond_signal(&cond_next3);
        printf("free3\n");
      } 
    }
  }
}



int main (int argc, char *argv[])
{


  //---------------------------------------- initialized part---------------------- 

  pthread_mutex_init(&mutex, NULL);

  q1 = create_queue();
  q2 = create_queue();
  q3 = create_queue();

  AO* ao1 = newAO(q1, Ccipher, enqueue); 
  AO* ao2 = newAO(q2, up_DWN, enqueue);
  AO* ao3 = newAO(q3, NULL,NULL );

  pair* pair1 = malloc(sizeof(pair));
  pair1->ao = ao1;
  pair1->next_queue = q2;
  pair1->index = 1;

  pair* pair2 = malloc(sizeof(pair));
  pair2->ao = ao2;
  pair2->next_queue = q3;
  pair2->index = 2;

  pair* pair3 = malloc(sizeof(pair));
  pair3->ao = ao3;
  pair3->next_queue = NULL;
  pair3->index = 3;




  pthread_create(&ao1->th_t, NULL, handler,pair1);
  pthread_create(&ao2->th_t, NULL, handler,pair2);
  pthread_create(&ao3->th_t, NULL, handler,pair3);

  //---------------------------------------- server part---------------------------

  int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // use my IP

  if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and bind to the first we can
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
          sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo); // all done with this structure

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while(1) {  // main accept() loop
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    int pt;
    pthread_t thread;

    pt = pthread_create(&thread, NULL, handler_connect, &new_fd);

    if (pt != 0){
      perror("thread...");
      return 1;
    }
    inet_ntop(their_addr.ss_family,
        get_in_addr((struct sockaddr *)&their_addr),
        s, sizeof s);
    printf("server: got connection from %s\n", s);
  }


}






