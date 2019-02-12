#include <omp.h>
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);

#ifdef __cplusplus
}
#endif

long int n;
void merge_sort(int *arr);
void merge(int *arr, long l, long m, long r);



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
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int * arr = new int [atoi(argv[1])];

  generateMergeSortData (arr, atoi(argv[1]));


  n = atol (argv[1]);
  int p= atoi(argv[2]);
  

  omp_set_num_threads (p);

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  merge_sort (arr);
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  std::cerr<<elapsed_seconds.count()<<std::endl;

  checkMergeSortResult (arr, atoi(argv[1]));

  delete[] arr;

  return 0;
}

void merge_sort(int *arr)
{
     for (long i=1; i<=n-1; i = 2*i)
      {
          #pragma omp parallel for schedule(static)
          for (long j=0; j<n-1; j += 2*i)
          {
             long r = j+(2*i)-1 < n-1? j+(2*i)-1:n-1;
             long m = j+i-1< n-1?j+i-1:n-1;

             merge(arr, j, m, r);
            }

        } 
}


void merge(int *arr, long l, long m, long r)
{
  long n1 = m - l + 1, n2 = r - m;
  int L[n1], R[n2];
  for (long i = 0; i < n1; i++)
  {
    L[i] = arr[l + i];
  }
  for (long j = 0; j < n2; j++)
  {
    R[j] = arr[m + 1+ j];
  }

  long i = 0, j = 0, k = l;

  while (i < n1 && j < n2)
  {
    if (L[i] <= R[j])
    {
      arr[k] = L[i]; k++; i++;
    }
    else
    {
      arr[k] = R[j]; k++; j++;
    }

  }

  if(j == n2) 
  {
    while (i < n1)
    {
      arr[k] = L[i]; k++; i++;
    }
  }
  else if(i == n1) 
  {
    while (j < n2)
    {
      arr[k] = R[j]; k++; j++;
    }
  }
  return;
}


