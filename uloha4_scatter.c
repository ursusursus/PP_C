#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define MAX_STRINGS 4
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
  int size, rank;
  // Dakedy tie define idu, dakedy nie, nasrat, bude to tu
  int maxStrings = 4;
  char table[maxStrings][MAX_STRING_LENGTH];
  char keyword[MAX_STRING_LENGTH];

  // Init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  /**
  *
  * Obmedzenie ze pocet stringov musi byt delitelne poctom procesov
  * inac treba pouzit scatterv, ktory mi zatial nejde
  *
  */

  if(rank == 0) {
    strcpy(keyword, "hello2");

    strcpy(table[0], "hello");
    strcpy(table[1], "worldddd");
    strcpy(table[2], "hello2");
    strcpy(table[3], "worldddd2");
  }

  int subTableStrings = maxStrings / size;
  char subTable[subTableStrings][MAX_STRING_LENGTH];

  MPI_Scatter(
    table, subTableStrings * MAX_STRING_LENGTH, MPI_CHAR,
    subTable, subTableStrings * MAX_STRING_LENGTH, MPI_CHAR,
    0, MPI_COMM_WORLD
    );

  MPI_Bcast(keyword, MAX_STRING_LENGTH, MPI_CHAR, 0, MPI_COMM_WORLD);

  int subMatchesSize = subTableStrings;
  int subMatches[subMatchesSize];
  findMatches(subTableStrings, MAX_STRING_LENGTH, subTable, keyword, subMatches);

  int matchesSize = size;
  int matches[matchesSize];

  // JEBE
  // int pica[2];
  // END JEBE

  MPI_Gather(
    subMatches, subMatchesSize, MPI_INT,
    matches, subMatchesSize, MPI_INT,
    0, MPI_COMM_WORLD
    );

  if(rank == 0) {
    int matchesCount = 0;
    int i;
    for(i = 0; i < matchesSize; i++) {
      if(matches[i] != -1) {
        printf("Match found at: %d'\n", (i * subMatchesSize) + matches[i]);
        matchesCount++;
      }
    }

    if(matchesCount <= 0) {
      printf("No matches found!'\n");
    }
  }

  // Teardown
  MPI_Finalize();

  // Exit
  return 0;
}
