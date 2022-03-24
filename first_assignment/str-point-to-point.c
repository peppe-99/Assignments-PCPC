// 2. Invio di una stringa (letta da stdin), da parte del processo con rank 0, al processo con rank 1.

#include<stdio.h>
#include<string.h>
#include<mpi.h>

int main(int argc, char **argv){

    int rank, count, dest=1, source=0, tag=23;
    char message[256];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        printf("write something: "); fflush(stdout);
        scanf("%[^\n]", message);

        MPI_Send(message, strlen(message)+1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    }
    else if (rank == 1) {
        MPI_Recv(message, sizeof(message), MPI_CHAR, source, tag, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_CHAR, &count);
        printf("Process (%d) - Received %d char - Message: %s\n", rank, count-1, message);
    }

    MPI_Finalize();
    return 0;
}
