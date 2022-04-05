#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<assert.h>
#include<limits.h>
#include<string.h>

#define N 640000000

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

int main(int argc, char *argv[]) {

    int np, rank;
    double start, end;
    MPI_Status status;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);  
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    float* numbers = (float*)malloc(sizeof(float) * N);
    assert(numbers != NULL);

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    MPI_Bcast(numbers, N, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    free(numbers);

    numbers = (float*)malloc(sizeof(float) * N);
    assert(numbers != NULL);

    if (rank == 0) {
        printf("MPI broadcast time in ms = %f with %d process\n", end-start, np);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    broadcast(numbers, MPI_FLOAT, np, rank, N, &status);
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    MPI_Finalize();

    if (rank == 0) {
        printf("My broadcast time in ms = %f with %d process\n", end-start, np);
    }
    
    return 0;
}


