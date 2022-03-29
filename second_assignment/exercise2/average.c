#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

int main(int argc, char **argv) {

    int myrank, np;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int size = 10;
    int *numbers = NULL;
    float *local_avgs = NULL;

    if (myrank == 0) {
        numbers = malloc(sizeof(int) * size);
        local_avgs = malloc(sizeof(float) * np);
        srand(time(NULL));
        printf("numbers: ");
        for (int i = 0; i < size; i++) {
            numbers[i] = rand() % 101;
            printf("%d ", numbers[i]);
        }
        printf("\n");
    }

    int sendcounts[np], displs[np];
    int resto = size % np;
    int size_per_process = size / np;
    int increment = 0;

    for (int rank = 0; rank < np; rank++) {
        displs[rank] = increment;
        sendcounts[rank] = (rank + 1 <= resto) ? size_per_process + 1 : size_per_process;
        increment += sendcounts[rank];
    }
    int process_size = sendcounts[myrank];  // la lunghezza dell'array locale di ogni processo
    int local_numbers[process_size];        // l'array locale di ogni processo

    // ogni processo riceverà un array "local_numbers" di lunghezza "process_size"
    MPI_Scatterv(numbers, sendcounts, displs, MPI_INT, local_numbers, process_size, MPI_INT, 0, MPI_COMM_WORLD);
    
    // ogni processo calcolerà il massimo locale
    float local_sum = 0;
    for (int i = 0; i < process_size; i++) {
        local_sum += local_numbers[i];
    }
    float local_avg = local_sum / process_size;
    printf("\nProcess %d - local avg: %.2f\n", myrank, local_avg);

    // raccogliamo con una gather tutti i massimi locali nell'array "local_maxs"
    MPI_Gather(&local_avg, 1, MPI_FLOAT, local_avgs, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    // solo il master calcolerà il massimo globale
    if (myrank == 0) {
        float global_sum = 0;
        for (int i = 0; i < np; i++) {
            global_sum += local_avgs[i];
        }
        float global_avg = global_sum / np;
        printf("\nGlobal average: %.2f\n", global_avg);
    }
    
    MPI_Finalize();

    return 0;
}