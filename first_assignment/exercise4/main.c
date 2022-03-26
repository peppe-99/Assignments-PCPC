#include<stdio.h>
#include<mpi.h>
#include"mycollective.h"

int main(int argc, char **argv) {

    int myrank, np;
    int number[10];
    char *message;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


    if (myrank == 0) {
        int number[np-1];
        gather(&number, MPI_INT, np, myrank, sizeof(int), &status);
        for (int i = 0; i<np-1; i++) {
            printf("%d ", number[i]);
        }
        printf("\n");

    }
    else {
        char x = 'c';
        gather(&myrank, MPI_INT, np, myrank, sizeof(int), &status);
    }



/*  TEST BROADCAST
   if (myrank == 0) {
        for(int i = 0; i < 10; i++) {
            number[i] = i;
        }
        broadcast(&number, MPI_INT, np, myrank, 10, &status);
    }
    else {
        broadcast(&number, MPI_INT, np, myrank, 10, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        printf("Processo %d ha ricevuto %d elementi: ", myrank, count);
        for(int i = 0; i<10; i++) {
                printf("%d ", number[i]);
        }
        printf("\n");
    }        
      
    if (myrank == 0) {
        strcpy(&message[0], "Ciao bellissima");
        broadcast(&message, MPI_CHAR, np, myrank, strlen(message)+1, &status);
    }
    else {
        broadcast(&message, MPI_CHAR, np, myrank, sizeof(message), &status);
        MPI_Get_count(&status, MPI_CHAR, &count);
        printf("Processo %d ha ricevuto %d elementi: ", myrank, count);
        printf("%s\n", message);
    }
*/
    
    MPI_Finalize();
    return 0;
}
