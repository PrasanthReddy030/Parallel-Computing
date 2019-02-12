#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
using namespace std;


float genA (int row, int col) {
  if (row > col)
    return 1.;
  else
    return 0.;
}

float genx0 (int i) {
  return 1.;
}


void checkx (int iter, long i, float xval) {
  if (iter == 1) {
    float shouldbe = i;
    if (fabs(xval/shouldbe) > 1.01 || fabs(xval/shouldbe) < .99 )
      std::cout<<"incorrect : x["<<i<<"] at iteration "<<iter<<" should be "<<shouldbe<<" not "<<xval<<std::endl;
  }

  if (iter == 2) {
    float shouldbe =(i-1)*i/2;
    if (fabs(xval/shouldbe) > 1.01 || fabs(xval/shouldbe) < .99)
      std::cout<<"incorrect : x["<<i<<"] at iteration "<<iter<<" should be "<<shouldbe<<" not "<<xval<<std::endl;
  }
}

long sub_row,sub_column,sub_matrixsize;
int row_root,col_root;

void matmul(float*A, float*x, float*y, long n, MPI_Comm row_comm,float*reducedy) {
  for (long row = 0; row<n; ++row) {
    float sum = 0;
    
    for (long col = 0; col<n; ++col) {
      sum += x[col] * A[row*n+col];
    }

    y[row] = sum;
  }

  MPI_Reduce(y,reducedy,sub_matrixsize,MPI_FLOAT,MPI_SUM,row_root,row_comm);

}


int main (int argc, char*argv[]) {

  if (argc < 3) {
    std::cout<<"usage: "<<argv[0]<<" <n> <iteration>"<<std::endl;
  }


  MPI_Init(&argc,&argv);
  
  int rank,size;

  bool check = true;
  
  long n = atol(argv[1]);

  long iter = atol(argv[2]);

  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);


  MPI_Comm row_comm,col_comm;

  long subsize=sqrt(size);
  sub_matrixsize=n/subsize; 
  sub_row=rank/subsize;
  sub_column=rank%subsize;


  long sub_matrix_row_begin = sub_row * sub_matrixsize;
  long sub_matrix_row_end = sub_matrix_row_begin + sub_matrixsize;
  long sub_matrix_column_begin = sub_column * sub_matrixsize;
  long sub_matrix_column_end = sub_matrix_column_begin + sub_matrixsize;

  MPI_Comm_split(MPI_COMM_WORLD,sub_row,rank,&row_comm);
  MPI_Comm_split(MPI_COMM_WORLD,sub_column,rank,&col_comm);

  row_root=sub_row;
  col_root=sub_column;
  

  float* localA = new float[sub_matrixsize*sub_matrixsize];

  for (long row = sub_matrix_row_begin; row<sub_matrix_row_end; row++) {
    for (long col=sub_matrix_column_begin; col<sub_matrix_column_end; col++) {
      localA[(row-sub_matrix_row_begin)*sub_matrixsize+(col-sub_matrix_column_begin)] = genA(row, col);
    }
  }

  float* localx = new float[sub_matrixsize];

  for (long i=sub_matrix_row_begin; i<sub_matrix_row_end; ++i)
    localx[i-sub_matrix_row_begin] = genx0(i);

  
  float* localy = new float[sub_matrixsize];
  float* reducedlocaly = new float[sub_matrixsize];

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  for (int it = 0; it<iter; ++it) {
  
    matmul(localA, localx, localy, sub_matrixsize, row_comm,reducedlocaly);

    {
      float*t = localx;
      localx=reducedlocaly;
      reducedlocaly=t;
    }

    if(sub_row==sub_column){
    if (check)
      for (long i = sub_matrix_row_begin; i<sub_matrix_row_end; ++i)
	     checkx (it+1, i, localx[i-sub_matrix_row_begin]);
   }
        MPI_Bcast(localx,sub_matrixsize,MPI_FLOAT,col_root,col_comm);
  }

  MPI_Finalize();
  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;

  std::cerr<<elapsed_seconds.count()<<std::endl;
  
  
  delete[] localA;
  delete[] localx;
  delete[] localy;
  delete[] reducedlocaly;
  
  return 0;
}
