#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define N 7
#define M 5

int main(int argc, char *argv[]) {
    
    int np, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int matrix[N][M];

    MPI_Datatype row_type;    
    MPI_Type_vector(1, M, M, MPI_INT, &row_type); 
    MPI_Type_commit(&row_type);

    int rows_for_process = N / np;
    int rows_extra = N % np;
    int collumn_for_process = M / np;
    int collumn_extra = M % np;

    int global_maxs[N], global_mins[M];
    int rows_to_send[np], collums_to_send[np];
    int displs_rows[np], displs_collums[np];

    if (rank == 0) {
        srand(rank);
        printf("Matrix:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                matrix[i][j] = rand() % 101;
                printf("%d\t", matrix[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        int extra1 = rows_extra, extra2 = collumn_extra;
        int index_row = 0, index_collumn = 0;
        for (int rank = 0; rank < np; rank++, extra1--, extra2--) {
            rows_to_send[rank] = (extra1 > 0) ? rows_for_process + 1 : rows_for_process;
            collums_to_send[rank] = (extra2 > 0) ? collumn_for_process + 1 : collumn_for_process;

            MPI_Send(&matrix[index_row][0], rows_to_send[rank], row_type, rank, rank, MPI_COMM_WORLD);
            displs_rows[rank] = index_row;
            index_row += rows_to_send[rank];

            MPI_Datatype collumn_type;
            MPI_Type_vector(N, collums_to_send[rank], M, MPI_INT, &collumn_type);
            MPI_Type_commit(&collumn_type);

            MPI_Send(&matrix[0][index_collumn], 1, collumn_type, rank, rank, MPI_COMM_WORLD);
            displs_collums[rank] = index_collumn;
            index_collumn += collums_to_send[rank];

            MPI_Type_free(&collumn_type);
        }
    }

    int rows_to_receive = (rank + 1 <= rows_extra) ? rows_for_process + 1 : rows_for_process;
    int collums_to_receive = (rank + 1 <= collumn_extra) ? collumn_for_process + 1 : collumn_for_process;
    int rows[rows_to_receive][M], collumns[N][collums_to_receive];
    int local_maxs[rows_to_receive], local_mins[collums_to_receive];

    MPI_Recv(rows, M*rows_to_receive, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(collumns, N*collums_to_receive, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 0; i < rows_to_receive; i++) {
        local_maxs[i] = 0;
        for (int j = 0; j < M; j++) {
            if (local_maxs[i] < rows[i][j]) local_maxs[i] = rows[i][j];
        }
    }

    for (int j = 0; j < collums_to_receive; j++){
        local_mins[j] = collumns[0][j];
        for (int i = 0; i < N; i++) {
            if (collumns[i][j] < local_mins[j]) local_mins[j] = collumns[i][j]; 
        }
    }

    MPI_Gatherv(local_maxs, rows_to_receive, MPI_INT, global_maxs, rows_to_send, displs_rows, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gatherv(local_mins, collums_to_receive, MPI_INT, global_mins, collums_to_send, displs_collums, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            printf("Max row %d: %d\n", i, global_maxs[i]);
        }
        for (int i = 0; i < M; i++) {
            printf("Min collumn %d: %d\n", i, global_mins[i]);
        }
    }

    MPI_Finalize();

    return 0;
}
