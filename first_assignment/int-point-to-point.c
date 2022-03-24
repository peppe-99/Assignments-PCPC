//Scambio di un valore intero tra due processi MPI. 
#include<stdio.h>
#include<mpi.h>

int main(int argc, char **argv) {
    int rank, processors, num, tag = 23, dest = 1;
    MPI_Status status;

    // inizializziamo MPI
    MPI_Init(&argc, &argv);
    // otteniamo il rank del processore
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    // otteniamo il numero di processori
    MPI_Comm_size(MPI_COMM_WORLD, &processors);

    if (rank == 0) {
        num = 3;
        MPI_Send(&num, 1, MPI_INT, dest, tag, MPI_COMM_WORLD);
    } 
    else if (rank == 1) {
        MPI_Recv(&num, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        printf("I'm first slave (rank %d). Receive: %d", rank, num);
    }
    
    MPI_Finalize();
    return 0;
}
