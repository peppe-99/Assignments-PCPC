#include<mpi.h>

extern void broadcast(void * buf, MPI_Datatype type, int np, int rank, int size, MPI_Status *status);

extern void gather(void * buf, MPI_Datatype type, int np, int rank, int byte, MPI_Status *status);

extern void scatter(void * buf, MPI_Datatype type, int np, int rank, int size, int byte, MPI_Status *status);