#include <mpi_try.hpp>

template <typename TaskType>
void check(const char *name, TaskType task, int rank, int buff_size, int count)
{
    char *buffer = new char[buff_size];
    TRY(buffer == nullptr, "Can't allocate memory");

    double start = MPI_Wtime();
    int left = count;
    while((left--) > 0)
        task(buffer);
    double end = MPI_Wtime();

    if(rank == 0)
    {
        std::cout << name << ", buff: " << buff_size << ", count: " << count;
        std::cout << ", time/byte: " << ((end - start) * 1e9 / count / buff_size) << " ns" << std::endl;
    }

    delete[] buffer;
}

void check_MPI_send(int world_size, int rank, int buff_size, int count)
{
    check("send/recv", [=](char *buffer) {
        if(rank == 0)
        {
            for(int i = 1; i < world_size; i++)
            {
                TRY(MPI_Send(buffer, buff_size, MPI_CHAR, i, 0, MPI_COMM_WORLD), 
                    "Can't send message");
            }
        }
        else
        {
            TRY(MPI_Recv(buffer, buff_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE), 
                "Can't receive message");
        }
    }, rank, buff_size, count);
}

void check_MPI_broadcast(int rank, int buff_size, int count)
{
    check("broadcast", [=](char *buffer) {
        TRY(MPI_Bcast(buffer, buff_size, MPI_CHAR, 0, MPI_COMM_WORLD), "Can't send/recv broadcast");
    }, rank, buff_size, count);
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");
    
    if(rank == 0)
        std::cout << "accuracy: " << MPI_Wtick() << " s" << std::endl;
    
    check_MPI_send(world_size, rank, 1000000, 10);
    check_MPI_send(world_size, rank, 100000, 100);
    check_MPI_send(world_size, rank, 10000, 1000);
    check_MPI_send(world_size, rank, 1000, 10000);
    check_MPI_send(world_size, rank, 100, 100000);
    check_MPI_send(world_size, rank, 10, 1000000);
    check_MPI_send(world_size, rank, 1, 10000000);

    if(rank == 0)
        std::cout << std::endl;

    check_MPI_broadcast(rank, 1000000, 10);
    check_MPI_broadcast(rank, 100000, 100);
    check_MPI_broadcast(rank, 10000, 1000);
    check_MPI_broadcast(rank, 1000, 10000);
    check_MPI_broadcast(rank, 100, 100000);
    check_MPI_broadcast(rank, 10, 1000000);
    check_MPI_broadcast(rank, 1, 10000000);

    TRY(MPI_Finalize(), "Bad MPI finalization");
    return 0;
}
