#include <iostream>
#include <omp.h>

int main()
{
    int num_threads = omp_get_max_threads();
    std::cout << "threads count = " << num_threads << std::endl;

    #pragma omp parallel
    {
        int xXx = 0;

#ifdef CPY // share new value of private variable if and only if CPY defined
        #pragma omp single copyprivate(xXx)
#else
        #pragma omp single
#endif
        xXx += 100;

        #pragma omp critical
        {
            xXx++;
            std::cout << "current xXx value = " << xXx << std::endl;
        }
    }

    return 0;
}
