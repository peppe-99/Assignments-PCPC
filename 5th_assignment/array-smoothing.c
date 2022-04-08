#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<string.h>

#define N 13
#define K 2

int main(int argc, char *argv[]) {
    
    int np, rank, tag = 28;
    float A[N];

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (rank == 0) {
        srand(rank);
        printf("Original: ");
        for (int i = 0; i < N; i++) {
            A[i] = rand() % 101;
            printf("%2.f ", A[i]);
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
    float subA[process_size], smoothed_subA[process_size];
    
    MPI_Scatterv(A, sendcounts, displs, MPI_FLOAT, subA, process_size, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank > 0) {

        int next = (rank == np-1) ? 1 : rank + 1;
        int prev = (rank == 1) ? np - 1 : rank - 1;

        float precedenti[K], successivi[K];
        float sum = 0.0;

        MPI_Request send_to_next, send_to_prev;
        MPI_Request receives[2]; // 0 is from prev and 1 is from next

        MPI_Isend(&subA[process_size - K], K, MPI_FLOAT, next, tag, MPI_COMM_WORLD, &send_to_next);
        MPI_Isend(&subA[0], K, MPI_FLOAT, prev, tag, MPI_COMM_WORLD, &send_to_prev);
        MPI_Irecv(precedenti, K, MPI_INT, prev, tag, MPI_COMM_WORLD, &receives[0]);
        MPI_Irecv(successivi, K, MPI_FLOAT, next, tag, MPI_COMM_WORLD, &receives[1]);

        if (2*K < process_size) {
            /* calcoliamo lo smoothing non vincolati */
            for (int i = K; i < process_size - K; i++) {
                sum = 0.0;
                for (int j = i-K; j <= i+K; j++) {
                    sum += subA[j];
                }
                smoothed_subA[i] = sum / (2*K + 1);
            }

            /* controlliamo quale receive è completata */
            int complete;
            MPI_Waitany(2, receives, &complete, MPI_STATUS_IGNORE);

            /* calcoliamo lo smoothing dei primi K e ultimi K vincolati */
            if (complete == 0) {
                for (int i = 0; i < K; i++) {
                    sum = 0.0;
                    for (int j = i-K; j <= i+K; j++) {
                        sum += (j >= 0) ? subA[j] : precedenti[j + K];
                    }
                    smoothed_subA[i] = sum / (2*K + 1);
                }

                MPI_Wait(&receives[1], MPI_STATUS_IGNORE);

                for (int i = process_size - K; i < process_size; i++) {
                    sum = 0.0;
                    for (int j = i-K; j <= i+K; j++) {
                        sum += (j < process_size) ? subA[j] : successivi[j % process_size];
                    }
                    smoothed_subA[i] = sum / (2*K + 1);
                }
            }
            else {
                for (int i = process_size - K; i < process_size; i++) {
                    sum = 0.0;
                    for (int j = i-K; j <= i+K; j++) {
                        sum += (j < process_size) ? subA[j] : successivi[j % process_size];
                    }
                    smoothed_subA[i] = sum / (2*K + 1);
                }

                MPI_Wait(&receives[0], MPI_STATUS_IGNORE);

                for (int i = 0; i < K; i++) {
                    sum = 0.0;
                    for (int j = i-K; j <= i+K; j++) {
                        sum += (j >= 0) ? subA[j] : precedenti[j + K];
                    }
                    smoothed_subA[i] = sum / (2*K + 1);
                }
            }
        }
        else {
            printf("%d\n", rank);
            /* tutti gli elementi sono vincolati */
            /* aspettiamo entrambe le receive    */
            MPI_Waitall(2, receives, MPI_STATUSES_IGNORE);

            /* Calcoliamo lo smooting */
            for (int i = 0; i < process_size; i++) {
                sum = 0.0;
                for (int j = i-K; j <= i+K; j++) {                    
                    sum += (j < 0) ? precedenti[j + K] : (j >= process_size) ? successivi[j % process_size] : subA[j]; //if(..) else if(..) else
                }
                smoothed_subA[i] = sum / (2*K + 1);
            }
        }

        printf("Slave %d\nOriginal: ", rank);
        for (int i = 0; i < process_size; i++) {
            printf("%.2f ", subA[i]);
        }
        printf("\nSmoothed: ");
        for (int i = 0; i < process_size; i++) {
            printf("%.2f ", smoothed_subA[i]);
        }
        printf("\n\n");
    }

    MPI_Gatherv(smoothed_subA, process_size, MPI_FLOAT, A, sendcounts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Smoothed: ");
        for (int i = 0; i < N; i++) {
            printf("%.2f ", A[i]);
        }
        printf("\n");
    }

    MPI_Finalize();

    return 0;
}