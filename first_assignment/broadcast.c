#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define N 1000000

int main(int argc, char **argv) {

    int rank, np, count, number[N], source=0;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            // il tag sarà il rank del processo
            MPI_Send(number, sizeof(number)/sizeof(int), MPI_INT, i, i, MPI_COMM_WORLD);
        }
    } 
    else {
        MPI_Recv(number, sizeof(number)/sizeof(int), MPI_INT, source, rank, MPI_COMM_WORLD, &status);
        int rc = MPI_Get_count(&status, MPI_INT, &count);
        printf("Process %d - received %d int\n", rank, count);       
    }
    MPI_Finalize();

    return 0;
}