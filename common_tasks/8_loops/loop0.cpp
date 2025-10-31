#include <mpi_try.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <omp.h>

#ifndef ISIZE
    #define ISIZE 5000
#endif
#ifndef JSIZE
    #define JSIZE 5000
#endif
#ifndef ITCOUNT
    #define ITCOUNT 20
#endif

void do_sequential(std::vector<std::vector<double>> &a)
{
    for(int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = sin(2*a[i][j]);       //Di = 0, Dj = 0
}

void do_parallel_omp(std::vector<std::vector<double>> &a)
{
    #pragma omp parallel for
    for(int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = sin(2*a[i][j]);
}

void do_parallel_mpi(std::vector<std::vector<double>> &a, int i_size)
{
    for(int i = 0; i < i_size; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = sin(2*a[i][j]);
}

int main(int argc, char **argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");
    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");
    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    int partition_size = (ISIZE-1) / world_size;
    int i_from = rank * partition_size;
    int last_rank = world_size - 1;
    int i_to = (rank == last_rank) ? ISIZE : (rank + 1) * partition_size;
    std::vector<std::vector<double>> a(i_to - i_from, std::vector<double>(JSIZE));

    for(int i = i_from; i < i_to; i++)
        for(int j = 0; j < JSIZE; j++)
            a[i - i_from][j] = 10 * i + j;

    double start = MPI_Wtime();
    if(argc > 1)
    {
        switch (argv[1][0])
        {
        case 'o':
            omp_set_num_threads(atoi(argv[1] + 1));
            #pragma omp parallel
            #pragma omp single
            std::cout << "omp threads = " << omp_get_num_threads() << std::endl;
            for(int i = 0; i < ITCOUNT; i++)
                do_parallel_omp(a);
            break;
        case 'm':
            if(rank == 0)
                std::cout << "mpi computers = " << world_size << std::endl;
            for(int i = 0; i < ITCOUNT; i++)
            {
                do_parallel_mpi(a, i_to - i_from);
                BARRIER;
            }
            break;
        default:
            for(int i = 0; i < ITCOUNT; i++)
                do_sequential(a);
            break;
        }
    }
    else
    {
        std::cout << "first arg: o[threads] for omp, m for mpi, "
        "anything else for sequential computing..." << std::endl;
        exit(0);
    }
    double end = MPI_Wtime();

    if(ISIZE * JSIZE <= 10000)
    {
        for (int i = 0; i < ISIZE; i++)
        {
            for (int j = 0; j < JSIZE; j++)
                std::cout << a[i][j] << ' ';
            std::cout << std::endl;
        }
    }

    if(rank == 0)
        std::cout << "time " << (end - start) / ITCOUNT << "s." << std::endl;

    TRY(MPI_Finalize(), "Bad MPI finalization");
}
