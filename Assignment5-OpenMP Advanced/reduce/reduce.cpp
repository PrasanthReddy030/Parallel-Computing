#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

using namespace std;


#ifdef __cplusplus
extern "C" {
#endif

  void generateReduceData (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

int sum=0;
int n;
void maintask(int *, int,int );
int reduce(int * , int,int);
int psum2(int *, int, int);



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
  
  if (argc < 3) {
    std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

   n = atoi(argv[1]);
  int * arr = new int [n];
  int nbthreads=atoi(argv[2]);

  generateReduceData (arr, atoi(argv[1]));

  //insert reduction code here
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  omp_set_num_threads(nbthreads);
  #pragma omp parallel
  {
        #pragma omp single
        maintask(arr, n, omp_get_num_threads());
        
  }
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::cout<<sum<<endl;
  delete[] arr;
        std::chrono::duration<double> elapsed_seconds = end-start;

       std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}

  int psum2(int *arr, int l, int r)
    { 
      int psum=0;
      for(int i=l; i<=r;i++)
        psum+=arr[i];
      //printf("%d\n",psum );
      return psum;
    }


  int reduce(int *arr,int l,int r)
  {
    int a=0,b=0,temp=0;

    if(r-l+1<=n/omp_get_num_threads())
    {
      temp=psum2(arr, l,r);
      return temp;
    }

      #pragma omp task shared(a)
      a=reduce(arr,l,(l+r)/2);

      #pragma omp task shared(b)
      b= reduce(arr,((l+r)/2)+1,r);

      #pragma omp taskwait

      return a+b;
  }


void maintask(int * arr,int n,int nbthreads){

  sum=reduce (arr,0,n-1);
    
}


