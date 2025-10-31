#include <mpi_try.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#ifndef ISIZE
    #define ISIZE 5000
#endif
#ifndef JSIZE
    #define JSIZE 5000
#endif
#ifndef ITCOUNT
    #define ITCOUNT 20
#endif

void do_sequential(std::vector<std::vector<double>> &a, std::vector<std::vector<double>> &b)
{
    for (int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = sin(0.1*a[i][j]);  // Di = 0, Dj = 0

    for (int i = 0; i < ISIZE-1; i++)
        for (int j = 0; j < JSIZE; j++)
            b[i][j] = a[i+1][j]*1.5;     // Di = -1, Dj = 0
}

void do_parallel(std::vector<std::vector<double>> &a, std::vector<std::vector<double>> &b)
{
    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");
    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");
    TRY((ISIZE-1) % world_size, "ISIZE-1 should be divisible by world_size for mpi");
    int partition_size = (ISIZE-1) / world_size;
    int i_from = rank * partition_size;
    int i_to = (rank + 1) * partition_size;
    if(rank == 0)
        std::cout << "mpi world size = " << world_size << std::endl;

    for(int i = i_from; i < i_to; i++)
    {
        for(int j = 0; j < JSIZE; j++)
        {
            a[i][j] = sin(0.1*a[i+1][j]);
            b[i][j] = a[i][j]*1.5;
        }
    }

    if(rank == 0)
    {
        for(int src = 1; src < world_size; src++)
        {
            int src_i_from = src * partition_size;
            int src_i_to = (src + 1) * partition_size;

            for(int i = i_from; i < i_to; i++)
            {
                MPI_Recv(a[i].data(), JSIZE, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(b[i].data(), JSIZE, MPI_DOUBLE, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }
    }
    else
    {
        for(int i = i_from; i < i_to; i++)
        {
            MPI_Send(a[i].data(), JSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
            MPI_Send(b[i].data(), JSIZE, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
        }
    }
}

int main(int argc, char **argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");
    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    std::vector<std::vector<double>> a(ISIZE, std::vector<double>(JSIZE));
    std::vector<std::vector<double>> b(ISIZE, std::vector<double>(JSIZE));
    for(int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = 10 * i + j;

    double start = MPI_Wtime();
    if(argc > 1)
    {
        switch (argv[1][0])
        {
        case 'p':
            for(int i = 0; i < ITCOUNT; i++)
            {
                do_parallel(a, b);
                BARRIER;
            }
            break;
        default:
            for(int i = 0; i < ITCOUNT; i++)
                do_sequential(a, b);
            break;
        }
    }
    else
    {
        if(rank == 0)
        {
            std::cout << "first arg: p for mpi, "
            "anything else for sequential computing..." << std::endl;
        }
        exit(0);
    }
    double end = MPI_Wtime();

    if(rank == 0)
    {
        if(ISIZE * JSIZE <= 10000)
        {
            for(int i = 0; i < ISIZE; i++)
            {
                for(int j = 0; j < JSIZE; j++)
                    std::cout << b[i][j] << ' ';
                std::cout << std::endl;
            }
        }

        std::cout << "time " << (end - start) / ITCOUNT << "s." << std::endl;
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
}
