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
    for (int i = 0; i < ISIZE-3; i++)
        for (int j = 4; j < JSIZE; j++)
            a[i][j] = sin(0.04*a[i+3][j-4]); // Di = -3, Dj = 4
}

void do_parallel_omp(std::vector<std::vector<double>> &a)
{
    std::vector<std::vector<double>> a_snapshot(ISIZE, std::vector<double>(JSIZE));
    a.swap(a_snapshot);

    #pragma omp parallel for
    for (int i = 0; i < ISIZE-3; i++)
        for (int j = 4; j < JSIZE; j++)
            a[i][j] = sin(0.04*a_snapshot[i+3][j-4]);
}

int main(int argc, char **argv)
{
    std::vector<std::vector<double>> a(ISIZE, std::vector<double>(JSIZE));

    for (int i = 0; i < ISIZE; i++)
        for (int j = 0; j < JSIZE; j++)
            a[i][j] = 10 * i + j;

    double start = omp_get_wtime();
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
        default:
            for(int i = 0; i < ITCOUNT; i++)
                do_sequential(a);
            break;
        }
    }
    else
    {
        std::cout << "first arg: o[threads] for omp, "
        "anything else for sequential computing..." << std::endl;
        exit(0);
    }
    double end = omp_get_wtime();

    if(ISIZE * JSIZE <= 1000)
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
