#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include<string>
#include <chrono>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif
  void generateReduceData (int* arr, size_t n);
#ifdef __cplusplus
}
#endif


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
  
  if (argc < 5) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads> <scheduling> <granularity>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int * arr = new int [n];
  int granularity=atoi(argv[4]);

  omp_sched_t type;
  float sum=0;

  if(strcmp(argv[3],"static"))
  	type=omp_sched_static ;
  else
  	type=omp_sched_dynamic;

  generateReduceData (arr, atoi(argv[1]));

  //insert reduction code here

omp_set_schedule(type, granularity);   
omp_set_num_threads(atoi(argv[2]));

std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

#pragma omp parallel for reduction(+:sum) schedule(runtime)
	 
  for(int i=0;i<n;i++){
  	sum+=arr[i];
  }
  
std::cout<<sum<<endl;
  delete[] arr;

   std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;

        std::cerr<<elapsed_seconds.count()<<std::endl;


  return 0;
}
