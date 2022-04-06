#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define N 7
#define M 5

int main(int argc, char *argv[]){
    
    int np, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int matrice[N][M];
    
    // Definiamo il tipo riga
    MPI_Datatype row_type;
    // Si tratta di un blocco di M elementi contigui. Più precisamente
    MPI_Type_vector(1, M, M, MPI_INT, &row_type); 
    MPI_Type_commit(&row_type);

    int rows_for_process = N / np;
    int rows_extra = N % np;

    int global_maxs[N];     // i massimi di ogni riga
    int rows_to_send[np];   // righe da inviare a / ricevere da ogni processo
    int displs[np];

    if (rank == 0) {
        srand(rank);
        printf("Matrix:\n");
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                matrice[i][j] = rand() % 101;
                printf("%d\t", matrice[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        // Bilanciamo le righe tra i diversi processi. Ci saranno processi che avranno una riga in più 
        int extra = rows_extra;     // righe extra da distribuire
        int index = 0;              // indice della riga da cui partire per inviare
        for (int rank = 0; rank < np; rank++, extra--) {
            rows_to_send[rank] = (extra > 0) ? rows_for_process + 1 : rows_for_process;
            MPI_Send(&matrice[index][0], rows_to_send[rank], row_type, rank, rank, MPI_COMM_WORLD);
            displs[rank] = index;
            index += rows_to_send[rank];
        }
    }

    // I primi "rows_extra" processi riceveranno una riga in più
    int rows_to_receive = (rank + 1 <= rows_extra) ? rows_for_process + 1 : rows_for_process;
    int rows[rows_to_receive][M];
    int local_maxs[rows_to_receive];

    // Ogni processo riceverà M * rows_to_receive interi, ovvero le righe che gli spettano
    MPI_Recv(rows, M*rows_to_receive, MPI_INT, 0, rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    for (int i = 0; i < rows_to_receive; i++) {
        local_maxs[i] = 0;
        for (int j = 0; j < M; j++) {
            if (local_maxs[i] < rows[i][j]) local_maxs[i] = rows[i][j];
        }
    }

    MPI_Gatherv(local_maxs, rows_to_receive, MPI_INT, global_maxs, rows_to_send, displs, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < N; i++) {
            printf("Max row %d: %d\n", i, global_maxs[i]);
        }
        printf("\n");
    }
        
    MPI_Finalize();

    return 0;
}