#include <cstddef>
#include <system_error>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <iostream>


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


class guardian{
  public:
    guardian(){
      pthread_mutex_lock(&mutex);
    }
    ~guardian(){
      pthread_mutex_unlock(&mutex);
    }
};




void* GuarMod(void* index)
{
  guardian guard;
  std::cout << "guard-> working" << std::endl;
  sleep(3);
  std::cout << "guard -> resting" << std::endl;
  std::cout <<index<< std::endl;
  
  return (int*)1; 
}


int main (int argc, char *argv[])
{
  int index = 0;
  pthread_t t1 , t2;
  int p_val = pthread_create(&t1, NULL, &GuarMod, &index);
  if (p_val != 0)
  {
    perror("an error occur on thread1\n"); 
    return 1;
  }
  p_val = pthread_create(&t2, NULL, &GuarMod, &index);
  if (p_val != 0)
  {
    perror("an error occur on thread2 \n");
    return 1;
  }

  pthread_join(t1, NULL); 
  pthread_join(t2, NULL);   

  return 0;
}
