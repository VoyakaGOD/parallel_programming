#include <iostream>
#include <unistd.h>
#include <thread>
#include <omp.h>

int main()
{
    omp_set_num_threads(4);

    std::cout << "Initial thread_id = " << std::this_thread::get_id() << std::endl;

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        #pragma omp critical
        {
            std::cout << "Hello, world! from thread " << thread_id << "/" << num_threads << std::endl;
            std::cout << "pid = " << getpid() << ", thread_id = " << std::this_thread::get_id() << std::endl;
        }
    }

    return 0;
}
