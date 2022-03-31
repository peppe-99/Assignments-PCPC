#include<mpi.h>

extern void broadcast(void * buf, MPI_Datatype type, int np, int rank, int size, MPI_Status *status);

extern void gather(void * buf, MPI_Datatype type, int np, int rank, int byte, MPI_Status *status);

extern void scatter(void * buf, MPI_Datatype type, int np, int rank, int size, int byte, MPI_Status *status);

extern void max_reduce(int *max, int num, int np, int rank);

extern void min_reduce(int *min, int num, int np, int rank);

extern void avg_reduce(float *avg, int num, int np, int rank);