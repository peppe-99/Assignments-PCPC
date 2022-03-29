#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>
#include"mycollective.h"

void broadcast(void * buf, MPI_Datatype type, int np, int rank, int size, MPI_Status *status) {
    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Send(buf, size, type, i, i, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(buf, size*4, type, 0, rank, MPI_COMM_WORLD, status);
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

void scatter(void * buf, MPI_Datatype type, int np, int rank, int size, int byte, MPI_Status *stauts) {
    int divisione = size / (np-1);
    int resto = size % (np-1);

    if (rank == 0) {
        void *sub = (void*)malloc(divisione * byte);
        for (int rank = 1; rank < np; rank++) {
            memcpy(sub, &buf[(rank - 1) * divisione * byte], divisione * byte);
	    MPI_Send(sub, divisione, type, rank, rank, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(buf, divisione, type, 0, rank, MPI_COMM_WORLD, stauts);
    }
}
