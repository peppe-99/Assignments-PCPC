#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>
#include"mycollective.h"

int main(int argc, char *argv[]) {

    int myrank, np;
    int min, max, num;
    float avg;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (myrank == 0) {
        int min;
        min_reduce(&min, num, np, myrank);
        printf("\nMin reduce: %d", min);
        max_reduce(&max, num, np, myrank);
        printf("Max reduce: %d\n", max);
        avg_reduce(&avg, num, np, myrank);
        printf("Average reduce: %.2f\n", avg);
    }
    else {
        srand(time(NULL) * myrank);
        int num = rand() % 101;
        min_reduce(&min, num, np, myrank);
        max_reduce(&max, num, np, myrank);
        avg_reduce(&avg, num, np, myrank);
    }

    MPI_Finalize();    

    return 0;
}
