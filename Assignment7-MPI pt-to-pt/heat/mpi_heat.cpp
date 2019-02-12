#include <mpi.h>
#include <math.h>
#include <iostream>
#include <chrono>

using namespace std;
#ifdef __cplusplus
extern "C" {
#endif

  int check2DHeat(double** H, long n, long rank, long P, long k); //this assumes array of array and grid block decomposition

#ifdef __cplusplus
}
#endif
/***********************************************
 *         NOTES on check2DHeat.
 ***********************************************
 *         
 *  First of, I apologize its wonky. 
 *
 *  Email me ktibbett@uncc.edu with any issues/concerns with this. Dr. Saule or the other
 *    TA's are not familiar with how it works. 
 *
 * Params:
 *  n - is the same N from the command line, NOT the process's part of N
 *  P - the total amount of processes ie what MPI_Comm_size gives you.
 *  k - assumes n/2 > k-1 , otherwise may return false negatives.
 *
 *   
 * Disclaimer:
 ***
 *** Broken for P is 9. Gives false negatives, for me it was always
 ***  ranks 0, 3, 6. I have not found issues with 1, 4, or 16, and these
/bin/bash: wq: command not found
 ***
 *
 * Usage:
 *  When code is WRONG returns TRUE. Short example below
 *  if (check2DHeat(...)) {
 *    // oh no it is (maybe) wrong  
 *    std::cout<<"rank: "<<rank<<" is incorrect"<<std::endl;
 *  }
 *
 *
 *
 *  I suggest commenting this out when running the bench
 *
 *
 * - Kyle
 *
 *************/

// Use similarily as the genA, genx from matmult assignment.

double genH0(long row, long col, long n) {
  double val = (double)(col == (n/2));
  return val;
}
long row1,column1,submatrixsize,submatrix_row_begin,submatrix_row_end,submatrix_column_begin,submatrix_column_end;

void heat(double**current, double**next, double*up,double*down,double*left,double*right){
    for(int i=0;i<submatrixsize;i++){
      for(int j=0;j<submatrixsize;j++){
        if(i==0 && j==0){
          next[i][j]=(1/static_cast<double>(5))*(current[i+1][j]+current[i][j+1]+current[i][j]+left[i]+up[j]);
        }
        else if(i==submatrixsize-1 && j==0){
          next[i][j]=(1/static_cast<double>(5))*(current[i][j+1]+current[i-1][j]+current[i][j]+left[i]+down[j]);
        }
        else if(i==0 && j==submatrixsize-1){
          next[i][j]=(1/static_cast<double>(5))*(current[i][j-1]+current[i][j+1]+current[i][j]+right[i]+up[j]);
        }
        else if(i==submatrixsize-1 && j==submatrixsize-1){
          next[i][j]=(1/static_cast<double>(5))*(current[i][j-1]+current[i-1][j]+current[i][j]+right[i]+down[j]);
        }
        else if(i==0){
          next[i][j]=(1/static_cast<double>(5))*(current[i][j-1]+current[i+1][j]+current[i][j+1]+up[j]+current[i][j]);
        }
        else if(j==0){
          next[i][j]=(1/static_cast<double>(5))*(current[i][j+1]+current[i+1][j]+current[i-1][j]+left[i]+current[i][j]);
        }
        else if(i==submatrixsize-1){
          next[i][j]=(1/static_cast<double>(5))*(current[i][j-1]+current[i-1][j]+current[i][j+1]+down[j]+current[i][j]);
        }
        else if(j==submatrixsize-1){
          next[i][j]=(1/static_cast<double>(5))*(current[i][j-1]+current[i+1][j]+current[i-1][j]+right[i]+current[i][j]);
        }
        else{
          next[i][j]=(1/static_cast<double>(5))*(current[i-1][j]+current[i][j-1]+current[i][j]+current[i+1][j]+current[i][j+1]);
        }
      }
    }
}

