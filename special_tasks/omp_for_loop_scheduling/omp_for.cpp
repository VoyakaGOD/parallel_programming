#include <iostream>
#include <vector>
#include <omp.h>

#define OMP_PRAGMA(x) _Pragma(#x)

#define PRINT OMP_PRAGMA(omp for ordered) \
        for(int t = 0; t < num_threads; t++) \
        { \
            OMP_PRAGMA(omp ordered) \
            { \
                std::cout << "thread(" << thread_num << ") its:"; \
                for(int it : its) \
                    std::cout << " " << it; \
                std::cout << std::endl; \
            } \
        } \
        its.clear()

#define LOOP for(int i = 0; i < 65; i++) its.push_back(i)

int main()
{
    int num_threads = omp_get_max_threads();
    std::cout << "threads count = " << num_threads << std::endl;
    int num_procs = omp_get_num_procs();
    std::cout << "procs count = " << num_procs << std::endl;

    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        std::vector<int> its;

        #pragma omp single
        std::cout << "*    Without omp for directive:" << std::endl;
        LOOP;
        PRINT;

        #pragma omp single
        std::cout << "*    Default omp for directive:" << std::endl;
        #pragma omp for
        LOOP;
        PRINT;

        #pragma omp single
        std::cout << "*    Static scheduling with chunk=1:" << std::endl;
        #pragma omp for schedule(static, 1)
        LOOP;
        PRINT;

        #pragma omp single
        std::cout << "*    Static scheduling with chunk=4:" << std::endl;
        #pragma omp for schedule(static, 4)
        LOOP;
        PRINT;

        #pragma omp single
        std::cout << "*    Dynamic scheduling with chunk=1:" << std::endl;
        #pragma omp for schedule(dynamic, 1)
        LOOP;
        PRINT;

        #pragma omp single
        std::cout << "*    Dynamic scheduling with chunk=4:" << std::endl;
        #pragma omp for schedule(dynamic, 4)
        LOOP;
        PRINT;

        #pragma omp single
        std::cout << "*    Guided scheduling with chunk=1:" << std::endl;
        #pragma omp for schedule(guided, 1)
        LOOP;
        PRINT;

        #pragma omp single
        std::cout << "*    Guided scheduling with chunk=4:" << std::endl;
        #pragma omp for schedule(guided, 4)
        LOOP;
        PRINT;
    }

    return 0;
}
