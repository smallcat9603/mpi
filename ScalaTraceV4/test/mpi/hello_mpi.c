#include <stdio.h>
#include "mpi.h"

int main(int argc, char **argv){
  int numprocs, rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  if (rank == 0) {
    printf("%d ranks start...", numprocs); 
    fflush(stdout);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  if (rank == 0) {
    printf("end\n"); 
    fflush(stdout);
  }
  return 0;
}
