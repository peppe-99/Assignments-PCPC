#include<stdio.h>
#include<mpi.h>
#include<string.h>
#include"mycollective.h"

#define N 100000

int main(int argc, char **argv) {

    int myrank, np;
    char message[256];
    double start, end;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);


    if (rank == 0) {
        printf("*-----------TEST BROADCAST-----------*\n");
        printf("PROCESSORI: %d N: %d ", np, size);
        fflush();
    }
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    if (rank == 0) {
        broadcast(&number, MPI_INT, np, myrank, size, &status);
    }
    else {
        broadcast(&number, MPI_INT, np, myrank, size, &status);
    }

    MPI_Barrier(MPI_COMM_WORLD)
    end = MPI_Wtime();
    if (myrank == 0) {
        printf("Time in ms = %f\n", end-start);
    }

    if (rank == 0) {
        printf("*-----------TEST SCATTER-----------*\n");
        printf("PROCESSORI: %d N: %d ", np, N);
        fflush();
    }
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    if (rank == 0) {
        scatter(&number, MPI_INT, np, myrank, N, sizeof(int), &status);
    }
    else {
        scatter(&number, MPI_INT, np, myrank, N, sizeof(int), &status);
    }

    MPI_Barrier(MPI_COMM_WORLD)
    end = MPI_Wtime();
    if (myrank == 0) {
        printf("Time in ms = %f\n", end-start);
    }  
    
    MPI_Finalize();
    return 0;
}
