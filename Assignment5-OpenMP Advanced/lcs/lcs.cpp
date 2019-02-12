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

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif


int main (int argc, char* argv[]) {
  
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

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  int nbthreads = atoi(argv[3]);
  int height,width;
  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  generateLCS(X, m, Y, n);
  width=m+1;
  height=n+1;
  
  int** C = new int*[width];
  for (int i=0; i<=width-1; ++i) {
    C[i] = new int[height];
    C[i][0] = 0;
  }
  for (int j=0; j<=height-1; ++j) {
    C[0][j] = 0;
  }
  
  //insert LCS code here.
  int result = -1; // length of common subsequence

  #if DEBUG
  std::cout<<m<<" "<<n<<std::endl;
  for (int i=0; i<m; ++i) std::cout<<X[i];
  std::cout<<std::endl;
  for (int i=0; i<n; ++i) std::cout<<Y[i];
  std::cout<<std::endl<<std::endl<<lcs<<std::endl;
#endif

   omp_set_num_threads(nbthreads);
   
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

  for(int k=2;k<width+height-1;k++){

  		#pragma omp parallel for
        for(int j=1;j<k;j++){

            int i=k-j;
                 
            if(i<width&&j<height) {
            
            if (X[i-1] == Y[j-1]) {
            C[i][j] = C[i-1][j-1] + 1; 
            } else {
            C[i][j] = std::max(C[i-1][j], C[i][j-1]);
            }

            }                
        }

  }

  
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end-start;

  checkLCS(X, m, Y, n,C[width-1][height-1]);

    std::cerr<<elpased_seconds.count()<<std::endl;

  delete[] X;
  delete[] Y;
  for (int i=0; i<=m; ++i) { 
    delete[] C[i];
  }
  delete[] C;


  return 0;
}
