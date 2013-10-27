#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define ROOT 0
#define TABLE_SIZE 8
#define MAX_STRING_LENGTH 15

int findMatches(int stringCount, int stringLength, char table[stringCount][stringLength], char keyword[stringLength], int matches[stringCount]) {
  int i;
  for(i = 0; i < stringCount; i++) {
    if(strcmp(keyword, table[i]) == 0) {
      matches[i] = i;
    } else {
      matches[i] = -1;
    }
  }
}

int main(int argc, char *argv[]) {
  
  char table[TABLE_SIZE][MAX_STRING_LENGTH];
  char keyword[MAX_STRING_LENGTH];

  int size, rank;

  // Init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sendCounts[size];
  int displacements[size];

  if(rank == ROOT) {
    strcpy(keyword, "paralelne");

    strcpy(table[0], "hello");
    strcpy(table[1], "world");
    strcpy(table[2], "paralelne");
    strcpy(table[3], "programovanie");
    strcpy(table[4], "rofl");
    strcpy(table[5], "lol");
    strcpy(table[6], "wtf");
    strcpy(table[7], "meh");

    int stringPerProcess = TABLE_SIZE / size;

    int i;
    int sum = 0;
    for(i = 0; i < size; i++) {
      int sendCount = MAX_STRING_LENGTH * stringPerProcess;
      sendCounts[i] = sendCount;
      displacements[i] = sum;
      sum += sendCount;
    }

    /* int j;
    for(j = 0; j < size; j++) {
      printf("S[%d]:%d - D[%d]:%d\n", j, sendCounts[j], j, displacements[j]);
    } */

  }


  int recvCount;
  MPI_Scatter(
      sendCounts, 1, MPI_INT,
      &recvCount, 1, MPI_INT,
      ROOT, MPI_COMM_WORLD
    );

  printf("RECVCOUNT:%d", recvCount / MAX_STRING_LENGTH);

  int subTableSize = recvCount / MAX_STRING_LENGTH;
  char subTable[subTableSize][MAX_STRING_LENGTH];

  MPI_Scatterv(
    table, sendCounts, displacements, MPI_CHAR,
    subTable, sendCounts[rank], MPI_CHAR,
    ROOT, MPI_COMM_WORLD
  );



  MPI_Bcast(keyword, MAX_STRING_LENGTH, MPI_CHAR, ROOT, MPI_COMM_WORLD);
 /*
  int subMatchesSize = subTableSize;
  int subMatches[subMatchesSize];
  findMatches(subTableSize, MAX_STRING_LENGTH, subTable, keyword, subMatches);

  int i;
  for(i = 0; i < subMatchesSize; i++) {
    printf("%d@", subMatches[i]);
  }

  /* int matchesSize = size;
  int matches[matchesSize];

  MPI_Gatherv(
    subMatches, subMatchesSize, MPI_INT,
    matches, sendCounts, displacements, MPI_INT,
    ROOT, MPI_COMM_WORLD
    ); */



  /* if(rank == 1) {
    int i;
    for(i = 0; i < subTableSize; i++) {
      printf("%s-", subTable[i]);
    }
  } */

  

  // Teardown
  MPI_Finalize();

  // Exit
  return 0;
}
