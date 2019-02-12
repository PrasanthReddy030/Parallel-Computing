#include <mpi.h>
#include <unistd.h>
#include <iostream>
#include <limits.h>

int main(int argc, char*argv[]) {

	int rank, size, p;
	char hostname[HOST_NAME_MAX + 1];
	hostname[HOST_NAME_MAX] = 0;


	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	p=gethostname(hostname, HOST_NAME_MAX);

	if(p==0)
	printf("I am  process %d out of %d . I am running on %s\n", rank, size,hostname);

	else 
		perror("gethostname");

	MPI_Finalize();
	return 0;

}


	