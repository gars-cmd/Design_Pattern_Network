#include <cstddef>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <iostream>


pthread_mutex_t mutex;

template<typename T>
class Singleton{
  public:
    static Singleton *Instance(T temp);
    static void destroy();
  private:
    static Singleton * single;
    Singleton(T temp);
    T priv;


};

  template<typename T>
Singleton<T>::Singleton(T temp)
{
  priv = temp;
}

template<typename T>
Singleton<T> *Singleton<T>::single = NULL;

template<typename T>
Singleton<T> *Singleton<T>::Instance(T temp){
  if (single == NULL)
  {
    pthread_mutex_lock(&mutex);
    if (single == NULL) {
      single = new Singleton(temp);
    }
    pthread_mutex_unlock(&mutex);
  }
  return single;
}

template<typename T>
void Singleton<T>::destroy(){
  delete(single);
  single = nullptr;
}

int main (int argc, char *argv[])
{
  FILE *file;
  Singleton<FILE*> *f1 = Singleton<FILE*>::Instance(file);
  Singleton<FILE*> *f2 = Singleton<FILE*>::Instance(file);
  std::cout << "the first file was create" << f1 << std::endl;
  std::cout << "the second file was create" << f2 << std::endl;

  if (f1 == f2)
  {
    std::cout << "SUCCESS-same address" << std::endl;
  }
  else
  {
    std::cout << "FAIL-different address" << std::endl;
  }
  return 0;
}
