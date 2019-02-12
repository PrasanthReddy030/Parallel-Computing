#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

#include <string.h>
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

float (*fun_ptr_arr[])(float,int) = {f1, f2,f3,f4};

int main (int argc, char* argv[]) {
  //forces openmp to create the threads beforehand
#pragma omp parallel
  {
    int fd = open (argv[0], O_RDONLY);
    if (fd != -1) {
      close (fd);
    }
    else {
      std::cerr<<"something is amiss"<<std::endl;
    }
  }
  
  if (argc < 9) {
    std::cerr<<"Usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  //insert code here
  int p = atoi(argv[1]);
  float a=atof(argv[2]);
  float b=atof(argv[3]);
  int n = atoi(argv[4]);
  float intensity=atoi(argv[5]);
  int nbthreads=atoi(argv[6]);
  int granularity = atoi(argv[8]);

  float q =(b-a)/n;
  float r;
  float sum=0;
   
  omp_sched_t type;

if(strcmp(argv[7],"static"))
    type=omp_sched_static ;
  else
    type=omp_sched_dynamic;
  
omp_set_schedule(type, granularity);    
omp_set_num_threads(nbthreads);

std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

#pragma omp parallel for reduction(+:sum) schedule(runtime)
  for (int i=0;i<=n-1;i++)
  sum+=(*fun_ptr_arr[p-1])((a+(i+0.5)*q),intensity);

         r=q*sum;
            
        std::cout<<r<<endl;
                
        std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;

        std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}
