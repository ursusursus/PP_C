#include <stdio.h>
#include <mpi.h>
#define BUFFER_SIZE 10
#define ROOT 0

int findMaximum(int *buffer, int bufferSize) {
  int maximum = buffer[0];
  int i;

  for(i = 0; i < bufferSize; i++) {
    if(buffer[i] > maximum) {
      maximum = buffer[i];
    }
  }

  return maximum;
}

int main(int argc, char *argv[]) {
  int rank;
  int worldSize;

  // Init
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  if(BUFFER_SIZE % size != 0) {
    printf("Velkost buffra musi byt delitelna poctom procesov\n");
    return 0;
  }

  // Create buffer
  int buffer[BUFFER_SIZE];
  if(rank == ROOT) {
    // Root fills the buffer
    buffer[0]=5; buffer[1]=1; buffer[2]=9; buffer[3]=7; buffer[4]=6; 
    buffer[5]=5; buffer[6]=4; buffer[7]=28; buffer[8]=3; buffer[9]=1;
  }

  // Create sub-buffer
  int subBufferSize = BUFFER_SIZE / worldSize;
  int subBuffer[subBufferSize];

  //
  MPI_Scatter(
    buffer, subBufferSize, MPI_INT,
    subBuffer, subBufferSize, MPI_INT,
    ROOT, MPI_COMM_WORLD
    );

  //
  int subMaximums[worldSize];
  int subMaximum = findMaximum(subBuffer, subBufferSize);

  // -- Pozor, tu count je pocet prijatych
  // elementov na proces, nie vsetkych spolu
  MPI_Gather(
    &subMaximum, 1, MPI_INT,
    subMaximums, 1, MPI_INT,
    ROOT, MPI_COMM_WORLD
    );

  // Root receives all the submaximums
  // and finds maximum of those
  if(rank == ROOT) {
    int totalMax = findMaximum(subMaximums, worldSize);
    printf("Maximum je: %d", totalMax);
  }

  // Clean up
  MPI_Finalize();
  return 0;
}
