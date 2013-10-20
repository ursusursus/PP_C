#include <mpi.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>


#define POCETRETAZCOV 4
#define MAXDLZKARETAZCA 15


int main (int argc, char ** argv) {
    
    MPI_Status status;
    MPI_Init(&argc, &argv);

    char table[POCETRETAZCOV][MAXDLZKARETAZCA];
    int size, rank;
    
    int *sendcounts, *offsets, pocetSlovNaProces, zvysok, *retbuf, *matches;    
  
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pocetSlovNaProces = POCETRETAZCOV / size;     
    zvysok = POCETRETAZCOV % size;

    if (rank == 0)
    {
        strcpy(table[0], "hello");
        strcpy(table[1], "world");
        strcpy(table[2], "paralelne");
        strcpy(table[3], "programovanie");

        sendcounts = (int*) malloc(size * sizeof(int));
        offsets = (int*) malloc(size * sizeof(int));

        int i;
        for(i = 0; i < size; i++)
        {
            if(i < zvysok)
            {
                sendcounts[i] = pocetSlovNaProces + 1;  // pocet slov pre proces
                offsets[i] = i * (pocetSlovNaProces + 1);
            }
            else
            {
                sendcounts[i] = pocetSlovNaProces;
                offsets[i] = (zvysok*(pocetSlovNaProces+1)) + ((i-zvysok) * pocetSlovNaProces);
            }
        }
    }

    if(rank == 0) {
        int i;
        for(i = 0; i < size; i++) {
            printf("lol:%d", offsets[i]);
        }
    }

    /* pocetSlovNaProces = rank < zvysok ? pocetSlovNaProces+1:pocetSlovNaProces;
    char dataPreProces[pocetSlovNaProces][MAXDLZKARETAZCA];
    // dataPreProces = malloc(sizeof * dataPreProces * pocetSlovNaProces);
    MPI_Scatterv(table, sendcounts, offsets, MPI_CHAR, dataPreProces, pocetSlovNaProces, MPI_CHAR, 0, MPI_COMM_WORLD);
    */ 
    char dataPreProces[pocetSlovNaProces][MAXDLZKARETAZCA];
    MPI_Scatterv(
        table, sendcounts, offsets, MPI_CHAR, 
        dataPreProces, pocetSlovNaProces, MPI_CHAR, 
        0, MPI_COMM_WORLD
        );

    if(rank == 1) {
        int i;
        for(i = 0; i < pocetSlovNaProces; i++) {
            printf("%s ", dataPreProces[i]);
        }
    }

    MPI_Finalize();
    return 0;
}

