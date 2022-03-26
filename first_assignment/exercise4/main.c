#include<stdio.h>
#include<mpi.h>
#include"mycollective.h"

int main(int argc, char **argv) {

    int myrank, np;
    int number[10];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    if (myrank == 0) {
        for(int i = 0; i < 10; i++) {
            number[i] = i;
        }
        broadcast(number, MPI_INT, np, myrank, status);
    }
    else {
        broadcast(&number, MPI_INT, np, myrank, status);
    }
    MPI_Get_count(&status, )
    


    MPI_Finalize();
    return 0;
}
