#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <cmath>
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
    int nl,nh;
    string sync;
};

pthread_mutex_t  mut; 

float sum =0.0;


void *func1( void *ptr )
{
   struct thread_args *args = (struct thread_args *)ptr;

   if(args->sync=="iteration")
   {
    for(int i=args->nl;i<args->nh;i++)
    {
      pthread_mutex_lock (&mut);
      sum+=f1((args->a+(i+0.5)*args->q),args->intensity);
      pthread_mutex_unlock (&mut);
    }
   }

   else if(args->sync=="thread")
   { float temp=0.0;
    for(int i=args->nl;i<args->nh;i++)
    {
      //pthread_mutex_lock (&mut);
      temp+=f1((args->a+(i+0.5)*args->q),args->intensity);
      //pthread_mutex_unlock (&mut);
      //r=q*sum;
    }
    pthread_mutex_lock (&mut);
    sum+=temp;
    pthread_mutex_unlock (&mut);
   }
   //float r=args->q*sum;
   //std::cout<<r<<endl;
}

void *func2( void *ptr )
{
   struct thread_args *args = (struct thread_args *)ptr;

   if(args->sync=="iteration")
   {
    for(int i=args->nl;i<args->nh;i++)
    {
      pthread_mutex_lock (&mut);
      sum+=f2((args->a+(i+0.5)*args->q),args->intensity);
      pthread_mutex_unlock (&mut);
    }
   }

   else if(args->sync=="thread")
   { float temp=0.0;
    for(int i=args->nl;i<args->nh;i++)
    {
      //pthread_mutex_lock (&mut);
      temp+=f2((args->a+(i+0.5)*args->q),args->intensity);
      //pthread_mutex_unlock (&mut);
      //r=q*sum;
    }
    pthread_mutex_lock (&mut);
    sum+=temp;
    pthread_mutex_unlock (&mut);
   }
  // float r=args->q*sum;
   //std::cout<<r<<endl;
}

void *func3( void *ptr )
{
   struct thread_args *args = (struct thread_args *)ptr;

   if(args->sync=="iteration")
   {
    for(int i=args->nl;i<args->nh;i++)
    {
      pthread_mutex_lock (&mut);
      sum+=f3((args->a+(i+0.5)*args->q),args->intensity);
      pthread_mutex_unlock (&mut);
    }
   }

   else if(args->sync=="thread")
   { float temp=0.0;
    for(int i=args->nl;i<args->nh;i++)
    {
      //pthread_mutex_lock (&mut);
      temp+=f3((args->a+(i+0.5)*args->q),args->intensity);
      //pthread_mutex_unlock (&mut);
      //r=q*sum;
    }
    pthread_mutex_lock (&mut);
    sum+=temp;
    pthread_mutex_unlock (&mut);
   }
  // float r=args->q*sum;
   //std::cout<<r<<endl;
}

void *func4( void *ptr )
{
   struct thread_args *args = (struct thread_args *)ptr;

   if(args->sync=="iteration")
   {
    for(int i=args->nl;i<args->nh;i++)
    {
      pthread_mutex_lock (&mut);
      sum+=f4((args->a+(i+0.5)*args->q),args->intensity);
      pthread_mutex_unlock (&mut);
    }
   }

   else if(args->sync=="thread")
   { float temp=0.0;
    for(int i=args->nl;i<args->nh;i++)
    {
      //pthread_mutex_lock (&mut);
      temp+=f4((args->a+(i+0.5)*args->q),args->intensity);
      //pthread_mutex_unlock (&mut);
      //r=q*sum;
    }
    pthread_mutex_lock (&mut);
    sum+=temp;
    pthread_mutex_unlock (&mut);
   }
  // float r=args->q*sum;
  // std::cout<<r<<endl;
}
  
int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <sync>"<<std::endl;
    return -1;
  }


  int p = atoi(argv[1]);
  float a=atof(argv[2]);
  float b=atof(argv[3]);
  int n = atoi(argv[4]);
  float intensity=atoi(argv[5]);
  int nbthreads=atoi(argv[6]);
  string sync=argv[7];

  float q =(b-a)/n;

  pthread_t thread[nbthreads];

  pthread_mutex_init (&mut , NULL);

  struct thread_args var[nbthreads+1];

   void *(*fun_ptr_arr[])(void *) = {func1, func2,func3,func4}; 

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  if(n%nbthreads==0){
              for(int i=0;i<nbthreads;i++)
              {
                var[i].a=a; var[i].b=b; var[i].intensity=intensity; var[i].sync=sync; var[i].q=q;
                var[i].nl=i*n/nbthreads;var[i].nh=(i+1)*n/nbthreads;
                pthread_create( &thread[i], NULL,*fun_ptr_arr[p-1], (void *) &var[i]);
          }
        }
        else{ 
            int re=n%nbthreads;
            for(int i=0;i<=nbthreads;i++)
                {
                  if(i!=nbthreads){
                  var[i].a=a; var[i].b=b; var[i].intensity=intensity; var[i].sync=sync; var[i].q=q;
                  var[i].nl=i*n/nbthreads;var[i].nh=(i+1)*n/nbthreads;
                  pthread_create( &thread[i], NULL,*fun_ptr_arr[p-1], (void *) &var[i]);
                  }
                  else 
                  {
                    var[i].a=a; var[i].b=b; var[i].intensity=intensity; var[i].sync=sync; var[i].q=q;
                    var[i].nl=n-re;var[i].nh=n;
                    pthread_create( &thread[i], NULL,*fun_ptr_arr[p-1], (void *) &var[i]);
                  }
            }
  }

if(n%nbthreads==0){
  for(int i=0;i<nbthreads;i++)
  {
    pthread_join(thread[i], NULL);
  }
}

  
else 
{
  for(int i=0;i<=nbthreads;i++)
  {
      pthread_join(thread[i], NULL);
  } 
}

float r=q*sum;
 std::cout<<r<<endl;
pthread_mutex_destroy (&mut);

      
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;

        std::cerr<<elapsed_seconds.count()<<std::endl;

  
  return 0;
}
