#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

struct thread_args
 {
    float a,b,q;
    int intensity;
    string sync;
    int p;
};

int flag=1 ;
int inc=0,granularity,n;
pthread_mutex_t  mut, mut1; 

int loopdone()
{
  return flag;
}

void getnext(int* begin , int* end)
{
  if(inc==n)
  {
    flag=0;
  }
  
  if(inc+granularity>n)
  {
    *begin=inc;
    *end=n;
    inc=n;

    return;
  }

  if(flag!=0 && inc+granularity<=n)
  {
  *begin=inc;
  *end=inc+granularity;
  inc+=granularity;
  }

}

float sum =0.0;

float (*fun_ptr_arr[])(float,int) = {f1, f2,f3,f4};

void *Iteration (void *ptr)
{
  struct thread_args *args = (struct thread_args *)ptr;

  while(loopdone()){
        int begin=0, end=0;
        pthread_mutex_lock (&mut1);
        getnext(&begin,&end);
        pthread_mutex_unlock (&mut1);
          for(int i=begin;i<end;i++)
          {
            pthread_mutex_lock (&mut);
            sum+=(*fun_ptr_arr[args->p-1])((args->a+(i+0.5)*args->q),args->intensity);
            pthread_mutex_unlock (&mut);
          }
  }
}

void *Thread (void *ptr)
{
  struct thread_args *args = (struct thread_args *)ptr;

  float temp=0.0;
  while(loopdone()){
          int begin=0, end=0;
          pthread_mutex_lock (&mut1);
          getnext(&begin,&end);
          pthread_mutex_unlock (&mut1);
      for(int i=begin;i<end;i++)
      {
        temp+=(*fun_ptr_arr[args->p-1])((args->a+(i+0.5)*args->q),args->intensity);
      }
  }
      pthread_mutex_lock (&mut);
      sum+=temp;
      pthread_mutex_unlock (&mut);
}

void *Chunk (void* ptr)
{
  struct thread_args *args = (struct thread_args *)ptr;

  float temp;
      while(loopdone()){
      int begin=0, end=0;
        pthread_mutex_lock (&mut1);
        getnext(&begin,&end);
        pthread_mutex_unlock (&mut1);
      temp=0.0;
      for(int i=begin;i<end;i++)
      {
        temp+=(*fun_ptr_arr[args->p-1])((args->a+(i+0.5)*args->q),args->intensity);
      }
      pthread_mutex_lock (&mut);
      sum+=temp;
      pthread_mutex_unlock (&mut);
    }
}


int main (int argc, char* argv[]) {

  if (argc < 9) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync> <granularity>"<<std::endl;
    return -1;
  }

  int p = atoi(argv[1]);
  float a=atof(argv[2]);
  float b=atof(argv[3]);
  n = atoi(argv[4]);
  float intensity=atoi(argv[5]);
  int nbthreads=atoi(argv[6]);
  string sync=argv[7];
  granularity = atoi(argv[8]);

  float q =(b-a)/n;
  
  pthread_t thread[nbthreads];
  pthread_mutex_init (&mut , NULL);
  pthread_mutex_init (&mut1 , NULL);

  struct thread_args var;
  var.a=a; var.b=b; var.intensity=intensity; var.sync=sync; var.q=q; var.p=p;

std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

if(sync=="iteration")
{
  for(int i=0;i<nbthreads;i++)
  {
   pthread_create( &thread[i], NULL,Iteration, (void *) &var);               
  }
}

else if(sync=="thread")
{
  for(int i=0;i<nbthreads;i++)
  {
   pthread_create( &thread[i], NULL,Thread, (void *) &var);               
  }
}

else
{
  for(int i=0;i<nbthreads;i++)
  {
   pthread_create( &thread[i], NULL,Chunk, (void *) &var);               
  }
}


	for(int i=0;i<nbthreads;i++)
	{
	  pthread_join(thread[i], NULL);
	}

float r=q*sum;
std::cout<<r<<endl;
pthread_mutex_destroy (&mut);
pthread_mutex_destroy (&mut1);

        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;

        std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}
