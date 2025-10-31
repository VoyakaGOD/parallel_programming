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
    for (int i = 1; i < ISIZE; i++)
        for (int j = 0; j < JSIZE - 1; j++)
            a[i][j] = sin(2 * a[i - 1][j + 1]); // Di = 1, Dj = -1
}

void do_parallel_omp(std::vector<std::vector<double>> &a)
{
    int total_diag = ISIZE + JSIZE - 2;

    // !!!cache misses!!!
    #pragma omp parallel for schedule(dynamic, 1)
    for(int k = 1; k < total_diag; k++)
    {
        int i_max = std::min(ISIZE - 1, k);
        for(int i = std::max(0, k - JSIZE + 1) + 1; i <= i_max; i++)
        {
            int j = k - i;
            a[i][j] = sin(2 * a[i - 1][j + 1]);
        }
    }
}

void do_parallel_omp_cache(std::vector<std::vector<double>> &a)
{
    for (int i = 1; i < ISIZE; i++)
    {
        #pragma omp parallel for
        for (int j = 0; j < JSIZE - 1; j++)
            a[i][j] = sin(2 * a[i - 1][j + 1]);
    }
}

void do_parallel_mpi(std::vector<std::vector<double>> &a)
{
    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");
    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");
    TRY((JSIZE-1) % world_size, "JSIZE-1 should be divisible by world_size for mpi");

    int partition_size = (JSIZE-1) / world_size;
    int j_from = rank * partition_size;
    int j_to = (rank + 1) * partition_size;

    for (int i = 1; i < ISIZE; i++)
    {
        for (int j = j_from; j < j_to; j++)
            a[i][j] = sin(2 * a[i - 1][j + 1]);
        void *sendbuf = (rank == 0) ? MPI_IN_PLACE : (a[i].data() + j_from);
        MPI_Gather(sendbuf, partition_size, MPI_DOUBLE, 
            a[i].data(), partition_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    }
}

int main(int argc, char **argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");
    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");
    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");
    std::vector<std::vector<double>> a(ISIZE, std::vector<double>(JSIZE));

    for(int i = 0; i < ISIZE; i++)
        for(int j = 0; j < JSIZE; j++)
            a[i][j] = 10 * i + j;

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
        case 'c':
            omp_set_num_threads(atoi(argv[1] + 1));
            #pragma omp parallel
            #pragma omp single
            std::cout << "cache, omp threads = " << omp_get_num_threads() << std::endl;
            for(int i = 0; i < ITCOUNT; i++)
                do_parallel_omp_cache(a);
            break;
        case 'm':
            if(rank == 0)
                std::cout << "mpi computers = " << world_size << std::endl;
            for(int i = 0; i < ITCOUNT; i++)
            {
                do_parallel_mpi(a);
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
        "c[threads] for cached omp, anything else for sequential computing..." << std::endl;
        exit(0);
    }
    double end = MPI_Wtime();

    if(rank == 0)
    {
        if(ISIZE * JSIZE <= 10000)
        {
            for (int i = 0; i < ISIZE; i++)
            {
                for (int j = 0; j < JSIZE; j++)
                    std::cout << a[i][j] << ' ';
                std::cout << std::endl;
            }
        }

        std::cout << "time " << (end - start) / ITCOUNT << "s." << std::endl;
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
}
