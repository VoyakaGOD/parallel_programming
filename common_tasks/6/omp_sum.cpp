#include <require.hpp>
#include <iostream>
#include <iomanip>
#include <omp.h>

int main(int argc, char** argv)
{
    omp_set_num_threads(4);

    require(argc == 2, "You should enter N[number of elements in sum]");
    int N = std::atoi(argv[1]);
    double result = 0;

    #pragma omp parallel
    {
        int thread_id = omp_get_thread_num();
        int num_threads = omp_get_num_threads();
        int partition_size = N / num_threads;
        int last_id = num_threads - 1;
        int from = thread_id * partition_size;
        int to = (thread_id == last_id) ? N : ((thread_id + 1) * partition_size);
        // from [i = 1] to [i = N]
        from++;
        to++;

        double sum = 0;
        for(int i = from; i < to; i++)
            sum += 1.0f / i;

        #pragma omp atomic
        result += sum;
    }

    std::cout << std::setprecision(20);
    std::cout << "Total sum(from 1 to 1/" << N << "): " << result << std::endl;

    return 0;
}
