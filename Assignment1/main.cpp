#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include<stdio.h>
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
  float sum =0.0;
  float a=atof(argv[2]);
  float b=atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity=atoi(argv[5]);

  float q =(b-a)/n;
  float r;

  auto start1 = std::chrono::high_resolution_clock::now();
  
  switch(p){
        case 1: for (int i=0;i<=n-1;i++)
                  sum+=f1((a+ (i+0.5)*q),intensity);
                 	r=q*sum;
                
                std::cout<<r<<endl;
                break;

        case 2: for (int i=0;i<=n-1;i++)
                  sum+=f2((a+ (i+0.5)*q),intensity);
                  r=q*sum;
                 
                 std::cout<<r<<endl;
                 break;

        case 3: for (int i=0;i<=n-1;i++)
                  sum+=f3((a+ (i+0.5)*q),intensity);
                  r=q*sum;
                 
                  std::cout<<r<<endl;
                  break;

        case 4: for (int i=0;i<=n-1;i++)
                  sum+=f4((a+ (i+0.5)*q),intensity);
                  r=q*sum;
                 
                std::cout<<r<<endl;
                break;

            }

        auto start2 = std::chrono::high_resolution_clock::now() - start1;
        auto start3 = std::chrono::duration_cast<std::chrono::nanoseconds>(start2);
             
        std::cerr << start3.count()<<std::endl;
        
  return 0;
}
