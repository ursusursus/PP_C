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
      matches[i] = 1;
    } else {
      matches[i] = 0;
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
  int charSendCounts[size];
  int charDisplacements[size];

  //
  if(rank == ROOT) {
    strcpy(keyword, "wtf");

    strcpy(table[0], "hello");
    strcpy(table[1], "world");
    strcpy(table[2], "paralelne");
    strcpy(table[3], "programovanie");
    strcpy(table[4], "rofl");
    strcpy(table[5], "lol");
    strcpy(table[6], "wtf");
    strcpy(table[7], "meh");

    int stringsPerProcess = TABLE_SIZE / size;
    int remainder = TABLE_SIZE % size;

    // Vypocitat sendCounty a displacementy
    int i;
    int sum = 0;
    for(i = 0; i < size; i++) {
      int sendCount = stringsPerProcess;
      // Upravit pocet pre nedelitelne veci
      if(remainder > 0) {
        sendCount++;
        remainder--;
      }

      sendCounts[i] = sendCount;
      charSendCounts[i] = sendCount * MAX_STRING_LENGTH;;

      displacements[i] = sum;
      charDisplacements[i] = sum * MAX_STRING_LENGTH;

      sum += sendCount;
    }

  }


  int subTableSize;

  // Rozscatterovat velkost poli
  MPI_Scatter(
    sendCounts, 1, MPI_INT,
    &subTableSize, 1, MPI_INT,
    ROOT, MPI_COMM_WORLD
    );

  printf("SubTableSize: %d", subTableSize);

  char subTable[subTableSize][MAX_STRING_LENGTH];

  // Rozscatterovat stringy
  MPI_Scatterv(
    table, charSendCounts, charDisplacements, MPI_CHAR,
    subTable, charSendCounts[rank], MPI_CHAR,
    ROOT, MPI_COMM_WORLD
  );

  // Broadcastnut keyword
  MPI_Bcast(keyword, MAX_STRING_LENGTH, MPI_CHAR, ROOT, MPI_COMM_WORLD);

  // Vyhladat matche
  int subMatchesSize = subTableSize;
  int subMatches[subMatchesSize];
  findMatches(subTableSize, MAX_STRING_LENGTH, subTable, keyword, subMatches);

  int matchesSize = TABLE_SIZE;
  int matches[matchesSize];

  // Gathernut matche na root
  MPI_Gatherv(
    subMatches, subMatchesSize, MPI_INT,
    matches, sendCounts, displacements, MPI_INT,
    ROOT, MPI_COMM_WORLD
    );

  // Na roote
  // vypocitat spravny index
  if(rank == 0) {
    int matchesCount = 0;
    int i;
    for(i = 0; i < matchesSize; i++) {
      if(matches[i] != 0) {
        printf("Match found at: %d'\n", i);
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
