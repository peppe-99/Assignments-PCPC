#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>
#include"mycollective.h"

void broadcast(void * buf, MPI_Datatype type, int np, int rank, int size, MPI_Status *status) {
    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Send(buf, size, type, i, i, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(buf, size*4, type, 0, rank, MPI_COMM_WORLD, status);
    }
}

void gather(void * buf, MPI_Datatype type, int np, int rank, int byte, MPI_Status *status) {
    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Recv(&buf[(i-1) * byte], 1, type, i, i, MPI_COMM_WORLD, status);
        }
    }
    else {
       MPI_Send(buf, 1, type, 0, rank, MPI_COMM_WORLD);
    }
}

void scatter(void * buf, MPI_Datatype type, int np, int rank, int size, int byte, MPI_Status *stauts) {
    int divisione = size / (np-1);
    int resto = size % (np-1);

    if (rank == 0) {
        void *sub = (void*)malloc(divisione * byte);
        for (int rank = 1; rank < np; rank++) {
            memcpy(sub, &buf[(rank - 1) * divisione * byte], divisione * byte);
	        MPI_Send(sub, divisione, type, rank, rank, MPI_COMM_WORLD);
        }
    }
    else {
        MPI_Recv(buf, divisione, type, 0, rank, MPI_COMM_WORLD, stauts);
    }
}

void min_reduce(int *min, int num, int np, int rank) {
    MPI_Status status;

    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Recv(&num, 1, MPI_INT, rank, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            printf("%d ", num);
            if (i == 1) *min = num;
            else if (num < *min) *min = num;  
        }
    }
    else {
        MPI_Send(&num, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD);
    }
}

void max_reduce(int *max, int num, int np, int rank) {
    MPI_Status status;
    *max = 0;

    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Recv(&num, 1, MPI_INT, rank, rank, MPI_COMM_WORLD, &status);
            printf("%d ", num);
            if (num > *max) *max = num;            
        }
    }
    else {
        MPI_Send(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }
}

void avg_reduce(float *avg, int num, int np, int rank) {
    MPI_Status status;
    float sum = 0;

    if (rank == 0) {
        for (int i = 1; i < np; i++) {
            MPI_Recv(&num, 1, MPI_INT, rank, rank, MPI_COMM_WORLD, &status);
            printf("%d ", num);
            sum += num;        
        }
        *avg = sum / np-1;
    }
    else {
        MPI_Send(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
    }
}

void Ibroadcast(void * buf, MPI_Datatype type, int np, int rank, int size, MPI_Status *status) {
    if (rank == 0) {
        MPI_Request requests[np-1];
        MPI_Status status[np-1];
        for (int i = 1; i < np; i++) {
            MPI_Isend(buf, size, type, i, i, MPI_COMM_WORLD, &requests[i-1]);
        }
        MPI_Waitall(np-1, requests, status);
    }
    else {
        MPI_Request request;
        MPI_Irecv(buf, size, type, 0, rank, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, status);
    }    
}

void Igather(void * buf, MPI_Datatype type, int np, int rank, int byte, MPI_Status *status) {
    if (rank == 0) {
        MPI_Request requests[np-1];
        MPI_Status status[np-1];
        for (int i = 1; i < np; i++) {
            MPI_Irecv(&buf[(i-1) * byte], 1, type, i, i, MPI_COMM_WORLD, &requests[i]);
        }
        MPI_Waitall(np-1, requests, status);
    }
    else {
        MPI_Request request;
        MPI_Isend(buf, 1, type, 0, rank, MPI_COMM_WORLD, &request);
        MPI_Wait(&request, status);
    }
}

void Iscatter(void * buf, MPI_Datatype type, int np, int rank, int size, int byte, MPI_Status *stauts) {
    int divisione = size / (np-1);
    int resto = size % (np-1);

    if (rank == 0) {        
        MPI_Request requests[np-1];
        MPI_Status status[np-1];
        if (resto == 0){
            void *sub = (void*)malloc(divisione * byte);
            for (int rank = 1; rank < np; rank++) {
                memcpy(sub, &buf[(rank - 1) * divisione * byte], divisione * byte);
	            MPI_Isend(sub, divisione, type, rank, rank, MPI_COMM_WORLD, &requests[rank-1]);
            }
        }
        else {
            void *sub = (void*)malloc(divisione * byte);
            void *sub_extra = (void*)malloc((divisione + resto) * byte);
            for (int rank = 1; rank < np; rank++) {
                if (rank == (np-1)) {
                    memcpy(sub_extra, &buf[(rank - 1) * divisione * byte], (divisione + resto) * byte);
                    MPI_Isend(sub_extra, divisione+resto, type, rank, rank, MPI_COMM_WORLD, &requests[rank-1]);
                }
                else {
                    memcpy(sub, &buf[(rank - 1) * divisione * byte], divisione * byte);
                    MPI_Isend(sub, divisione, type, rank, rank, MPI_COMM_WORLD, &requests[rank-1]);
                }
            }
        }
        MPI_Waitall(np-1, requests, status);
    }
    else {
        MPI_Request request;
        if (resto != 0 && rank == (np-1)) {
            MPI_Irecv(buf, divisione+resto, type, 0, rank, MPI_COMM_WORLD, &request);        
        }
        else {
            MPI_Irecv(buf, divisione, type, 0, rank, MPI_COMM_WORLD, &request);
        }
        MPI_Wait(&request, stauts);
    }
}

void Imax_reduce(int *max, int num, int np, int rank) {
    if (rank == 0) {
        *max = 0;
        MPI_Request requests[np-1];
        MPI_Status status[np-1];
        for (int i = 1; i < np; i++) {
            MPI_Irecv(&num, 1, MPI_INT, i, i, MPI_COMM_WORLD, &requests[i-1]);
            MPI_Wait(&requests[i-1], &status[i-1]);
            if (num > *max) *max = num;
        }
    }
    else {
        MPI_Request request;
        MPI_Isend(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &request);
    }
}

void Imin_reduce(int *min, int num, int np, int rank) {
    if (rank == 0) {
        MPI_Request requests[np-1];
        MPI_Status status[np-1];
        for (int i = 1; i < np; i++) {
            MPI_Irecv(&num, 1, MPI_INT, i, i, MPI_COMM_WORLD, &requests[i-1]);
            MPI_Wait(&requests[i-1], &status[i-1]);
            if (i == 1) *min = num
            if (num < *min) *min = num;
        }
    }
    else {
        MPI_Request request;
        MPI_Isend(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &request);
    }
}

void Iavg_reduce(float *avg, int num, int np, int rank) {
    if (rank == 0) {
        MPI_Request requests[np-1];
        MPI_Status status[np-1];
        float sum = 0;
        for (int i = 1; i < np; i++) {
            MPI_Irecv(&num, 1, MPI_INT, i, i, MPI_COMM_WORLD, &requests[i-1]);
            MPI_Wait(&requests[i-1], &status[i-1]);
            sum += num;
        }
        *avg = sum / np-1;
    }
    else {
        MPI_Request request;
        MPI_Isend(&num, 1, MPI_INT, 0, rank, MPI_COMM_WORLD, &request);
    }
}