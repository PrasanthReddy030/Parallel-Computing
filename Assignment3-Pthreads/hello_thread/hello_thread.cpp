#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include  <pthread.h>

struct thread_args{
  int i;
  int nbthreads;
};

void* f(void* p) {

  struct thread_args *args = (struct thread_args *)p;


	//std::cout<<"I am thread "<< *(int*)p<<" in nbthreads\n";
  std::cout<<"I am thread "<<args->i<<" in "<<args->nbthreads<<"\n";
//printf ("%s\n", p);
return  NULL;
}

int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbthreads>"<<std::endl;
    return -1;
  }
  
  int nbthreads=atoi(argv[1]);
  pthread_t student [10];
  struct thread_args arr[nbthreads];

for (int j=0; j < nbthreads; ++j)
{	
	arr[j].i=j;
  arr[j].nbthreads=nbthreads;
    pthread_create (& student[j], NULL , f, (void*)&arr[j]);
}

//wait  for  the  nbthreads  to  complete
for (int i=0; i < nbthreads; ++i)
pthread_join(student[i], NULL);

  return 0;
}
