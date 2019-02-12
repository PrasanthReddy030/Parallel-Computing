#include <iostream>
#include <chrono>

__global__ void polynomial_expansion (float* poly, int degree,
			   int n, float* array) {
  //TODO: Write code to use the GPU here!
  //code should write the output back to array
  
  int index = blockIdx.x * blockDim.x + threadIdx.x;
  if(index < n){
    float out = 0.;
    float xtothepowerof = 1.;
    for (int i=0; i<=degree; ++i) {
      out += xtothepowerof*poly[i];
      xtothepowerof *= array[index];
    }
    array[index] = out;

  }

}

int main (int argc, char* argv[]) {
  //TODO: add usage
  
  if (argc < 3) {
     std::cerr<<"usage: "<<argv[0]<<" n degree"<<std::endl;
     return -1;
  }

  int n = atoi(argv[1]); //TODO: atoi is an unsafe function
  int degree = atoi(argv[2]);
  int nbiter = 1;

  float* array = new float[n];
  float* poly = new float[degree+1];
  float* d_array;
  float* d_poly;
  for (int i=0; i<n; ++i)
    array[i] = 1.;

  for (int i=0; i<degree+1; ++i)
    poly[i] = 1.;

    long asize=n*sizeof(float);
    long psize=(degree+1)*sizeof(float);


  std::chrono::time_point<std::chrono::system_clock> begin, end;
  begin = std::chrono::system_clock::now();

  cudaMallocManaged(&d_array, asize);
  cudaMallocManaged(&d_poly, psize);

  cudaMemcpy(d_array,array,asize,cudaMemcpyHostToDevice);
  cudaMemcpy(d_poly,poly,psize,cudaMemcpyHostToDevice);

  int blocks=n/256;
  if(n%256>0){
    blocks = n/256+1;
  }

 
  for (int iter = 0; iter<nbiter; ++iter)
    polynomial_expansion<<<blocks,256>>> (d_poly, degree, n, d_array);

  cudaMemcpy(array,d_array,asize,cudaMemcpyDeviceToHost);
  cudaFree(d_array);
  cudaFree(d_poly);

  end = std::chrono::system_clock::now();
  std::chrono::duration<double> totaltime = (end-begin)/nbiter;

  {
    bool correct = true;
    int ind;
    for (int i=0; i< n; ++i) {
      if (fabs(array[i]-(degree+1))>0.01) {
        correct = false;
	ind = i;
      }
    }
    if (!correct)
      std::cerr<<"Result is incorrect. In particular array["<<ind<<"] should be "<<degree+1<<" not "<< array[ind]<<std::endl;
  }
  

  std::cerr<<array[0]<<std::endl;
  std::cout<<n<<" "<<degree<<" "<<totaltime.count()<<std::endl;

  delete[] array;
  delete[] poly;

  return 0;
}
