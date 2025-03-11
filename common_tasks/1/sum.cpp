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

    TRY((argc == 2) ? 0 : -1, "You should enter N[number of elements in sum]");
    
    int N = std::atoi(argv[1]);
    int partition_size = N / world_size;
    int last_rank = world_size - 1;

    int from = rank * partition_size;
    int to = (rank == last_rank) ? N : ((rank + 1) * partition_size);
    // from [i = 1] to [i = N]
    from++;
    to++;

    double sum = 0;
    for(int i = from; i < to; i++)
        sum += 1.0f / i;

    if(rank == 0)
    {
        double partition = 0;
        for(int i = 1; i < world_size; i++)
        {
            TRY(MPI_Recv(&partition, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
                "Can't receive msg from rank " + std::to_string(i));
            sum += partition;
        }

        std::cout << std::setprecision(20);
        std::cout << "Total sum(from 1 to 1/" << N << "): " << sum << std::endl;
    }
    else
    {
        TRY(MPI_Send(&sum, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD), "Can't send msg to rank 0");
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
