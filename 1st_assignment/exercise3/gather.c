#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

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
        gather(&number, MPI_INT, np, myrank, np-1, sizeof(int), &status);
        for (int i = 0; i<np-1; i++) {
            printf("%d ", number[i]);
        }
        printf("\n");

    }
    else {
        char x = 'c';
        gather(&myrank, MPI_INT, np, myrank, 1, sizeof(int), &status);
    }
    
    MPI_Finalize();

    return 0;
}

// meglio prendere l'input da linea di comando (argc, argv)
