#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>

# define N 100

int main(int argc, char **argv) {
    
    int myrank, np, count;
    int number[N];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int divisione = N / (np-1);
    int resto = N % (np-1);

    if (myrank == 0) {
        for (int i = 0; i < N; i++) {
            number[i] = i;
        }

        if (resto == 0) {
            int sub[divisione];
            for (int rank = 1; rank < np; rank++) {
                memcpy(&sub, &number[(rank-1) * divisione], divisione * sizeof(*number));
                MPI_Send(sub, divisione, MPI_INT, rank, rank, MPI_COMM_WORLD);
            }
             
        }
        else {
            int sub[divisione];
            int sub_extra[divisione + resto];
            printf("resto: %d\n", resto);
            for (int rank = 1; rank < np; rank++) {
                if (rank == (np-1)) {
                    memcpy(&sub_extra, &number[(rank-1) * divisione], (divisione + resto) * sizeof(*number));
                    MPI_Send(sub_extra, divisione + resto, MPI_INT, rank, rank, MPI_COMM_WORLD);
                } else {
                    memcpy(&sub, &number[(rank-1) * divisione], divisione * sizeof(*number));
                    MPI_Send(sub, divisione, MPI_INT, rank, rank, MPI_COMM_WORLD);
                }
            }
            
        }
    }
    else if (myrank == (np-1)) {
        int sub_extra[divisione+resto];
        MPI_Recv(sub_extra, divisione+resto, MPI_INT, 0, myrank, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        printf("Processo %d ha ricevuto %d interi\n", myrank, count);
    }
    
    else {
        int sub[divisione];
        MPI_Recv(sub, divisione, MPI_INT, 0, myrank, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        printf("Processo %d ha ricevuto %d interi\n", myrank, count);
    }
    


    MPI_Finalize();

    return 0;
}
