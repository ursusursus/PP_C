#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define MAX_STRINGS 4
#define MAX_STRING_LENGTH 15

int main(int argc, char *argv[]) {
  int size, rank;
  char array[2][MAX_STRING_LENGTH];

  // Init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if(rank == 0) {
    strcpy(array[0], "hello");
    strcpy(array[1], "worldddd");
    strcpy(array[2], "hello2");
    strcpy(array[3], "worldddd2");
  }

  int subArrayStrings = MAX_STRINGS / size;
  char subArray[subArrayStrings][MAX_STRING_LENGTH];
  MPI_Scatter(
    array, subArrayStrings * MAX_STRING_LENGTH, MPI_CHAR,
    subArray, subArrayStrings * MAX_STRING_LENGTH, MPI_CHAR,
    0, MPI_COMM_WORLD
    );

  // if(rank == 1) {
    int i;
    for(i = 0; i < subArrayStrings; i++) {
      printf("%s-", subArray[i]);
    }
  // }

  // Teardown
  MPI_Finalize();

  // Exit
  return 0;
}
