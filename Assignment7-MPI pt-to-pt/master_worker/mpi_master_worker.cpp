#include <mpi.h>
#include <iostream>
#include <chrono>

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

int done=1;
int n;
int inc=0;
int granularity=1000;
int c=0;

void getNext(int* begin,int* end,int* status){
  if(inc>=n){
    done=0;
  }
  if(done==1 && inc+granularity<=n){
        *begin=inc;
    *end=inc+granularity;
    inc=inc+granularity;
    *status=done;
    c++;
  }
  else{
    *begin=inc;
    *end=n;
    inc=n;
    *status=done;
    if(done==1)
      c++;
  }
}

float (*func_ptr_arr[])(float,int)={f1,f2,f3,f4};

int main (int argc, char* argv[]) {

  if (argc < 6) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  MPI_Init(&argc,&argv);
  int p=atoi(argv[1]);
  float a=atof(argv[2]);
  float b=atof(argv[3]);
  n=atoi(argv[4]);
  int intensity=atoi(argv[5]);
  
  float z=(b-a)/n;
  int rank,size;
  float sum=0;
  int data[3];
  float psum=0;
  
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  if(rank==0){
    MPI_Status status;
    if(n/granularity<size-1){
      for(int i=1;i<size;i++){
        getNext(&data[0],&data[1],&data[2]);
        MPI_Send(data,3,MPI_INT,i,0,MPI_COMM_WORLD);
      }
      data[2]=0;
      for(int i=0;i<c;i++){
            MPI_Recv(&psum,1,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            sum+=psum;
            MPI_Send(data,3,MPI_INT,status.MPI_SOURCE,0,MPI_COMM_WORLD);
      }
      cout<<z*sum<<endl;
    }
    else{
      for(int i=1;i<size;i++){
        getNext(&data[0],&data[1],&data[2]);
        MPI_Send(data,3,MPI_INT,i,0,MPI_COMM_WORLD);
      }

      int count=size-1;

      while(count){
        MPI_Recv(&psum,1,MPI_FLOAT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
        count=count-1;
        sum+=psum;
        getNext(&data[0],&data[1],&data[2]);
        if(data[2]!=0){
          count+=1;
        }
        MPI_Send(data,3,MPI_INT,status.MPI_SOURCE,0,MPI_COMM_WORLD);
      }
      cout<<z*sum<<endl;
    }
  }


  else{
    float temp=0;
    MPI_Status status;
    while(1){
      MPI_Recv(data,3,MPI_INT,0,0,MPI_COMM_WORLD,&status);
      if(data[2]==0){
        break;
      }
      for(int i=data[0];i<data[1];i++){
        temp+=(*func_ptr_arr[p-1])((a+(i+.5)*(z)),intensity);
      }
    MPI_Send(&temp,1,MPI_FLOAT,0,0,MPI_COMM_WORLD);
    temp=0;
    }

  }

  MPI_Finalize();

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;

  if(rank==0)
  std::cerr<<elapsed_seconds.count()<<std::endl;

  return 0;
}
