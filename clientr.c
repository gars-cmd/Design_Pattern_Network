/*
 ** client.c -- a stream socket client demo
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT "3490" // the port client will be connecting to

#define MAXDATASIZE 1024 // max number of bytes we can get at once
int receive(int sockfd);
// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];

  if (argc != 2)
  {
    fprintf(stderr, "usage: client hostname\n");
    exit(1);
  }

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
  {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // loop through all the results and connect to the first we can
  for (p = servinfo; p != NULL; p = p->ai_next)
  {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1)
    {
      perror("client: socket");
      continue;
    }

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      // perror("client: connect");
      continue;
    }

    break;
  }

  if (p == NULL)
  {
    // fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
      s, sizeof s);
  printf("DEBUG:client: connecting to %s\n", s);

  freeaddrinfo(servinfo); // all done with this structure
  pthread_t threadC;
  send(sockfd, "new client connected", 20,0);
  while (1) {
  pthread_create(&threadC, NULL, receive, sockfd);
  // printf("\nDEBUG:enter a string for the server >");
  scanf("%[^\n]%*c",&buf);
  // printf("buf = %s\n", buf);
  if ((numbytes = send(sockfd, buf, 1024, 0)) == -1)
  {
    perror("send...");
    exit(1);
  }

  else
  {

    // printf("the data %s was sent\n",buf);
    fflush(stdout);
  }
  // memset(buf, 0, MAXDATASIZE);
  // if ((numbytes = recv(sockfd, buf, MAXDATASIZE, 0)) == -1)
  // {
  //   perror("ERROR:recv...");
  //   exit(1);
  // }
  // else
  // {
  //   buf[numbytes] = '\0';
  //   printf("OUTPUT:%s\n", buf);
  //   close(sockfd);
  // }
}
}


int receive(int sockfd){
  while (1) {
  char buf2[1024];
  memset(buf2 , 0 ,  MAXDATASIZE);
  int numbytes = recv(sockfd , buf2 , MAXDATASIZE , 0);
  if (numbytes == -1) {
    perror("receive");
    exit(1);
  }else {
    buf2[numbytes] = '\0';
    int size = strlen(buf2);
    if (size>0) {
      printf("\nOUTPUT:%s\n",buf2);
    }
  }
  }
}



