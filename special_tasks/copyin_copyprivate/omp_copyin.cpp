#include <iostream>
#include <omp.h>

int xXx;
#pragma omp threadprivate(xXx)

int main()
{
    int num_threads = omp_get_max_threads();
    std::cout << "threads count = " << num_threads << std::endl;

    xXx = 100;

#ifdef CPY // copy value of threadprivate variable if and only if CPY defined
    #pragma omp parallel copyin(xXx)
#else
    #pragma omp parallel
#endif
    {
        #pragma omp critical
        std::cout << "current xXx value = " << xXx << std::endl;
    }

    return 0;
}
