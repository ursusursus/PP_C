#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define ROOT 0
#define TABLE_SIZE 4
#define MAX_STRING_LENGTH 15
#define KEYWORD_LENGTH 4

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
  /* char table[TABLE_SIZE][MAX_STRING_LENGTH] = {
      "hello", "world", "paralelne", "programovanie", "rofl", "lol", "wtf", "meh"
  }; */
  char table[TABLE_SIZE][MAX_STRING_LENGTH];
  // int stringLengths[TABLE_SIZE] = {5, 5, 9, 13, 4, 3, 3, 3};
  // int stringLengths[TABLE_SIZE];
  // int displacements[TABLE_SIZE] = {0, 5, 10, 19, 32, 36, 39, 42};

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
    /* strcpy(table[4], "rofl");
    strcpy(table[5], "lol");
    strcpy(table[6], "wtf");
    strcpy(table[7], "meh"); */

    sendCounts[0] = 5;
    sendCounts[1] = 5;
    sendCounts[2] = 9;
    sendCounts[3] = 13;

    /* displacements[0] = MAX_STRING_LENGTH;
    displacements[1] = MAX_STRING_LENGTH * 2;
    displacements[2] = MAX_STRING_LENGTH * 3;
    displacements[3] = MAX_STRING_LENGTH * 4; */
    displacements[0] = 0;
    displacements[1] = MAX_STRING_LENGTH * 1;
    displacements[2] = MAX_STRING_LENGTH * 2;
    displacements[3] = MAX_STRING_LENGTH * 3;

    /* int i;
    int sum = 0;
    for(i = 0; i < TABLE_SIZE; i++) {
      int stringLength = strlen(table[i]);

      stringLengths[i] = stringLength;
      displacements[i] = sum;
      sum += stringLength;
    } */

    /* int j;
    for(j = 0; j < TABLE_SIZE; j++) {
      printf("%d-%d ", stringLengths[j], displacements[j]);
    } */
  }

  MPI_Bcast(keyword, KEYWORD_LENGTH, MPI_CHAR, ROOT, MPI_COMM_WORLD);
  // MPI_Bcast(stringLengths, TABLE_SIZE, MPI_INT, ROOT, MPI_COMM_WORLD);
  // MPI_Bcast(displacements, TABLE_SIZE, MPI_INT, ROOT, MPI_COMM_WORLD);

  int subTableSize = TABLE_SIZE / size;
  char subTable[subTableSize][MAX_STRING_LENGTH];

  //
  // Blbosti to prijma ale inac funguje
  //
  /* MPI_Scatterv(
    table, sendCounts, displacements, MPI_CHAR,
    subTable, sendCounts[rank], MPI_CHAR,
    ROOT, MPI_COMM_WORLD
    ); */
  /* MPI_Scatterv(
    table, sendCounts, displacements, MPI_CHAR,
    subTable, MAX_STRING_LENGTH, MPI_CHAR,
    ROOT, MPI_COMM_WORLD
    ); */
    /* MPI_Scatter(
      table, MAX_STRING_LENGTH, MPI_CHAR,
      subTable, MAX_STRING_LENGTH, MPI_CHAR,
      ROOT, MPI_COMM_WORLD
      ); */

  // if(rank == 0) {
    int i;
    for(i = 0; i < subTableSize; i++) {
      printf("%s-", subTable[i]);
    }
  // }

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