int main(int argc, char* argv[]) {

  if (argc < 3) {
    std::cerr<<"usage: mpirun "<<argv[0]<<" <N> <K>"<<std::endl;
    return -1;
  }
  MPI_Init(&argc,&argv);
  int rank,size;

  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);

  long N, K;
  N = atol(argv[1]);
  K = atol(argv[2]);

  long subsize=sqrt(size);
  submatrixsize=N/subsize;

  row1=rank/subsize;
  column1=rank%subsize;

  submatrix_row_begin = row1 * submatrixsize;
  submatrix_row_end = submatrix_row_begin + submatrixsize;
  submatrix_column_begin = column1 * submatrixsize;
  submatrix_column_end = submatrix_column_begin + submatrixsize;

  double* up = new double[submatrixsize];
  double* down = new double[submatrixsize];
  double* left = new double[submatrixsize];
  double* right = new double[submatrixsize];
  double* sleft = new double[submatrixsize];
  double* sright = new double[submatrixsize];

 
  double** current=new double*[submatrixsize];
  double** next=new double*[submatrixsize];

  for(long i=0;i<submatrixsize;i++){
    current[i]=(double*)malloc(submatrixsize * sizeof(double));
    next[i]=(double*)malloc(submatrixsize * sizeof(double));
  }

  for (long i=submatrix_row_begin;i<submatrix_row_end;i++){
    for(long j=submatrix_column_begin;j<submatrix_column_end;j++)
      current[i-submatrix_row_begin][j-submatrix_column_begin]= genH0(i, j, N);
  }

  MPI_Request* request;
  MPI_Status* status;

  MPI_Barrier(MPI_COMM_WORLD);

  std::chrono::time_point<std::chrono::system_clock> starttime = std::chrono::system_clock::now();
  if(size==1){
    for(int x=0;x<K;x++){
      for(long i = 0;i<submatrixsize;i++){
      left[i]=current[i][0];
      right[i]=current[i][submatrixsize-1];
      up[i]=current[0][i];
      down[i]=current[submatrixsize-1][i];
      }

      heat(current,next,up,down,left,right);

      {
      double**t = current;
      current=next;
      next=t;
      }
    }

  }
  else{
  for(long x=0;x<K;x++){
  if(row1==0 && column1 ==0){
    for(long i = 0;i<submatrixsize;i++){
      left[i]=current[i][0];
      right[i]=current[i][submatrixsize-1];
      up[i]=current[0][i];
      down[i]=current[submatrixsize-1][i];
      }
    for(long i = 0;i<submatrixsize;i++){
      sright[i]=current[i][submatrixsize-1];
      }
    request = new MPI_Request[4];
    status = new MPI_Status[4];
    MPI_Irecv(right, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(down, submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Isend(current[submatrixsize-1], submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(sright, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Waitall(4, request, status);
  }
  else if(row1==0 && column1 == subsize-1){
    for(long i = 0;i<submatrixsize;i++){
      sleft[i]=current[i][0];
      }
    request = new MPI_Request[4];
    status = new MPI_Status[4];
    MPI_Irecv(left, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(down, submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Isend(current[submatrixsize-1], submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(sleft, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Waitall(4, request, status);
  }
  else if(row1==subsize-1 && column1==0){
    for(long i = 0;i<submatrixsize;i++){
      sright[i]=current[i][submatrixsize-1];
      }
    request = new MPI_Request[4];
    status = new MPI_Status[4];
    MPI_Irecv(right, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(up, submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Isend(current[0], submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(sright, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Waitall(4, request, status);
  }
  else if(row1==subsize-1 && column1==subsize-1){
    for(long i = 0;i<submatrixsize;i++){
      sleft[i]=current[i][0];
      }
    request = new MPI_Request[4];
    status = new MPI_Status[4];
    MPI_Irecv(left, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(up, submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Isend(current[0], submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(sleft, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Waitall(4, request, status);

  }
  else if(row1==0){
    for(long i = 0;i<submatrixsize;i++){
      sleft[i]=current[i][0];
      sright[i]=current[i][submatrixsize-1];
      }
    request = new MPI_Request[6];
    status = new MPI_Status[6];
    MPI_Irecv(left, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(down, submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Irecv(right, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(current[submatrixsize-1], submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Isend(sleft, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[4]);
    MPI_Isend(sright, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[5]);
    MPI_Waitall(6, request, status);
  }
  else if(row1==subsize-1){
    for(long i = 0;i<submatrixsize;i++){
      sleft[i]=current[i][0];
      sright[i]=current[i][submatrixsize-1];
      }
    request = new MPI_Request[6];
    status = new MPI_Status[6];
    MPI_Irecv(left, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(up, submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Irecv(right, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(current[0], submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Isend(sleft, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[4]);
    MPI_Isend(sright, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[5]);
    MPI_Waitall(6, request, status);
  }
  else if(column1==0){
    for(long i = 0;i<submatrixsize;i++){
      sright[i]=current[i][submatrixsize-1];
      }
    request = new MPI_Request[6];
    status = new MPI_Status[6];
    MPI_Irecv(down, submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(up, submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Irecv(right, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(current[0], submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Isend(current[submatrixsize-1], submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[4]);
    MPI_Isend(sright, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[5]);
    MPI_Waitall(6, request, status);
  }
  else if(column1==subsize-1){
    for(long i = 0;i<submatrixsize;i++){
      sleft[i]=current[i][0];
      }
    request = new MPI_Request[6];
    status = new MPI_Status[6];
    MPI_Irecv(down, submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(up, submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Irecv(left, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Isend(current[0], submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Isend(current[submatrixsize-1], submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[4]);
    MPI_Isend(sleft, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[5]);
    MPI_Waitall(6, request, status);
  }
  else{
    request = new MPI_Request[8];
    status = new MPI_Status[8];
    for(long i = 0;i<submatrixsize;i++){
      sleft[i]=current[i][0];
      sright[i]=current[i][submatrixsize-1];
      }
    MPI_Irecv(up, submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[0]);
    MPI_Irecv(down, submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[1]);
    MPI_Irecv(left, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[2]);
    MPI_Irecv(right, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[3]);
    MPI_Isend(current[0], submatrixsize, MPI_DOUBLE, rank-subsize, 0, MPI_COMM_WORLD, &request[4]);
    MPI_Isend(current[submatrixsize-1], submatrixsize, MPI_DOUBLE, rank+subsize, 0, MPI_COMM_WORLD, &request[5]);
    MPI_Isend(sleft, submatrixsize, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &request[6]);
    MPI_Isend(sright, submatrixsize, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &request[7]);
    MPI_Waitall(8, request, status);
  } 
  heat(current,next,up,down,left,right);
    {
      double**t = current;
      current=next;
      next=t;
    }
}
}
MPI_Finalize();
  std::chrono::time_point<std::chrono::system_clock> endtime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = endtime-starttime;
  
  if(rank==0)
    std::cerr<<elapsed_seconds.count()<<std::endl;
  delete[] up;
  delete[] down;
  delete[] right;
  delete[] left;
  for(int i=0;i<submatrixsize;i++){
    delete current[i];
    delete next[i];
  }
  delete current;
  delete next; 
  return 0;
}

