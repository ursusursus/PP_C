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

  // Init
  MPI_Init(&argc, &argv);
  int worldRank;
  MPI_Comm_rank(MPI_COMM_WORLD, &worldRank);
  int worldSize;
  MPI_Comm_size(MPI_COMM_WORLD, &worldSize);

  // Create buffer
  int buffer[BUFFER_SIZE];
  if(worldRank == ROOT) {
    // Root fills the buffer
    buffer[0]=5; buffer[1]=1; buffer[2]=99; buffer[3]=7; buffer[4]=6; 
    buffer[5]=5; buffer[6]=4; buffer[7]=2; buffer[8]=3; buffer[9]=1;
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


  // Toto ked zavolam s rozmerom "worldSize" tak to mrdne
  //int subMaximums[worldSize];
  int subMaximums[2];
  int subMaximum = findMaximum(subBuffer, subBufferSize);
  // printf("SubMaximum: %d", subMaximum);

  //
  MPI_Gather(
    &subMaximum, 1, MPI_INT,
    subMaximums, 1, MPI_INT,
    ROOT, MPI_COMM_WORLD
    );

  // Root receives all the submaximums
  // and finds maximum of those
  if(worldRank == ROOT) {
    int totalMax = findMaximum(subMaximums, worldSize);
    printf("Maximum je: %d", totalMax);
  }

  // Clean up
  // MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}
