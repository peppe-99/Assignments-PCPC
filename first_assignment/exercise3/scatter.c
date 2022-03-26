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
            int *sub = (int*)malloc(divisione * sizeof(int));
            for (int rank = 1; rank < np; rank++) {
                memcpy(sub, &number[(rank-1) * divisione], divisione * sizeof(int));
                MPI_Send(sub, divisione, MPI_INT, rank, rank, MPI_COMM_WORLD);
            }
            // free(sub);
             
        }
        else {
            int *sub = (int*)malloc(divisione * sizeof(int));
            int *sub_extra = (int*)malloc((divisione + resto) * sizeof(int));
            printf("resto: %d\n", resto);
            for (int rank = 1; rank < np; rank++) {
                if (rank == (np-1)) {
                    memcpy(sub_extra, &number[((rank-1) * divisione)], ((divisione + resto) * sizeof(int)));
                    MPI_Send(sub_extra, divisione + resto, MPI_INT, rank, rank, MPI_COMM_WORLD);
                } else {
                    memcpy(sub, &number[(rank-1) * divisione], divisione * sizeof(int));
                    MPI_Send(sub, divisione, MPI_INT, rank, rank, MPI_COMM_WORLD);
                }
            }
            //free(sub);
            //free(sub_extra);
        }
    }
    else if (myrank == (np-1)) {
        int sub_extra[divisione+resto];
        MPI_Recv(sub_extra, divisione+resto, MPI_INT, 0, myrank, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        printf("Processo %d ha ricevuto %d interi\n", myrank, count);
        for (int i = 0; i < divisione+resto; i++) {
            printf("%d ", sub_extra[i]);
        }
        printf("\n");
    }
    
    else {
        int sub[divisione];
        MPI_Recv(sub, divisione, MPI_INT, 0, myrank, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        printf("Processo %d ha ricevuto %d interi\n", myrank, count);
        for (int i = 0; i < divisione; i++) {
            printf("%d ", sub[i]);
        }
        printf("\n");
    }
    


    MPI_Finalize();

    return 0;
}
