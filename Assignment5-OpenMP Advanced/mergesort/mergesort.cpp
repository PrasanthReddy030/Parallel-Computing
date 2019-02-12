#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <omp.h>

using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif
int n,nbthreads;

void merge(int * arr, int l, int mid, int r) {

  if (l == r) return;
  if (r-l == 1) {
    if (arr[l] > arr[r]) {
      int temp = arr[l];
      arr[l] = arr[r];
      arr[r] = temp;
    }
    return;
  }

  int i, j, k;
  int n = mid - l;
  

  int *temp = new int[n];
  for (i=0; i<n; ++i)
    temp[i] = arr[l+i];

  i = 0;    
  j = mid;  
  k = l;    

  
  while (i<n && j<=r) {
     if (temp[i] <= arr[j] ) {
       arr[k++] = temp[i++];
     } else {
       arr[k++] = arr[j++];
     }
  }

 
  while (i<n) {
    arr[k++] = temp[i++];
  }

  delete[] temp;

}

void mergesort(int * arr, int l, int r) {

  
  if(r-l+1<=n/(nbthreads)){
      if (l < r) {
        int mid = (l+r)/2;
        mergesort(arr, l, mid);
        mergesort(arr, mid+1, r);
        merge(arr, l, mid+1, r);
      }
      return ;
  }

  else{
    int mid = (l+r)/2;
    #pragma omp task
    mergesort(arr, l, mid);
    #pragma omp task
    mergesort(arr, mid+1, r);
    #pragma omp taskwait
    merge(arr, l, mid+1, r);
  }
  

}


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
  
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  n = atoi(argv[1]);
  nbthreads=atoi(argv[2]);
  int * arr = new int [n];

  generateMergeSortData (arr, n);
  
  //insert sorting code here.
  omp_set_num_threads(nbthreads);
  std::chrono::time_point<std::chrono::system_clock> starttime = std::chrono::system_clock::now();

  #pragma omp parallel
  {
     #pragma omp single
     mergesort(arr, 0, n-1);
  }

    std::chrono::time_point<std::chrono::system_clock> endtime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endtime-starttime;
    cerr<<elapsed_seconds.count()<<endl; 

   checkMergeSortResult (arr, n);
  
  delete[] arr;

  return 0;
}
