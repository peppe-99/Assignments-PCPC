#include<stdio.h>
#include<mpi.h>

int main(int argc, char *argv[]) {

    int np, myrank;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    int sum = myrank;
    int num_receive = myrank;
    int num_send = (myrank == np-1) ? 0 : myrank + 1;

    MPI_Request receive_requests[num_receive], send_requests[num_send];
    int received_ranks[num_receive];

    // Tutti i processi, tranno l'ultimo, inviano il proprio rank al successivo
    if (myrank != np-1) {
        MPI_Isend(&myrank, 1, MPI_INT, myrank+1, myrank+1, MPI_COMM_WORLD, &send_requests[myrank]);
    }

    // Ogni processo fa un numero di receive asincrone pari al proprio rank
    for (int i = 0; i < num_receive; i++) {
        MPI_Irecv(&received_ranks[i], 1, MPI_INT, myrank-1, myrank, MPI_COMM_WORLD, &receive_requests[i]);
    }

    // Man mano che le receive terminano, ogni processo invia al successivo, tranne l'ultimo, i rank ricevuti ed aggiorna la somma
    int complete;
    for (int i = 0; i < num_receive; i++) {
        MPI_Waitany(num_receive, receive_requests, &complete, MPI_STATUS_IGNORE);
        sum += received_ranks[complete];
        if (myrank != np-1) {
            MPI_Isend(&received_ranks[complete], 1, MPI_INT, myrank+1, myrank+1, MPI_COMM_WORLD, &send_requests[complete]);
        }
    }
    printf("Processo %d somma: %d\n\n", myrank, sum);
 
    MPI_Finalize();

    return 0;
}
