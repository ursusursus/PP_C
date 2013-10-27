#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define MAX_STRING_COUNT 4 //Nejde, dafuq hej
#define MAX_STRING_LENGTH 15

int hash(int stringLength, char string[stringLength]) {
  int sum = 0;
  int i;
  for(i = 0; i < stringLength; i++) {
    sum += (int) string[i];
  }
  return sum;
  printf("Sum: %d", sum);
}


int main(int argc, char *argv[]) {
  int size, rank;

  int maxStringCount = 4;
  char strings[maxStringCount][MAX_STRING_LENGTH];

  int hashTableSize = 126 * MAX_STRING_LENGTH;
  int hashTable[hashTableSize];

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
    strcpy(strings[0], "hello");
    strcpy(strings[1], "worldddd");
    strcpy(strings[2], "hello2");
    strcpy(strings[3], "worldddd2");
  }

  int subStringsCount = maxStringCount / size;
  char subStrings[subStringsCount][MAX_STRING_LENGTH];

  MPI_Scatter(
    strings, subStringsCount * MAX_STRING_LENGTH, MPI_CHAR,
    subStrings, subStringsCount * MAX_STRING_LENGTH, MPI_CHAR,
    0, MPI_COMM_WORLD
    );


  int subHashesCount = subStringsCount;
  int subHashes[subHashesCount];
  int i;
  for(i = 0; i < subHashesCount; i++) {
    subHashes[i] = hash(strlen(subStrings[i]), subStrings[i]);
  }

  int hashesCount = maxStringCount;
  int hashes[hashesCount];

  MPI_Gather(
    subHashes, subHashesCount, MPI_INT,
    hashes, subHashesCount, MPI_INT,
    0, MPI_COMM_WORLD
    );

  if(rank == 0) {
    // Vynulovat tabulku
    int i;
    for(i = 0; i < hashTableSize; i++) {
      hashTable[i] = 0;
    }

    // Naplnit hodnoty
    int j;
    for(j = 0; j < hashesCount; j++) {
      int hash = hashes[j];
      hashTable[hash] = 1;
      
      printf("Calculated hash: %d'\n", hash);
    }
  }

  // Teardown
  MPI_Finalize();

  // Exit
  return 0;
}
