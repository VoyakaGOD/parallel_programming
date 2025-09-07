#include <require.hpp>
#include <iostream>
#include <omp.h>

int main()
{
    int value = 0;

    // shared clause is not necessary
    #pragma omp parallel shared(value)
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        require(num_threads > 1, "You should create 2 threads at least");

        #pragma omp critical
        {
            value += thread_id;
            value *= 2;
            std::cout << "Current value: " << value << ", thread_id: " << thread_id << std::endl;
        }
    }

    return 0;
}
