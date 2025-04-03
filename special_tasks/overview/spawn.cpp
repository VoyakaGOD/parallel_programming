#include <mpi_try.hpp>
#include <string>

#define AUX_COUNT 4

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    TRY(argc < 2, "You should enter maximum world size");
    int max_world_size = std::atoi(argv[1]);
    TRY((1 + AUX_COUNT) * world_size > max_world_size, 
        "The maximum world size is not large enough");

    MPI_Comm intercomm;
    int errcodes[AUX_COUNT] = {};
    MPI_Comm_spawn("./executables/spawn.aux", argv, AUX_COUNT, MPI_INFO_NULL, 
        0, MPI_COMM_SELF, &intercomm, errcodes);
    for(int j = 0; j < AUX_COUNT; j++)
    {
        TRY(errcodes[j], "Can't spawn new process");
        TRY(MPI_Send(&rank, 1, MPI_INT, j, 0, intercomm), "Can't send group id");
    }

    int msg = 19;

    if(rank != 0)
    {
        TRY(MPI_Recv(&msg, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive message from previous rank");
    }

    TRY(MPI_Send(&msg, 1, MPI_INT, 0, 0, intercomm), "Can't send msg to the zero child");
    TRY(MPI_Recv(&msg, 1, MPI_INT, AUX_COUNT - 1, 0, intercomm, MPI_STATUS_IGNORE),
            "Can't receive message from last AUX");
    std::cout << "Main process, Rank: " << rank << "/" << world_size << ", msg: " << msg << std::endl;

    if(rank != world_size - 1)
        TRY(MPI_Send(&msg, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD), "Can't send msg to next rank");

    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
