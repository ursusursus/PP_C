#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define ROOT 0
#define TABLE_SIZE 8
#define MAX_STRING_LENGTH 15
#define KEYWORD_LENGTH 5

int findMatch(int stringCount, int stringLength, char table[stringCount][stringLength], char* keyword) {
  int i;
  for(i = 0; i < stringCount; i++) {
    if(strcmp(keyword, table[i]) == 0) {
      return i;
    }
  }
  return -1;
}

int main(int argc, char *argv[]) {
  
  char table[TABLE_SIZE][MAX_STRING_LENGTH];
  char keyword[KEYWORD_LENGTH];

  int size, rank;

  // Init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sendCounts[size];
  int displacements[size];

  if(rank == ROOT) {
    strcpy(keyword, "hello");

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

    int j;
    for(j = 0; j < size; j++) {
      printf("S[%d]:%d - D[%d]:%d\n", j, sendCounts[j], j, displacements[j]);
    }

  }

  MPI_Bcast(keyword, KEYWORD_LENGTH, MPI_CHAR, ROOT, MPI_COMM_WORLD);

  int recvCount;
  MPI_Scatter(
      sendCounts, 1, MPI_INT,
      &recvCount, 1, MPI_INT,
      ROOT, MPI_COMM_WORLD
    );

  //printf("AAAAAAAAAA%dAAAAAAAAAAA", recvCount);

  int subTableSize = TABLE_SIZE / size;
  // int subTableSize = recvCount / MAX_STRING_LENGTH;
  char subTable[subTableSize][MAX_STRING_LENGTH];

  MPI_Scatterv(
    table, sendCounts, displacements, MPI_CHAR,
    subTable, sendCounts[rank], MPI_CHAR,
    ROOT, MPI_COMM_WORLD
  );

  if(rank == 1) {
    int i;
    for(i = 0; i < subTableSize; i++) {
      printf("%s-", subTable[i]);
    }
  }

  /* int match = findMatch(subTableSize, MAX_STRING_LENGTH, subTable, keyword);
  // printf("%d", match);

  if(rank == 0) {
    int matches = 0;
    if(match != -1) {
      printf("-- Match Found at [%d]\n", match);
      matches++;
    }

    int i;
    for(i = 1; i < size; i++) {
      MPI_Status status;
      MPI_Recv(&match, 1, MPI_INT, i, 123, MPI_COMM_WORLD, &status);

      if(match != -1) {
        printf("-- Match Found at [%d]!\n", match);
        matches++;
      }
    }

    if(matches == 0) {
      printf("No matches found...");
    }

  } else {
    int i;
    for(i = 1; i < size; i++) {
      MPI_Send(&match, 1, MPI_INT, 0, 123, MPI_COMM_WORLD);
    }
  } */

  // Teardown
  MPI_Finalize();

  // Exit
  return 0;
}
