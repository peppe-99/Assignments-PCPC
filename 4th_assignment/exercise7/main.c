#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>
#include<time.h>

#define N 10

int main(int argc, char *argv[]) {
    
    int np, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int data[N];
    int sendcounts[np], displs[np];
    int resto = N % (np - 1);
    int size_for_process = N / (np - 1);

    int disp = 0;
    for (int rank = 0; rank < np; rank++) {
        displs[rank] = disp;
        if (rank == 0) {
            sendcounts[rank] = 0;
        }
        else if (rank == np-1) {
            sendcounts[rank] = size_for_process + resto;
        }
        else {
            sendcounts[rank] = size_for_process;
        }
        disp += sendcounts[rank];
    }
    int process_size = sendcounts[rank];
    int sub_data[process_size], new_sub_data[process_size];    

    if (rank == 0) {
        srand(time(NULL));
        printf("Original array: ");
        for (int i = 0; i < N; i++) {
            data[i] = rand() % 101;
            printf("%d ", data[i]);
        }
        printf("\n\n");fflush(stdout);
    }

    MPI_Scatterv(data, sendcounts, displs, MPI_INT, sub_data, process_size, MPI_INT, 0, MPI_COMM_WORLD);

    printf("Processo %d: ", rank);
    for (int i = 0; i < process_size; i++) {
        printf("%d ", sub_data[i]);
    }
    printf("\n");
    
    if (rank != 0) {
        int next_num, prev_num;
        MPI_Request next_send_request, next_receive_request, prev_send_request, prev_receive_request;
        MPI_Request receive_requests[2]; // 0 is prev and 1 is next

        int next = (rank != np-1) ? rank + 1 : 1;
        int prev = (rank != 1) ? rank - 1 : np-1;

        int to_send_next = sub_data[process_size-1];
        int to_send_prev = sub_data[0];

        memcpy(new_sub_data, sub_data, process_size * sizeof(int));

        // inviamo l'ultimo elemento al successivo
        MPI_Isend(&to_send_next, 1, MPI_INT, next, next, MPI_COMM_WORLD, &next_send_request);
        // riceviamo dal precedente il suo ultimo elemento
        MPI_Irecv(&prev_num, 1, MPI_INT, prev, rank, MPI_COMM_WORLD, &receive_requests[0]);
        // inviamo il primo elemento al precedente
        MPI_Isend(&to_send_prev, 1, MPI_INT, prev, prev, MPI_COMM_WORLD, &prev_send_request);
        // riceviamo dal successivo il suo primo elemento
        MPI_Irecv(&next_num, 1, MPI_INT, next, rank, MPI_COMM_WORLD, &receive_requests[1]);

        for (int i = 1; i < process_size-1; i++) {
            new_sub_data[i] += sub_data[i-1] + sub_data[i+1];
        }
    
        int complete;
        MPI_Waitany(2, receive_requests, &complete, MPI_STATUS_IGNORE);
        if (complete == 0) {
            new_sub_data[0] += prev_num + sub_data[1];
            MPI_Wait(&receive_requests[(complete + 1) % 2], MPI_STATUS_IGNORE);
            new_sub_data[process_size-1] += sub_data[process_size-2] + next_num;
        }
        else {
            new_sub_data[process_size-1] += sub_data[process_size-2] + next_num;
            MPI_Wait(&receive_requests[(complete + 1) % 2], MPI_STATUS_IGNORE);
            new_sub_data[0] += prev_num + sub_data[1];
        }
    }

    MPI_Gatherv(new_sub_data, process_size, MPI_INT, data, sendcounts, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\nNew array: ");
        for (int i = 0; i < N; i++) {
            printf("%d ", data[i]);
        }
        printf("\n\n");
    }

    MPI_Finalize();

    return 0;
}
