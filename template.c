#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
  int size;

  // Init
  MPI_Init(&argc, &argv);

  // Get number of nodes
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  printf("Je dostupných %d nodov\n", size);

  // Teardown
  MPI_Finalize();

  // Exit
  return 0;
}
