#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include"mycollective.h"

int main(int argc, char const *argv[]) {
    
    int rank, np, count;
    int numbers = (int*)malloc(sizeof(int) * 10);
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (rank == 0) {
        for (int i = 0; i < 10; i++) {
            numbers[i] = i;
        }
        Ibroadcast(numbers, MPI_INT, np, rank, 10, sizeof(int), &status);
    }
    else {
        Ibroadcast(numbers, MPI_INT, np, rank, 10, sizeof(int), &status);
        MPI_Get_count(&status, MPI_INT, &count);
        printf("Processo %d ha ricevuto %d interi\n", rank, count);
        for (int i = 0; i < 10; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");   
    }

    if (rank == 0) {
        Igather(numbers, MPI_INT, np, rank, sizeof(int), &status);
        printf("Gather: ")
        for (int i = 0; i < 10; i++) {
            printf("%d ", numbers[i]);
        }
        printf("\n");   
    }
    else {
        Igather(&numbers[rank-1], MPI_INT, np, rank, sizeof(int), &status);
    }
    
    
    MPI_Finalize();

    return 0;
}
