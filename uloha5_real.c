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

  // int maxStringCount = 4;
  char strings[MAX_STRING_COUNT][MAX_STRING_LENGTH];

  int hashTableSize = 126 * MAX_STRING_LENGTH;
  int hashTable[hashTableSize];

  // Init
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int sendCounts[size];
  int displacements[size];
  int charSendCounts[size];
  int charDisplacements[size];

  // Obmedzenie
  if(size > MAX_STRING_COUNT) {
    printf("Prilis vela procesov");
    return 0;
  }

  if(rank == 0) {
    strcpy(strings[0], "hello");
    strcpy(strings[1], "worldddd");
    strcpy(strings[2], "hello2");
    strcpy(strings[3], "worldddd2");

    int stringsPerProcess = MAX_STRING_COUNT / size;
    int remainder = MAX_STRING_COUNT % size;

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

    /* int j;
    for(j = 0; j < size; j++) {
      printf("S[%d]: %d - D[%d]: %d\n", j, sendCounts[j], j, displacements[j]);
    } */
  }

  int subStringsCount;

  // Rozscatterovat velkost poli
  MPI_Scatter(
    sendCounts, 1, MPI_INT,
    &subStringsCount, 1, MPI_INT,
    0, MPI_COMM_WORLD
    );


  char subStrings[subStringsCount][MAX_STRING_LENGTH];

  // Rozscatterovat stringy
  MPI_Scatterv(
    strings, charSendCounts, charDisplacements, MPI_CHAR,
    subStrings, charSendCounts[rank], MPI_CHAR,
    0, MPI_COMM_WORLD
  );

  // Vypocitat hashe zo stringov
  int subHashesCount = subStringsCount;
  int subHashes[subHashesCount];
  int i;
  for(i = 0; i < subHashesCount; i++) {
    subHashes[i] = hash(strlen(subStrings[i]), subStrings[i]);
  }

  int hashesCount = MAX_STRING_COUNT;
  int hashes[hashesCount];  
  
  // Gathernut vypocitane hashe
  MPI_Gatherv(
    subHashes, subHashesCount, MPI_INT,
    hashes, sendCounts, displacements, MPI_INT,
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
