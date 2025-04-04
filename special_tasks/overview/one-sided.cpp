#include <mpi_try.hpp>

struct Pipeline
{
    int stage;
    int data;

    Pipeline(int data) : stage(0), data(data) {}
};

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    MPI_Win win;
    Pipeline task(19);
    MPI_Win_create(&task, sizeof(Pipeline), sizeof(Pipeline), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);

    while(task.stage != world_size)
    {
        task.stage++;
        if(task.stage == rank + 1)
        {
            task.data *= 2;
            task.data += rank;
            std::cout << "Rank: " << rank << "/" << world_size << ", msg: " << task.data << std::endl;
            MPI_Put(&task, sizeof(Pipeline), MPI_BYTE, (rank + 1) % world_size, 
                0, sizeof(Pipeline), MPI_BYTE, win);
        }

        MPI_Win_fence(0, win);
    }

    MPI_Win_free(&win);
    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
