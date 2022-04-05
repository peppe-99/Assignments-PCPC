#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define N 1000000000


int main(int argc, char *argv[]) {
    
    int np, rank;
    double start, end;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    float* send_data = (float*)malloc(sizeof(float) * N);
    float* receive_data = (float*)malloc(sizeof(float) * N / np);


    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    MPI_Scatter(send_data, N / np, MPI_FLOAT, receive_data, N / np, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();

    free(send_data);
    free(receive_data);

    MPI_Finalize();

    if (rank == 0) {
        printf("Time in ms = %f with %d process\n", end-start, np);
    }

    return 0;
}
