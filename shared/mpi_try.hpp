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
