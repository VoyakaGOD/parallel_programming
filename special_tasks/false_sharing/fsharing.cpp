#include <require.hpp>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <omp.h>

#ifndef CACHE_LINE
    #define CACHE_LINE 64
#endif

typedef long long counter_t;

typedef struct
{
    counter_t value;
    char padding[CACHE_LINE - sizeof(counter_t)];
} padded_t;

double run_false_sharing(int nthreads, uint64_t iterations_per_thread)
{
    counter_t *counters = nullptr;
    posix_memalign(reinterpret_cast<void **>(&counters), CACHE_LINE, nthreads * sizeof(counter_t));
    require(counters, "Can't allocate memory for counters");
    memset(reinterpret_cast<void *>(counters), 0, nthreads * sizeof(counter_t));

    double start = omp_get_wtime();

    #pragma omp parallel num_threads(nthreads)
    {
        int tid = omp_get_thread_num();
        counter_t *counter = counters + tid;
        for (uint64_t i = 0; i < iterations_per_thread; i++)
            (*counter)++;
    }

    double end = omp_get_wtime();
    free((void*)counters);
    return end - start;
}

double run_no_false_sharing(int nthreads, uint64_t iterations_per_thread)
{
    padded_t *counters = new padded_t[nthreads];
    require(counters, "Can't allocate memory for counters");

    double start = omp_get_wtime();

    #pragma omp parallel num_threads(nthreads)
    {
        int tid = omp_get_thread_num();
        padded_t *counter = counters + tid;
        for (uint64_t i = 0; i < iterations_per_thread; i++)
            counter->value++;
    }

    double end = omp_get_wtime();
    delete[] counters;
    return end - start;
}

int main(int argc, char **argv)
{
    require(argc == 3, "fsharing [threads] [iterations]");
    int thc = atoi(argv[1]);
    int itc = atoi(argv[2]);
    std::cout << "False sharing:    " << run_false_sharing(thc, itc) << "s." << std::endl;
    std::cout << "No false sharing: " << run_no_false_sharing(thc, itc) << "s." << std::endl;
}
