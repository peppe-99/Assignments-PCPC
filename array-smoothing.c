#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>

#define N 14
#define K 2

int main(int argc, char *argv[]) {
    
    int np, rank, A[N], tag = 28;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (rank == 0) {
        printf("Original: ");
        for (int i = 0; i < N; i++) {
            A[i] = i;
            printf("%d ", A[i]);
        }
        printf("\n\n");
    }

    int divisione = N / (np - 1);
    int resto = N % (np - 1);
    int sendcounts[np], displs[np];

    for (int rank = 1, i = 0; rank < np; rank++) {
        displs[rank] = i;
        sendcounts[rank] = (rank <= resto) ? divisione + 1 : divisione;
        i += sendcounts[rank];
    }
    // Il master non deve ricevere nulla
    displs[0] = 0;      
    sendcounts[0] = 0;

    int process_size = sendcounts[rank];
    int subA[process_size];
    
    MPI_Scatterv(A, sendcounts, displs, MPI_INT, subA, process_size, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank > 0) {
        printf("Slave %d: ", rank);
        for (int i = 0; i < process_size; i++) {
            printf("%d ", subA[i]);
        }
        printf("\n\n");

        int non_vincolati = (2*K >= process_size) ? 0 : process_size - 2*K;
        // printf("Slave %d ha %d elementi non vincolati\n\n", rank, non_vincolati);

        int precedenti[K], successivi[K];
        if (rank == 1) {
            MPI_Request send_next, receive_next;
            MPI_Isend(&subA[process_size - K], K, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &send_next);
            MPI_Irecv(successivi, K, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &receive_next);

            /* calcolare lo smoothing dei non vincolati */

            MPI_Wait(&receive_next, MPI_STATUS_IGNORE);
            printf("Slave %d - successivi: ", rank);
            for (int i = 0; i < K; i++) {
                printf("%d ", successivi[i]);
            }
            printf("\n\n");
            
        }
        else if(rank == np-1) {
            MPI_Request send_back, receive_back;
            MPI_Isend(&subA[0], K, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &send_back);
            MPI_Irecv(precedenti, K, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &receive_back); 

            /* calcolare lo smoothing dei non vincolati */

            MPI_Wait(&receive_back, MPI_STATUS_IGNORE);
            printf("Slave %d - precedenti: ", rank);
            for (int i = 0; i < K; i++) {
                printf("%d ", precedenti[i]);
            }
            printf("\n\n");

        }
        else {
            MPI_Request send_next, receive_next, send_back, receive_back;
            MPI_Isend(&subA[0], K, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &send_next);
            MPI_Isend(&subA[process_size - K], K, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &send_back);
            MPI_Irecv(successivi, K, MPI_INT, rank+1, tag, MPI_COMM_WORLD, &receive_next);
            MPI_Irecv(precedenti, K, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &receive_back);

            /* calcolare lo smoothing dei non vincolati */

            MPI_Wait(&receive_next, MPI_STATUS_IGNORE);
            printf("Slave %d - successivi: ", rank);
            for (int i = 0; i < K; i++) {
                printf("%d ", successivi[i]);
            }
            printf("\n");
            MPI_Wait(&receive_back, MPI_STATUS_IGNORE);
            printf("Slave %d - precedenti: ", rank);
            for (int i = 0; i < K; i++) {
                printf("%d ", precedenti[i]);
            }
            printf("\n\n");
        }
        

    }
    
    




    MPI_Finalize();

    return 0;
}
