#include <mpi.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>


#define POCETRETAZCOV 10
#define MAXDLZKARETAZCA 15

typedef struct{
    char meno[MAXDLZKARETAZCA];
    unsigned int vek;
} person;

int main (int argc, char ** argv) {
    
    MPI_Status status;
    MPI_Init(&argc, &argv);

    person osoby[POCETRETAZCOV];
    person *dataPreProces;

    const int nitems=3;
    int blocklengths[3] = {MAXDLZKARETAZCA,1,1};
    MPI_Datatype types[3] = {MPI_UNSIGNED_CHAR, MPI_INT, MPI_UB};
    MPI_Datatype person;
    MPI_Aint base;
    MPI_Aint offsety[3];

    MPI_Address(&osoby[0].meno, &offsety[0]);
    MPI_Address(&osoby[0].vek, &offsety[1]);
    MPI_Address(&osoby[1].meno, &offsety[2]);
    base = offsety[0];
    int i,j;
    for (i = 0; i < 4; ++i) offsety[i] -= base;

    MPI_Type_create_struct(nitems, blocklengths, offsety, types, &person);
    MPI_Type_commit(&person); 

    int size, rank;
    
    char rozsahMien[2];
    int rozsahVeku[2];
    int *sendcounts, *offsets, pocetSlovNaProces, zvysok, *retbuf, *matches;    
  
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    pocetSlovNaProces = POCETRETAZCOV / size;     
    zvysok = POCETRETAZCOV % size;

    if (rank == 0)
    {
        //inicializacia mien
        strcpy(osoby[0].meno, "Peter"); osoby[0].vek = 23; strcpy(osoby[1].meno, "Anna"); osoby[1].vek = 25;
        strcpy(osoby[2].meno, "Michal"); osoby[2].vek = 26; strcpy(osoby[3].meno, "Alzbeta"); osoby[3].vek = 30;
        strcpy(osoby[4].meno, "Jaro"); osoby[4].vek = 18; strcpy(osoby[5].meno, "Jana"); osoby[5].vek = 20;
        strcpy(osoby[6].meno, "Zuzana"); osoby[6].vek = 23; strcpy(osoby[7].meno, "Albert"); osoby[7].vek = 48;
        strcpy(osoby[8].meno, "Dragan"); osoby[8].vek = 22; strcpy(osoby[9].meno, "Peter"); osoby[9].vek = 55;
        
        printf("\nTabulka osob:\n\n");
        for(i = 0; i < POCETRETAZCOV; i++)
        {
             printf("[%d] %s : %d\n", i, osoby[i].meno, osoby[i].vek);
        }
        printf("\n");

        rozsahMien[0] = 'J';
        rozsahMien[1] = 'P';
        rozsahVeku[0] = 15;
        rozsahVeku[1] = 30;

        matches = (int*) malloc(POCETRETAZCOV * sizeof(int));

        sendcounts = (int*) malloc(size*sizeof(int));

        offsets = (int*) malloc(size*sizeof(int));
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

    MPI_Bcast(rozsahMien,2,MPI_CHAR,0,MPI_COMM_WORLD);
    MPI_Bcast(rozsahVeku,2,MPI_INT,0,MPI_COMM_WORLD);

    pocetSlovNaProces = rank < zvysok ? pocetSlovNaProces+1:pocetSlovNaProces;
    dataPreProces = malloc(sizeof * dataPreProces * pocetSlovNaProces);
    MPI_Scatterv(osoby, sendcounts, offsets, person, dataPreProces, pocetSlovNaProces, person, 0, MPI_COMM_WORLD);

    retbuf = (int*) malloc(pocetSlovNaProces*sizeof(int));

    for(i = 0; i < pocetSlovNaProces; i++)
    {
        //printf("rank %d: %s\n", rank, dataPreProces[i]);
        retbuf[i] = 0;
        int prve = rozsahMien[0] - '0';
        int posledne = rozsahMien[1] - '0';
        int rozsah = posledne - prve;
        for(j = 0; j <= rozsah; j++){
           char string[1];
           char pismeno = rozsahMien[0] + j;
           if((dataPreProces[i].meno[0] == pismeno) && dataPreProces[i].vek >= rozsahVeku[0] && dataPreProces[i].vek <= rozsahVeku[1])
            retbuf[i] = 1;
        }
        
    }

    MPI_Gatherv(retbuf, pocetSlovNaProces, MPI_INT, matches, sendcounts, offsets, MPI_INT, 0, MPI_COMM_WORLD);

    if(rank == 0)
    {
        printf("\nVysledok vyhladavania pre rozsah mien %c - %c a veku %d - %d \n", rozsahMien[0], rozsahMien[1], rozsahVeku[0], rozsahVeku[1]);
        for(i = 0; i < POCETRETAZCOV; i++)
        {
            if(matches[i] == 1)
                printf("[%d] %s %d\n", i, osoby[i].meno, osoby[i].vek);
        }
    }

    MPI_Finalize();
    return 0;
}

