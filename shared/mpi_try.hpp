#include <iostream>
#include <mpi.h>

#define TRY(func, error_msg) do { \
    int error_code = func; \
    if(error_code != MPI_SUCCESS) \
    { \
        std::cerr << error_msg << ", error_code = " << error_code << std::endl; \
        MPI_Abort(MPI_COMM_WORLD, error_code); \
    } \
} while(false)

#define BARRIER TRY(MPI_Barrier(MPI_COMM_WORLD), "Can't create a barrier")

#define MPI_SYNC(func) do { \
    for(int i = 0; i < world_size; i++) \
    { \
        BARRIER; \
        if (rank == i) { func; } \
    } \
} while(false)
