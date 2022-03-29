#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>

int main(int argc, char **argv) {

    int np, myrank, num = 0, getted_num, dest, round, rounds[10], iter = 0;
    int soglia = 500;
    MPI_Status status;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &np);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
 
    dest = (myrank + 1) % np;

    // settiamo il generatore per ogni processo usando come seed il rank del processo
    srand(myrank);

    if (myrank == 0) {
        printf("Soglia: %d\n", soglia);
    }
    
    for (iter = 0; iter < 10; iter++) {
        while (1) {
            if (myrank != 0) {                                                              // se il processo è uno slave     
                MPI_Recv(&getted_num, 1, MPI_INT, myrank - 1, 0, MPI_COMM_WORLD, &status);  // si blocca nel ricevere il numero dallo slave precedente

                if (getted_num <= soglia) {                                                 // se non abbiamo superato la soglia
                    num = getted_num + rand() % 101;                                        // aggiungiamo al numero ricevuto un numero casuale tra 0 e 100
                }
                else num = -1;                                                              // altrimenti settiamo num a -1 così da capire che dobbiamo fermarci

            }
            else { 
                if (num == 0) {
                    num = rand() % 101;   // il processo master inizializza il num di partezza con un numero casuale tra 0 e 100
                    round = 1;
                }
                round++;
            }

                                                                                                                
            if (num != -1) {                                            // ogni processo (sia master che slave)
                MPI_Send(&num, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);    // se non abbiamo raggiunto la soglia invia il num al processo successivo
            }
            else {
                MPI_Send(&getted_num, 1, MPI_INT, dest, 0, MPI_COMM_WORLD);
                break;
            }        
                                                                                                                
            if (myrank == 0) {                                                          // se il processo è il master si blocca nel ricevere 
                MPI_Recv(&getted_num, 1, MPI_INT, np - 1, 0, MPI_COMM_WORLD, &status);  // il numero dall'ultimo slave e fa gli ennesimi controlli 
                if (getted_num <= soglia) {                                             // sul raggiungimento o meno della soglia
                    num = getted_num + rand() % 101;
                }
                else {
                    num = -1;
                }
            }
        }
        num = 0;
        getted_num = 0;
        if (myrank == 0) {
            printf("\nIterazione %d terminata in %d round\n", iter+1, round);
            rounds[iter] = round;
        }
        round = 1;
    }

    if (myrank == 0) {
        int sum = 0;
        for (int i = 0; i < 10; i++) {
            sum += rounds[i];
        }
        printf("\nMedia round: %d\n", sum/10);
        
    }
    
    
    MPI_Finalize();
    return 0;
}
