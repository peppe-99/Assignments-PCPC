#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define N 5
#define M 7

int main(int argc, char *argv[]) {
    
    int rank, np;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int matrix[N][M];

    int collumn_for_process = M / np;
    int collumn_extra = M % np;

    int global_mins[M];
    int collums_to_send[np];
    int displs[np];

    if (rank == 0) {
        srand(rank);
        printf("Matrix:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                matrix[i][j] = i * M + j;
                // matrix[i][j] = rand() % 101;
                printf("%d\t", matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        int extra = collumn_extra;
        int index = 0;
        for (int rank = 0; rank < np; rank++, extra--) {
            collums_to_send[rank] = (extra > 0) ? collumn_for_process + 1 : collumn_for_process;

            // "aggiorniamo" il tipo colonna
            MPI_Datatype collumn_type;
            MPI_Type_vector(N, collums_to_send[rank], M, MPI_INT, &collumn_type);
            MPI_Type_commit(&collumn_type);

            MPI_Send(&matrix[0][index], 1, collumn_type, rank, rank, MPI_COMM_WORLD);
            displs[rank] = index;
            index += collums_to_send[rank];

            MPI_Type_free(&collumn_type);
        }
    }

    int collums_to_receive = (rank + 1 <= collumn_extra) ? collumn_for_process + 1 : collumn_for_process;
    int collumns[N][collums_to_receive];
    int local_mins[collums_to_receive];

    MPI_Recv(collumns, N*collums_to_receive, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    printf("Processo %d\n", rank);
    for (int j = 0; j < collums_to_receive; j++){
        local_mins[j] = collumns[0][j];
        for (int i = 0; i < N; i++) {
            printf("%d\t", collumns[i][j]);
            if (collumns[i][j] < local_mins[j]) local_mins[j] = collumns[i][j]; 
        }
        printf("\n");
    }
    printf("\n");

    MPI_Gatherv(local_mins, collums_to_receive, MPI_INT, global_mins, collums_to_send, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < M; i++) {
            printf("Min collumn %d: %d\n", i, global_mins[i]);
        }
    }    

    MPI_Finalize();

    return 0;
}