#include<stdio.h>
#include<mpi.h>

int main(int argc, char *argv[]) {
    
    int np, myrank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    int received_rank;
    int sum = myrank;
    int num_receive = myrank;
    int num_send = (myrank == np-1) ? 0 : myrank + 1;

    if (myrank != np-1) {
        MPI_Send(&myrank, 1, MPI_INT, myrank + 1, myrank + 1, MPI_COMM_WORLD);
    }
    for (int i = 0; i < num_receive; i++) {
        MPI_Recv(&received_rank, 1, MPI_INT, myrank - 1, myrank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        sum += received_rank;
        if (myrank != np-1) {
            MPI_Send(&received_rank, 1, MPI_INT, myrank + 1, myrank + 1, MPI_COMM_WORLD);
        }
    }
    printf("Processo %d somma: %d\n\n", myrank, sum);

    MPI_Finalize();

    return 0;
}
