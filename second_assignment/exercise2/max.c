#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<mpi.h>

int main(int argc, char **argv) {

    int myrank, np;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int size = 10, local_max = 0, global_max = 0;
    int *numbers = NULL;
    int *local_maxs = NULL;

    if (myrank == 0) {
        numbers = malloc(sizeof(int) * size);
        local_maxs = malloc(sizeof(int) * np);
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
    for (int i = 0; i < process_size; i++) {
        if (local_numbers[i] > local_max) {
            local_max = local_numbers[i];
        }
    }
    printf("\nProcess %d - local max: %d\n", myrank, local_max);

    // raccogliamo con una gather tutti i massimi locali nell'array "local_maxs"
    MPI_Gather(&local_max, 1, MPI_INT, local_maxs, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // solo il master calcolerà il massimo globale
    if (myrank == 0) {
        for (int i = 0; i < np; i++) {
            if (local_maxs[i] > global_max) {
                global_max = local_maxs[i];
            }
        }
        printf("\nGlobal max: %d\n", global_max);
    }
    
    MPI_Finalize();

    return 0;
}
    
    
    
    

    
    
/*

    divisione = np / n;
    resto = np % n;

    for(int rank = 0; rank < np; rank++){
       displs[processID] = increment;
       sendcounts[processID] = (processID + 1 <= res) ? size_per_process + 1 : size_per_process;
       increment += sendcounts[processID];
    }

    if (myrank == 0) {
        int number[n];
        srand(time(NULL));

        MPI_Scatter(number, 1, )
        

    }
    


    MPI_Finalize();

    return 0;
}
*/

