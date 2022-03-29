#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

int main(int argc, char **argv) {

    int myrank, np;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int size = 10, local_min, global_min;
    int *numbers = NULL;
    int *local_mins = NULL;

    if (myrank == 0) {
        numbers = malloc(sizeof(int) * size);
        local_mins = malloc(sizeof(int) * np);
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
    local_min = local_numbers[0];
    for (int i = 1; i < process_size; i++) {
        if (local_numbers[i] < local_min) {
            local_min = local_numbers[i];
        }
    }
    printf("\nProcess %d - local min: %d\n", myrank, local_min);

    // raccogliamo con una gather tutti i massimi locali nell'array "local_maxs"
    MPI_Gather(&local_min, 1, MPI_INT, local_mins, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // solo il master calcolerà il massimo globale
    if (myrank == 0) {
        global_min = local_mins[0];
        for (int i = 1; i < np; i++) {
            if (local_mins[i] < global_min) {
                global_min = local_mins[i];
            }
        }
        printf("\nGlobal min: %d\n", global_min);
    }
    
    MPI_Finalize();

    return 0;
}
