#include <mpi.h>
#include <iostream>

#define TRY(func, error_msg) do { \
    int error_code = func; \
    if(error_code != MPI_SUCCESS) \
    { \
        std::cerr << error_msg << ", error_code = " << error_code << std::endl; \
        MPI_Abort(MPI_COMM_WORLD, error_code); \
    } \
} while(false)

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    std::cout << "Hello, World! Rank: " << rank << ", size: " << world_size << std::endl;

    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
