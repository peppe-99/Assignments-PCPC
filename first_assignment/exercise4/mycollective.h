#include<mpi.h>

extern void broadcast(void * buf, MPI_Datatype type, int np, int rank, int size, MPI_Status status);