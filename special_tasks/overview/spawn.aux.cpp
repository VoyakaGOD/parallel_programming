#include <mpi_try.hpp>
#include <thread>
#include <string>

using namespace std::chrono_literals;

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    MPI_Comm parent;
    MPI_Comm_get_parent(&parent);

    int msg = 0;
    int group = 0;

    TRY(MPI_Recv(&group, 1, MPI_INT, 0, 0, parent, MPI_STATUS_IGNORE), "Can't receive group id");

    if(rank == 0)
    {
        TRY(MPI_Recv(&msg, 1, MPI_INT, 0, 0, parent, MPI_STATUS_IGNORE), "Can't receive msg from the parent");
    }
    else
    {
        TRY(MPI_Recv(&msg, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive msg from rank " + std::to_string(rank - 1));
    }

    msg *= 2;
    msg += rank;
    std::cout << "AUX[" << group << "], Rank: " << rank << "/" << world_size << ", msg: " << msg << std::endl;

    if(rank == world_size - 1)
    {
        TRY(MPI_Send(&msg, 1, MPI_INT, 0, 0, parent), "Can't send msg to the parent");
    }
    else
    {
        TRY(MPI_Send(&msg, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD),
                "Can't send msg to rank " + std::to_string(rank + 1));
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
