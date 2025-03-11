#include <mpi_try.hpp>
#include <iomanip>
#include <string>

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    TRY((world_size < 2) ? -1 : 0, "You should create 2 processes at least");

    int value = 0;
    if(rank == 0)
    {
        std::cout << "Initial value: " << value << std::endl;
        TRY(MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD),
            "Can't send first msg to rank = 1");
    }

    int prev_rank = (rank == 0) ? (world_size - 1) : (rank - 1);
    TRY(MPI_Recv(&value, 1, MPI_INT, prev_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
        "Can't receive msg from rank = " + std::to_string(prev_rank));
    value += rank;
    value *= 2;
    std::cout << "Current value: " << value << ", rank: " << rank << std::endl;

    if(rank != 0)
    {
        TRY(MPI_Send(&value, 1, MPI_INT, (rank + 1) % world_size, 0, MPI_COMM_WORLD),
            "Can't send msg to rank = " + std::to_string(rank + 1));
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");

    return 0;
}
