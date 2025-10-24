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
    int partition_size = JSIZE / world_size;
    int last_rank = world_size - 1;
    int j_from = rank * partition_size;
    int j_to = (rank == last_rank) ? JSIZE : ((rank + 1) * partition_size);
    if(rank == 0)
        std::cout << "mpi world size = " << world_size << std::endl;

    for (int j = j_from; j < j_to; j++)
    {
        for (int i = 0; i < ISIZE-1; i++)
        {
            a[i][j] = sin(0.1*a[i+1][j]);
            b[i][j] = a[i][j]*1.5;
        }
    }

    if (rank == 0)
    {
        for (int src = 1; src < world_size; src++)
        {
            int src_j_from = src * partition_size;
            int src_j_to = (src == last_rank) ? JSIZE : ((src + 1) * partition_size);
            int cols = src_j_to - src_j_from;

            std::vector<double> recv_a(ISIZE * cols);
            std::vector<double> recv_b(ISIZE * cols);
            MPI_Recv(recv_a.data(), ISIZE * cols, MPI_DOUBLE, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(recv_b.data(), ISIZE * cols, MPI_DOUBLE, src, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // for (int i = 0; i < ISIZE; i++)
            // {
            //     for (int j = 0; j < cols; j++)
            //     {
            //         a[i][src_j_from + j] = recv_a[i * cols + j];
            //         b[i][src_j_from + j] = recv_b[i * cols + j];
            //     }
            // }
        }
    }
    else
    {
        int cols = j_to - j_from;
        std::vector<double> send_a(ISIZE * cols);
        std::vector<double> send_b(ISIZE * cols);

        for (int i = 0; i < ISIZE; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                send_a[i * cols + j] = a[i][j_from + j];
                send_b[i * cols + j] = b[i][j_from + j];
            }
        }

        MPI_Send(send_a.data(), ISIZE * cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        MPI_Send(send_b.data(), ISIZE * cols, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }
    BARRIER;
}

int main(int argc, char **argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");
    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    std::vector<std::vector<double>> a(ISIZE, std::vector<double>(JSIZE));
    std::vector<std::vector<double>> b(ISIZE, std::vector<double>(JSIZE));
    for (int i = 0; i < ISIZE; i++)
            for (int j = 0; j < JSIZE; j++)
                a[i][j] = 10 * i + j;

    double start = MPI_Wtime();
    if(argc > 1)
    {
        switch (argv[1][0])
        {
        case 'p':
            do_parallel(a, b);
            break;
        default:
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
        if(ISIZE * JSIZE <= 1000)
        {
            for (int i = 0; i < ISIZE; i++)
            {
                for (int j = 0; j < JSIZE; j++)
                    std::cout << b[i][j] << ' ';
                std::cout << std::endl;
            }
        }

        std::cout << "time " << (end - start) << "s." << std::endl;
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
}
