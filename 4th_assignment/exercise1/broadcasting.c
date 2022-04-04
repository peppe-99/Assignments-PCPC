#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<assert.h>
#include<limits.h>

#define N 640000000

int main(int argc, char *argv[]) {

    int np, rank;
    double start, end;
    
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
    MPI_Finalize();

    if (rank == 0) {
        printf("Time in ms = %f with %d process\n", end-start, np);
    }
    
    return 0;
}
