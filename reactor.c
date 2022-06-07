#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "3490"
char buf[1024];


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa){
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


typedef int (*foo_ptr_t)(int);

typedef struct Reactor{
  // file descriptor arr 
  fd_set file_desc_arr;
  foo_ptr_t funcs[1024];
  int size_fd;
  pthread_t thread;
}Reactor;

void RemoveHandler(Reactor* reactor , int fd);
int acceptX(int fd);
int recvX(int fd);
int sendX(int fd);
void InstallHandler(Reactor* reactor , int (*foo)(int) ,   int file_desc );

void reactor_handler(Reactor* reactor){

  printf("DEBUG:start of the reactor_handler\n");

  fd_set origin,tmp;

  FD_ZERO(&origin);
  // FD_ZERO(&tmp);

  reactor->file_desc_arr = origin;
  int listener;
  int newfd;
  struct sockaddr_storage remoteaddr;
  socklen_t addrlen;


  int nbytes;

  char remoteIP[INET6_ADDRSTRLEN];

  int yes=1;
  int i,j,rv;

  struct addrinfo hints, *ai , *p;


  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, PORT, &hints , &ai)) != 0) {
    fprintf(stderr, "Reactor_server : %s\n",gai_strerror(rv));
    exit(1);
  }

  for(p = ai; p!=NULL; p = p->ai_next){
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0 ) {
      close(listener);
      continue;
    }
    break;
  }
  if (p == NULL) {
    fprintf(stderr, "Reactor_server: failed to bind\n");
    exit(2);
  }

  freeaddrinfo(ai);

  if (listen(listener, 10) == -1) {
    perror("listen");
    exit(3);
  }

  FD_SET(listener, &origin);
  reactor->size_fd = listener;

  while (1) {
    tmp = origin; // copy it
    if (select(reactor->size_fd+1, &tmp, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(4);
    }
    for (int i = 0 ; i<=reactor->size_fd; i++) {
      // printf("iterate to %d\n",i);
      if (FD_ISSET(i, &tmp)) {
        // printf("see some acrivities from %d\n",i);
        if (i == listener) {
          printf("listener = %d\n",i);
          InstallHandler(reactor, acceptX, i);
          // acceptX(listener);
          newfd = reactor->funcs[i](listener); 
          if (newfd == -1) {
            perror("acceptX");
          }else {
            FD_SET(newfd, &origin);
            if (newfd > reactor->size_fd) {
              reactor->size_fd = newfd;
              // printf("new size_fd = %d\n",reactor->size_fd);
            }
            printf("Reactor_server get a new connection \n");
          }
        } else {
          // printf("case where we see activities from client %d \n",i);
          InstallHandler(reactor,recvX,i);
          nbytes = reactor->funcs[i](i);
          if (nbytes <=0) {
            if (nbytes == 0) {
              printf("DEBUG:Reactor_server: socket %d hung up\n",i);
            }else {
              perror("recv");
            }
            printf("DEBUG:we close the client\n");
            close(i);
            RemoveHandler(reactor, i);
            
          }else {
            // printf("send part initialized\n");
            InstallHandler(reactor, sendX,i);
            for (int j = 0; j <= reactor->size_fd; j++) {
              // printf("-%d-\n",j);
              if (FD_ISSET(j, &reactor->file_desc_arr)) {
                if (j!=listener && j !=i) {
                  // InstallHandler(reactor, sendX,i);
                  int test = reactor->funcs[i](j);
                  // int test = send(j, buf, sizeof(buf), 0);
                  if (test == -1) {
                    perror("send");
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}


void InstallHandler(Reactor* reactor , int (*foo)(int) ,   int file_desc ){
  FD_SET(file_desc, &reactor->file_desc_arr);
  // printf("add new fd to the list\n");
  reactor->funcs[file_desc]=foo;
  // printf("add new function to the list\n");
}



Reactor* newReactor() {
  Reactor* newOne =  malloc(sizeof(Reactor));
  if (newOne == 0) {
    perror("error in creation of reactor");
  }else {
    // printf("let's go\n");
  }
  newOne->size_fd = 0;
  *newOne->funcs = calloc(1, sizeof(foo_ptr_t));
  // pthread_create(&newOne->thread,  NULL, reactor_handler, newOne);
  return newOne;
}


int acceptX(int fd){
  // printf("accept command initialized\n");
  socklen_t addrlen;
  struct sockaddr_storage remoteaddr;
  addrlen = sizeof(remoteaddr);
  int newfd = accept(fd, (struct sockaddr *)&remoteaddr,&addrlen); 
  // printf("comand accpetX done\n");
  return newfd;
}

int recvX(int fd){
  // printf("recv command initialized\n");
  int nbytes = recv(fd, buf, sizeof(buf), 0);
  // printf("the command recv done and the data %s was received\n",buf);
  return nbytes;
} 

int sendX(int fd){
  // printf("send command initialized\n");
  int nbytes = send(fd, buf, sizeof(buf), 0);
  // printf("data was sent to %d client\n",fd);
  return nbytes;
}

void RemoveHandler(Reactor* reactor , int fd){
  FD_CLR(fd, &reactor->file_desc_arr);
  // printf("the file_desc at place %d was removed from the list\n",fd);
  reactor->funcs[fd]=NULL;
  // printf("the functions at place %d has been set to NULL\n",fd);
}


int main (int argc, char *argv[])
{
  Reactor* Ariane = newReactor();
  // printf("the fusee Ariane was created\n");

  int p_problem = pthread_create(&Ariane->thread,  NULL, reactor_handler, Ariane);
  if (p_problem != 0 ) {
    perror("thread_creation problem");
  }
  printf("DEBUG:the thread was launched\n");

  while (1) {

  }

  return 0;
} 




