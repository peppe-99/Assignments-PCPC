#include<stdio.h>
#include<mpi.h>
#include"mycollective.h"

void broadcast(void * buf, MPI_Datatype type, int np, int rank, int size, MPI_Status *status) {
    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Send(buf, size, type, i, i, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(buf, size, type, 0, rank, MPI_COMM_WORLD, status);
    }
}

void gather(void * buf, MPI_Datatype type, int np, int rank, int byte, MPI_Status *status) {
    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Recv(&buf[(i-1) * byte], 1, type, i, i, MPI_COMM_WORLD, status);
        }
    }
    else {
       MPI_Send(buf, 1, type, 0, rank, MPI_COMM_WORLD);
    }
}
