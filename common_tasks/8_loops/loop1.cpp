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

struct parallel_task
{
    int rank;
    int world_size;
    int k_from;
    int k_to;
};

void do_parallel_mpi(std::vector<std::vector<double>> &a, const parallel_task &task)
{

    for(int k = task.k_from; k < task.k_to; k++)
    {
        for(int i = 1; i < ISIZE; i++)
        {
            int j = k - i;
            if(j < 0)
                break;

            a[i][j] = sin(2 * a[i - 1][j + 1]);
        }
    }

    // if(task.rank == 0)
    // {
    //     for(int i = 1; i < task.world_size; i++)
    //     {
    //         TRY(MPI_Recv(&partition, 1, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
    //             "Can't receive msg from rank " + std::to_string(i));
    //     }

    //     std::cout << std::setprecision(20);
    //     std::cout << "Total sum(from 1 to 1/" << N << "): " << sum << std::endl;
    // }
    // else
    // {
    //     TRY(MPI_Send(&a.data(), ISIZE*JSIZE, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD), "Can't send msg to rank 0");
    // }
}

int main(int argc, char **argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");
    std::vector<std::vector<double>> a(ISIZE, std::vector<double>(JSIZE));

    parallel_task task;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &task.rank), "Can't get rank of this process");
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &task.world_size), "Can't get total count of processes");

    int total_diag = ISIZE + JSIZE - 3; // from k = 2 to k = (ISIZE + JSIZE - 2)
    int partition_size = total_diag / task.world_size;
    int last_rank = task.world_size - 1;

    task.k_from = task.rank * partition_size;
    task.k_to = (task.rank == last_rank) ? total_diag : ((task.rank + 1) * partition_size);

    for (int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = ((i+j >= task.k_from) && (i+j<= task.k_to))? 10 * i + j : 0;

    double start = MPI_Wtime();
    if(argc > 1)
    {
        switch (argv[1][0])
        {
        case 'o':
            omp_set_num_threads(atoi(argv[1] + 1));
            std::cout << "omp threads = " << omp_get_num_threads() << std::endl;
            do_parallel_omp(a);
            break;
        case 'm':
            do_parallel_mpi(a, task);
            break;
        default:
            do_sequential(a);
            break;
        }
    }
    else
    {
        std::cout << "first arg: o[threads] for omp, m[threads] for mpi, "
        "anything else for sequential computing..." << std::endl;
        exit(0);
    }
    double end = MPI_Wtime();

    if(task.rank == 0)
    {
        if(ISIZE * JSIZE <= 1000)
        {
            for (int i = 0; i < ISIZE; i++)
            {
                for (int j = 0; j < JSIZE; j++)
                    std::cout << a[i][j] << ' ';
                std::cout << std::endl;
            }
        }

        std::cout << "time " << (end - start) << "s." << std::endl;
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
}
