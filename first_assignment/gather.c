#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

int main(int argc, char **argv) {
    
    int myrank, np;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (myrank == 0) {
        int array[np-1];
        int count;
        int received = 0;
        for (int rank = 1; rank < np; rank++) {
            MPI_Recv(&array[rank-1], 1, MPI_INT, rank, rank, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);
            received += count;
        }
        printf("Received %d int\n", received);
        for (int i = 0; i < sizeof(array)/sizeof(int); i++) {
            printf("%d ", array[i]);
        }
        printf("\n");
    }
    else {
        srand(myrank * rand());
        int num = rand() % 51;
        MPI_Send(&num, 1, MPI_INT, 0, myrank, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();

    return 0;
}

// meglio prendere l'input da linea di comando (argc, argv)
