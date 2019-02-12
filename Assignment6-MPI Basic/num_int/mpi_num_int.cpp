#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>

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

  
int main (int argc, char* argv[]) {
  
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

int p = atoi(argv[1]);
  float a=atof(argv[2]);
  float b=atof(argv[3]);
  int n = atoi(argv[4]);
int intensity=atoi(argv[5]);

float q=(b-a)/n;
float sum;
int rank,size;

float (*fun_ptr_arr[])(float,int) = {f1, f2,f3,f4};
  
std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

float temp;
  
  if(rank<size)
  {
   for(int i=(rank*n/size);i<((rank+1)*n/size);i++)
   temp+=(*fun_ptr_arr[p-1])((a+(i+0.5)*q),intensity);

  }

  if(n%size!=0 && rank==0)
  {
    for(int i=(size*n/size);i<n;i++)
    temp+=(*fun_ptr_arr[p-1])((a+(i+0.5)*q),intensity);
  }

MPI_Reduce(&temp, &sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

MPI_Finalize();

if(rank==0)
{
  
  float r=q*sum;
  std::cout<<r<<endl;

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  std::cerr<<elapsed_seconds.count()<<std::endl;
  
}
   
 
  return 0;
}

